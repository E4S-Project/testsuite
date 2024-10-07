#!/usr/bin/env python3
import os
import sys
import argparse
import subprocess
import datetime
import multiprocessing
import math
import signal
import shlex


#This program searches through directories in a non-recursive manner. Upon finding
# a subdirectory containing run.sh, it adds it to the worker queue. Each worker
# proccess then calls iterate_files.sh, which calls setup.sh, clean.sh compile.sh and run.sh.
# Outputs are either printed to the screen in a standard manner or in a json form, --json.
# Options for a timeout time are given, with a default of no timeout.
# Tests can be ran on a compute node given the option --scheduler, and if necessary 
# --scheduler-flags="your-flags-here"

#NOTE: json goes through stderr and other output goes through stdout

#Notes for both functions below:
#JSON output requires removal of "\n", hence the output.replace calls.
#In the case of a timeout, the functions add '"timeout"}}' or '{Setup\Compile\Run} Timed out' to the output,
#depending on whether or not it is json

# Function to execute shell scripts and handle errors, asynchronously by the worker queue
def async_run_command(command, current_dir_with_symlinks, timeout=False, print_json=False, timestamp=""):
    # Change to the specified directory and update environment variables
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this and scripts such as setup.sh/compile.sh require this to be set
    os.environ['testdir'] = current_dir_with_symlinks
    os.environ['testtime'] = timestamp
    stdout = None
    stderr = None
    return_code = None
    command = shlex.split(command)
    try:
        result = None
        # Run the command with a timeout
        if timeout:
            result = subprocess.run(
                command, 
                timeout=timeout,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
        else:
            result = subprocess.run(
                command, 
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
        stdout = result.stdout.decode().strip()
        stderr = result.stderr.decode().strip()
        return_code = result.returncode
        if print_json:
            stderr = stderr.replace("\n","") + "\n"

    except KeyboardInterrupt:
        # This will catch Control-C and make sure the terminal is reset properly after handling the interrupt
        print("\nProcess interrupted, cleaning up...")
        signal.signal(signal.SIGINT, signal.SIG_DFL)
    except subprocess.TimeoutExpired as e:
        # If the process times out, return the partial output and a timeout indicator
        stdout = e.stdout.decode().strip() if e.stdout else ""  # Get any output so far
        stderr = e.stderr.decode().strip() if e.stderr else ""
        if print_json:
            stderr = stderr.replace("\n","")
            return_code = -1
            stderr  += '"timeout"}},\n'
        else:
            failed_step = stdout.split("\n")[-1].split()[0] #Gets the last step that it failed at
            return_code = -1
            stdout += f"\n{failed_step} Timed out "

    return return_code, stdout, stderr

# Function to execute shell scripts and handle errors, asynchronously by the worker queue, on compute nodes
def scheduler_async_run_command(command,current_dir_with_symlinks, scheduler="", scheduler_flags="", timeout=False, print_json=False, timestamp=""):
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this and scripts such as  setup.sh/compile.sh require this to be set
    os.environ['testdir']=current_dir_with_symlinks
    os.environ['testtime'] = timestamp

    if timeout:
        minutes = timeout // 60
        seconds = timeout % 60
        scheduler_flags=f"-t {minutes}:{seconds} {scheduler_flags}"
        # Timeout is handled in scheduler instead of of subproc.run, if it fails it doesn't
        # raise an error, just returns an error code. Tiemout is handled in scheduler so as
        # to not have to deal with it timing out because it takes a long time to get 
        # job allocations. 

    scheduler_command = None
    if scheduler == "slurm":
        scheduler_flags = f"--exclusive -N 1 -Q --quit-on-interrupt {scheduler_flags}"
        # -Q quiets slurm output, and --quit-on-interrupt I think aids in cancelling
        scheduler_command = f"srun {scheduler_flags} {scheduler_flags} {command}"
    elif scheduler == "qsub":
        pass

    scheduler_command = shlex.split(scheduler_command)

    result = None
    try:
        result = subprocess.run(
                scheduler_command,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
        )
    except KeyboardInterrupt:
        # This will catch Control-C and make sure the terminal is reset properly after handling the interrupt
        print("\nProcess interrupted, cleaning up...")
        signal.signal(signal.SIGINT, signal.SIG_DFL)

    stdout = result.stdout.decode().strip()
    stderr = result.stderr.decode().strip()

    new_stderr_list = []
    new_stdout_list = [] 
    if "slurmstepd" in stderr: #Srun timeout
        for line in stderr.split("\n"):
            if "slurmstepd" in line: #remove this line
                pass
            elif "srun" in line:
                pass
            else:
                new_stderr_list.append(line)
        if print_json:
            new_stderr_list.append('"timeout"}},')
            stderr = "".join(new_stderr_list)
        else:
            new_stdout_list = stdout.split("\n")
            failed_step = new_stdout_list[-1].split()[0] #Gets the last step that it failed at
            new_stdout_list.append(f"{failed_step} Timed out ")
            stdout = "\n".join(new_stdout_list)
            stderr = "\n".join(new_stderr_list)
    elif "srun" in stderr: #Srun timeout
        for line in stderr.split("\n"):
            if "srun" in line:
                pass
            else:
                new_stderr_list.append(line)
        stderr = "".join(new_stderr_list)
    elif print_json:
        stderr = stderr.replace("\n","")
    stderr += "\n"

    return result.returncode, stdout, stderr

# Function to iterate through directories in a non-recursive manner, adding calls to iterate_files.sh to a worker queue.
#Given a directory, this finds all sub directories that contain a run.sh, and then the worker processes go through each 
#sub directory calling setup.sh, compile.sh, run.sh.
def iterate_directories(testdir, processes=4, scheduler=False, scheduler_flags="", print_json=False, print_logs=False, e4s_print_color=True, skip_to="",skip_if="",test_only="",timeout=False, json_name=""):
    final_ret = 0
    results = [] #This maintains the order of prints and aynchronous job calls.
                 #It contains jobs through the results.append... lines below
                 #AND it contains strings that get printed in order. See the results for loop below for more clarification
    pool = multiprocessing.Pool(processes=processes)

    if not os.path.isdir(testdir):
        raise NotADirectoryError(f"{testdir} is not a directory")

    testdir = os.path.abspath(testdir)               #Note everything should be done with full paths,
                                                     #and only reduced to a basename later
    os.environ['testdir']=os.path.basename(testdir)
    
    # Use a stack to simulate recursion
    stack = [testdir]

    testsuite_dir = os.path.dirname(os.path.realpath(__file__)) #this is the testsuite directory

    # Begin JSON output if needed, make directory if needed
    os.makedirs(os.path.join(testsuite_dir,"json-outputs"), exist_ok=True)
    json_output_file = None
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
    os.environ['testtime'] = timestamp

    if print_json:
        if json_name == "":
            json_output_file = os.path.join(testsuite_dir,'json-outputs',f"testsuite-{timestamp}.json")
        elif json_name != "":
            json_output_file = os.path.join(testsuite_dir,'json-outputs',f"{json_name}-{timestamp}.json")
        with open(json_output_file,"a+") as file:
            file.write("[")
            file.close() #Since this program might run for a long time, close after writing to ensure no funny file business

    iterate_files_sh = os.path.join(testsuite_dir,'iterate_files.sh')

    while stack:
        current_dir_with_symlinks = stack.pop(0)
        os.chdir(current_dir_with_symlinks)

        # Check if there is a run.sh script to execute
        if os.path.exists(os.path.join(current_dir_with_symlinks, "run.sh")):
            if scheduler == False:
                results.append(pool.apply_async(async_run_command,  (iterate_files_sh, current_dir_with_symlinks, timeout, print_json, timestamp)))
            else:
                results.append(pool.apply_async(scheduler_async_run_command,  (iterate_files_sh, current_dir_with_symlinks, scheduler, scheduler_flags, timeout, print_json, timestamp)))
            #Call it with symlinks so that the .sh scripts know which test to run
        else:
            if print_json == False:
                results.append("===\n" + os.path.basename(current_dir_with_symlinks))

            files = sorted(os.listdir("."))
            # Iterate through subdirectories
            for d in files:
                if os.path.isdir(d):
                    if skip_to and d < skip_to:
                        continue
                    if skip_if and skip_if in d:
                        continue
                    if test_only and not (os.path.basename(d) in test_only):
                        continue
                    # Push the directory onto the stack instead of recursive call
                    stack.append(os.path.join(current_dir_with_symlinks, d))

    pool.close()
    
    for r in results: #If r is a string, print it, if r is not a string, it represents a 
                      #results from an asynchronous job call, and thus process its output

        if type(r) != type(""): 
            return_tuple = r.get()
            return_stdout = return_tuple[1]
            return_stderr = return_tuple[2]
            
            print(return_stdout)
            if print_json:
                if r == results[-1]: #Final json string, remove ,\n
                    return_stderr=return_stderr.rstrip(",\n")
                with open(json_output_file,"a+") as file:
                    file.write(return_stderr)
                    file.close()

            if return_tuple[0] != 0:
                final_ret += 1
        else:
            print(r)
    pool.join()
    # End JSON output if needed
    if print_json:
        with open(json_output_file,"a+") as file:
            file.write("]")
            file.close()
    print("Total number of failed tests: %d" % final_ret)

    return final_ret


    # Main function to parse arguments and execute the script
def main():

    # Argument parsing
    parser = argparse.ArgumentParser(description='Run all tests in the specified directory.')
    parser.add_argument('directory', nargs='?', default='validation_tests', help='Test directory to use')
    parser.add_argument('--json', action='store_true', help='Print JSON output.')
    parser.add_argument('--json-name', default="", help='Optional name for json output')
    parser.add_argument('--print-logs', action='store_true', help='Print all logs.')
    parser.add_argument('--settings', type=str, help='Path to settings file', default="settings.sh")
    parser.add_argument('--color-off', action='store_false', dest='e4s_print_color', help='Disable color output.')
    parser.add_argument('--skip-to', type=str, help='Skip to specified test.')
    parser.add_argument('--skip-if', type=str, help='Skip tests with the given substring in the directory name.')
    parser.add_argument('--test-only', type=str, help='Run only specified tests.')
    parser.add_argument('--scheduler', choices=['slurm'], help='Enable scheduler mode, which allows tests to be submitted and executed on job queues. Command line option takes precedence over settings', default="")
    parser.add_argument('--scheduler-flags', type=str, help="Optional flags for scheduler, such as the account code")
    parser.add_argument('--processes', type=int, default=4, help='Run tests on multiple proccesses, default is 4')
    parser.add_argument('--timeout', type=int, default=0, help='Timeout value in seconds for each test, default is 0 for no timeout')
    args = parser.parse_args()

    basedir = args.directory
    
    #These get set as environment variables for iterate_files.sh and setup.sh scripts
    print_json = args.json
    json_name = args.json_name
    if (json_name != "") and args.json == False:
        raise ValueError("--json must be set if you wish to use --json-name")
    print_logs = args.print_logs
    e4s_print_color = args.e4s_print_color
    os.environ['print_logs'] = str(print_logs).lower()
    os.environ['print_json'] = str(print_json).lower()
    os.environ['e4s_print_color'] = str(e4s_print_color).lower()
     
    #Set the settings environs
    os.environ["TESTSUITE_SETTINGS_FILE"] = os.path.realpath(args.settings)
    settings_lines = open(args.settings, 'r').readlines()
    for line in settings_lines:
        # Skip empty lines and comments
        if not line or line.startswith('#'):
            continue

        # Process lines that start with 'export '
        if line.startswith('export'):
            # Remove 'export ' from the beginning
            line = line[len('export'):]

            # Use shlex to handle quoted strings and shell-like syntax
            tokens = shlex.split(line, posix=True)[0].split("=")
            value = str(tokens[1]) or ""
            os.environ[tokens[0]] = value


    #These control which tests are ran
    skip_to = args.skip_to or ""
    skip_if = args.skip_if or ""
    if args.test_only:
        test_only=args.test_only.split()
    else:
        test_only = ""
                
    timeout = int(args.timeout)

    #These are for the multiprocessing/scheduler functionality
    scheduler = args.scheduler or os.environ.get("SCHEDULER", None) or False #Sets command line precedence over settings file (which is what sets SCHEDULER)
    scheduler_flags = args.scheduler_flags or os.environ.get("SCHEDULER_FLAGS", None) or "" if scheduler else ""

    processes = args.processes


    # Call the main directory iteration function
    final_ret = iterate_directories(basedir, processes=processes, scheduler=scheduler, scheduler_flags=scheduler_flags, print_json=print_json, print_logs=print_logs, e4s_print_color=e4s_print_color, skip_to=skip_to, skip_if=skip_if, test_only=test_only, timeout=timeout, json_name=json_name)

    # Exit with the final return code
    sys.exit(final_ret)

# Execute the main function if the script is run directly
if __name__ == "__main__":
    main()


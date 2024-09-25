import os
import sys
import argparse
import subprocess
import datetime
import multiprocessing
import math
import signal


#This program searches through directories in a non-recursive manner. Upon finding
# a subdirectory containing run.sh, it adds it to the worker queue. Each worker
# proccess then calls iterate_files.sh, which calls setup.sh, clean.sh compile.sh and run.sh.
# Outputs are either printed to the screen in a standard manner or in a json form, --json.
# Options for a timeout time are given, with a standard time of 600 seconds.
# Tests can be ran on a compute node given the option --slurm, and if necessary 
# --slurm-flags="your-flags-here"

#UNTESTED FEATURES: skip-to/if, test-only, print_logs

#NOTE: json goes through stderr and other output goes through stdout

#Notes for both functions below:
#JSON output requires removal of "\n", hence the output.replace calls.
#In the case of a timeout, the functions add '"timeout"}}' or '{Setup\Compile\Run} Timed out' to the output,
#depending on whether or not it is json

# Function to execute shell scripts and handle errors, asynchronously by the worker queue
def async_run_command(command, current_dir_with_symlinks, timeout=False, print_json=False):
    # Change to the specified directory and update environment variables
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this and scripts such as setup.sh/compile.sh require this to be set
    os.environ['testdir'] = current_dir_with_symlinks
    
    try:
        result = None
        # Run the command with a timeout
        if timeout:
            result = subprocess.run(
                command, 
                shell=True, 
                timeout=timeout,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
        else:
            result = subprocess.run(
                command, 
                shell=True, 
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
        stdout = result.stdout.decode().strip()
        stderr = result.stderr.decode().strip()
        if print_json:
            stderr = stderr.replace("\n","")
        return result.returncode, stdout, stderr

    except subprocess.TimeoutExpired as e:
        # If the process times out, return the partial output and a timeout indicator
        stdout = e.stdout.decode().strip() if e.stdout else ""  # Get any output so far
        stderr = e.stderr.decode().strip() if e.stderr else ""
        if print_json:
            stderr = stderr.replace("\n","")
            return -1, stdout, stderr  + '"timeout"}},'
        else:
            failed_step = stdout.split("\n")[-1].split()[0] #Gets the last step that it failed at
            return -1, f"{stdout}\n{failed_step} Timed out "
    except KeyboardInterrupt:
        # This will catch Control-C and make sure the terminal is reset properly after handling the interrupt
        print("\nProcess interrupted, cleaning up...")
        signal.signal(signal.SIGINT, signal.SIG_DFL)

# Function to execute shell scripts and handle errors, asynchronously by the worker queue, on compute nodes
def srun_async_run_command(command,current_dir_with_symlinks, slurm_flags="", timeout=False, print_json=False):
    srun_flags = None
    if timeout:
        minutes = timeout // 60
        seconds = timeout % 60
        srun_flags=f"--exclusive -N 1 -t {minutes}:{seconds} -Q --quit-on-interrupt"
        # Timeout is handled in srun instead of of subproc.run, if it fails it doesn't
        # raise an error, just returns an error code. Tiemout is handled in slurm so as
        # to not have to deal with it timing out because it takes a long time to get 
        # job allocations. 
        # -Q quiets slurm output, and --quit-on-interrupt I think aids in cancelling
    else:
        srun_flags=f"--exclusive -N 1 -Q --quit-on-interrupt" 

    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this and scripts such as 
                                                #setup.sh/compile.sh require this to be set
    os.environ['testdir']=current_dir_with_symlinks
    srun_command = f"srun {srun_flags} {slurm_flags} {command}"

    result = None
    try:
        result = subprocess.run(
                srun_command,
                shell=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT
        )
    except KeyboardInterrupt:
        # This will catch Control-C and make sure the terminal is reset properly after handling the interrupt
        print("\nProcess interrupted, cleaning up...")
        signal.signal(signal.SIGINT, signal.SIG_DFL)

    output = result.stdout.decode().strip()

    new_output = [] 
    if "slurmstepd" in output: #When the srun timeout, it puts this in the output, so here I prune it and any line containing srun.
        for line in output.split("\n"):
            if "srun" in line:
                pass
            elif "slurmstepd" in line:
                pass
            else:
                new_output.append(line)
        if print_json:
            new_output.append('"timeout"}},')
        else:
            failed_step = new_output[-1].split()[0] #Gets the last step that it failed at
            new_output.append(f"{failed_step} Timed out ")

    elif "srun: error" in output: #Failed run
        new_output = []
        for line in output.split("\n"):
            if "srun" in line:
                pass
            else:
                new_output.append(line)
    else:
        new_output = output.split("\n")

    if print_json:
        output = "".join(new_output)
    else:
        output = "\n".join(new_output)

    return result.returncode, output

# Function to iterate through directories in a non-recursive manner, adding calls to iterate_files.sh to a worker queue.
#Given a directory, this finds all sub directories that contain a run.sh, and then the worker processes go through each 
#sub directory calling setup.sh, compile.sh, run.sh.
def iterate_directories(testdir, processes=4, slurm=False, slurm_flags="", print_json=False, print_logs=False, e4s_print_color=True, skip_to="",skip_if="",test_only="",timeout=False, json_name=""):
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

    # Begin JSON output if needed
    json_output_file = None
    timestamp = timestamp = datetime.datetime.now().strftime("Y%YM%mD%dH%HM%MS%S")

    if print_json:
        if json_name == "":
            json_output_file = os.path.join(os.path.dirname(os.path.realpath(__file__)),'json-outputs',f"testsuite-{timestamp}.json")
        elif json_name != "":
            json_output_file = os.path.join(os.path.dirname(os.path.realpath(__file__)),'json-outputs',f"{json_name}-{timestamp}.json")
        with open(json_output_file,"a+") as file:
            file.write("[")
            file.close()

    iterate_files_sh = os.path.join(os.path.dirname(os.path.realpath(__file__)),'iterate_files.sh')

    while stack:
        current_dir_with_symlinks = stack.pop(0)
        os.chdir(current_dir_with_symlinks)
        current_dir = os.getcwd() #I do this to get the full path, but it doesnt have the symlinks in it, hence why I keep current dir with symlinks

        # Check if there is a run.sh script to execute
        if os.path.exists(os.path.join(current_dir, "run.sh")):
            if slurm == False:
                results.append(pool.apply_async(async_run_command,  (iterate_files_sh, current_dir_with_symlinks, timeout, print_json)))
            else:
                results.append(pool.apply_async(srun_async_run_command,  (iterate_files_sh, current_dir_with_symlinks, slurm_flags, timeout, print_json)))
            #Call it with symlinks so that the .sh scripts know which test to run
        else:
            if print_json == False:
                results.append("===\n" + os.path.basename(current_dir))

            files = sorted(os.listdir("."))
            # Iterate through subdirectories
            for d in files:
                if os.path.isdir(d):
                    if skip_to and d < skip_to:
                        continue
                    if skip_if and skip_if in d:
                        continue
                    if test_only and os.path.basename(d) in test_only:
                        continue
                    # Push the directory onto the stack instead of recursive call
                    stack.append(os.path.join(current_dir, d))

    pool.close()
    
    for r in results: #If r is a string, print it, if r is not a string, it represents a 
                      #results from an asynchronous job call, and thus process its output

        if type(r) != type(""): 
            return_tuple = r.get()
            return_stdout = return_tuple[1]
            return_stderr = return_tuple[2]
            if r == results[-1] and print_json: #Final json has a ',' at the end of it lol
                return_stderr = return_stderr[:-1]
            
            print(return_stdout)
            if print_json:
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
    parser.add_argument('--settings', type=str, help='Path to settings file')
    parser.add_argument('--color-off', action='store_false', dest='e4s_print_color', help='Disable color output.')
    parser.add_argument('--skip-to', type=str, help='Skip to specified test.')
    parser.add_argument('--skip-if', type=str, help='Skip tests with the given substring in the directory name.')
    parser.add_argument('--test-only', type=str, help='Run only specified tests.')
    parser.add_argument('--slurm', action='store_true', help='Enable SLURM mode')
    parser.add_argument('--slurm_flags', type=str, help="Optional flags for slurm, such as the account code")
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
     

    #These control which tests are ran
    skip_to = args.skip_to or ""
    skip_if = args.skip_if or ""
    test_only = args.test_only or ""
    timeout = int(args.timeout)

    #These are for the multiprocessing/slurm functionality
    slurm_flags = args.slurm_flags or ""
    slurm = args.slurm 
    processes = args.processes


    # Call the main directory iteration function
    final_ret = iterate_directories(basedir, processes=processes, slurm=slurm, slurm_flags=slurm_flags, print_json=print_json, print_logs=print_logs, e4s_print_color=e4s_print_color, skip_to=skip_to, skip_if=skip_if, test_only=test_only, timeout=timeout, json_name=json_name)

    # Exit with the final return code
    sys.exit(final_ret)

# Execute the main function if the script is run directly
if __name__ == "__main__":
    main()


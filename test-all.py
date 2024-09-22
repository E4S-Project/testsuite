import os
import sys
import argparse
import subprocess
import datetime
import time
import multiprocessing
import math


#ASSUMPTIONS: THAT TESTSUITE IS IN THE HOEM DIRECTORY

# Initialize global variables
# Function to execute shell scripts and handle errors
def run_command(command, timeout=600):
    """ Run a shell command and return the output and status code """
    #result = subprocess.run(command, shell=True)# This will not capture output
    result = subprocess.run(command, shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT, timeout=timeout)
    output = result.stdout.decode().strip()
    return result.returncode, output

def async_run_command(command, current_dir_with_symlinks, timeout=600, print_json=False):
    """Run a shell command and return the output and status code, handle timeouts."""
    
    # Change to the specified directory and update environment variables
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD'] = current_dir_with_symlinks  # Update the working directory in the environment
    os.environ['testdir'] = current_dir_with_symlinks

    try:
        # Run the command with a timeout
        result = subprocess.run(
            command, 
            shell=True, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.STDOUT, 
            timeout=timeout
        )
        output = result.stdout.decode().strip()
        if print_json:
            output = output.replace("\n","")
        return result.returncode, output

    except subprocess.TimeoutExpired as e:
        # If the process times out, return the partial output and a timeout indicator
        output = e.stdout.decode().strip() if e.stdout else ""  # Get any output so far
        if print_json:
            output = output.replace("\n","")
            return -1, output + '"timeout"}},'
        else:
            failed_step = output.split("\n")[-1].split()[0] #Gets the last step that it failed at
            return -1, f"{output}\n{failed_step} Timed out "

def srun_async_run_command(command,current_dir_with_symlinks, slurm_flags="", timeout=600, print_json=False):
    """ Run a shell command in a node and return the output and status code """
    #srun_flags=f"--exclusive -N 1 -t {math.ceil(timeout/60)} -Q --quit-on-interrupt" #Timeout is handled in srun instead of of subproc.run, if it fails it doesn't raise an error, just returns a nonzero return code
    srun_flags=f"--exclusive -N 1 -t 0:1 -Q --quit-on-interrupt" #Timeout is handled in srun instead of of subproc.run, if it fails it doesn't raise an error, just returns a nonzero return code
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this
    os.environ['testdir']=current_dir_with_symlinks
    srun_command = f"srun {srun_flags} {slurm_flags} {command}"

    result = subprocess.run(srun_command, shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    output = result.stdout.decode().strip()

    new_output = [] 
    if "slurmstepd" in output: #Timed out
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

# Function to iterate through directories in a parallel non-recursive manner
def iterate_directories(testdir, processes=4, slurm=False, slurm_flags="", print_json=False, print_logs=False, e4s_print_color=True, skip_to="",skip_if="",test_only="",timeout=600):
    final_ret = 0
    results = [] #This maintains the order of prints and aynchronous job calls.
                 #It contains jobs through the results.append... lines below
                 #And it contains strings that get printed in order. See the results for loop below for more clarification
    pool = multiprocessing.Pool(processes=processes)

    if not os.path.isdir(testdir):
        raise NotADirectoryError(f"{testdir} is not a directory")

    testdir = os.path.abspath(testdir)               #Note everything should be done with full paths,
                                                     #and only reduced to a basename later
    os.environ['testdir']=os.path.basename(testdir)
    
    # Use a stack to simulate recursion
    stack = [testdir]

    # Begin JSON output if needed
    if print_json:
        print("[",end="")

    while stack:
        current_dir_with_symlinks = stack.pop(0)
        os.chdir(current_dir_with_symlinks)
        current_dir = os.getcwd() #I do this to get the full path, but it doesnt have the symlinks in it, hence why I keep current dir with symlinks

        # Check if there is a run.sh script to execute
        if os.path.exists(os.path.join(current_dir, "run.sh")):
            if slurm == False:
                results.append(pool.apply_async(async_run_command,  (os.path.join(os.path.dirname(os.path.realpath(__file__)),'iterate_files.sh'), current_dir_with_symlinks, timeout, print_json)))
            else:
                results.append(pool.apply_async(srun_async_run_command,  (os.path.join(os.path.dirname(os.path.realpath(__file__)),'iterate_files.sh'), current_dir_with_symlinks, slurm_flags, timeout, print_json)))
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


    for r in results: #If r is a string, print it, if r is not a string, it represents a 
                      #results from an asynchronous job call, and thus process its output

        if type(r) != type(""): 
            return_tuple = r.get()
            return_string = return_tuple[1]
            if r == results[-1] and print_json: #Final return value has a ', ' at the end of it lol
                return_string = return_string[:-1]
            print(return_string,end="" if print_json else "\n")
            if return_tuple[0] != 0:
                final_ret += 1
        else:
            print(r)

    # End JSON output if needed
    if print_json:
        print("]")
    else:
        print("Total number of failed tests: %d" % final_ret)

    return final_ret

# Main function to parse arguments and execute the script
def main():

    # Argument parsing
    parser = argparse.ArgumentParser(description='Run all tests in the specified directory.')
    parser.add_argument('directory', nargs='?', default='validation_tests', help='Test directory to use')
    parser.add_argument('--json', action='store_true', help='Print JSON output.')
    parser.add_argument('--print-logs', action='store_true', help='Print all logs.')
    parser.add_argument('--settings', type=str, help='Path to settings file')
    parser.add_argument('--color-off', action='store_false', dest='e4s_print_color', help='Disable color output.')
    parser.add_argument('--skip-to', type=str, help='Skip to specified test.')
    parser.add_argument('--skip-if', type=str, help='Skip tests with the given substring in the directory name.')
    parser.add_argument('--test-only', type=str, help='Run only specified tests.')
    parser.add_argument('--slurm', action='store_true', help='Enable SLURM mode')
    parser.add_argument('--slurm_flags', type=str, help="Optional flags for slurm, such as the account code")
    parser.add_argument('--processes', type=int, default=4, help='Run tests on multiple proccesses, default is 4')
    parser.add_argument('--timeout', type=int, default=600, help='Timeout value in seconds for each test, default is 600')
    args = parser.parse_args()

    basedir = args.directory
    
    #These get set as environment variables for iterate_files.sh and setup.sh scripts
    print_json = args.json
    print_logs = args.print_logs
    e4s_print_color = args.e4s_print_color
    os.environ['print_logs'] = str(print_logs).lower()
    os.environ['print_json'] = str(print_json).lower()
    os.environ['e4s_print_color'] = str(e4s_print_color).lower()
 

    #These control which tests are ran
    skip_to = args.skip_to or ""
    skip_if = args.skip_if or ""
    test_only = args.test_only or ""
    timeout = args.timeout

    #These are for the multiprocessing/slurm functionality
    slurm_flags = args.slurm_flags or ""
    slurm = args.slurm 
    processes = args.processes


    # Call the main directory iteration function
    final_ret = iterate_directories(basedir, processes=processes, slurm=slurm, slurm_flags=slurm_flags, print_json=print_json, print_logs=print_logs, e4s_print_color=e4s_print_color, skip_to=skip_to, skip_if=skip_if, test_only=test_only, timeout=timeout)

    # Exit with the final return code
    sys.exit(final_ret)

# Execute the main function if the script is run directly
if __name__ == "__main__":
    main()


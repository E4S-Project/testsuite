import os
import sys
import argparse
import subprocess
import datetime
import time
import multiprocessing


#ASSUMPTIONS: THAT TESTSUITE IS IN THE HOEM DIRECTORY

# Initialize global variables
final_ret = 0
print_json = False
print_logs = False
basedir = 'validation_tests'
e4s_print_color = True
skip_to = ""
test_only = ""
skip_if = ""
slurm = False
slurm_flags = ""
testtime = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")

# Function to execute shell scripts and handle errors
def run_command(command, timeout=600):
    """ Run a shell command and return the output and status code """
    #result = subprocess.run(command, shell=True)# This will not capture output
    result = subprocess.run(command, shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT, timeout=timeout)
    output = result.stdout.decode().strip()
    return result.returncode, output

def async_run_command(command,current_dir_with_symlinks, timeout=600):
    """ Run a shell command and return the output and status code """
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this
    os.environ['testdir']=current_dir_with_symlinks
    result = subprocess.run(command, shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT, timeout=timeout)
    output = result.stdout.decode().strip()
    return result.returncode, output

def srun_async_run_command(command,current_dir_with_symlinks, timeout=600):
    """ Run a shell command in a node and return the output and status code """
    srun_flags=f"--exclusive -N 1 -t {timeout}" #Timeout is handled in srun instead of of subproc.run
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this
    os.environ['testdir']=current_dir_with_symlinks
    srun_command = f"srun {srun_flags} {os.environ['slurm_flags']} {command}"
    #print(srun_command)
    result = subprocess.run(srun_command, shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    output = result.stdout.decode().strip()
    return result.returncode, output

# Function to iterate through directories in a parallel non-recursive manner
def iterate_directories(testdir, processes=4):
    final_ret = 0
    results = []
    pool = multiprocessing.Pool(processes=processes)

    if not os.path.isdir(testdir):
        raise NotADirectoryError(f"{testdir} is not a directory")

    testdir = os.path.abspath(testdir)
    os.environ['testdir']=os.path.basename(testdir)
    
    # Use a stack to simulate recursion
    stack = [testdir]

    while stack:
        current_dir_with_symlinks = stack.pop(0)
        os.chdir(current_dir_with_symlinks)
        current_dir = os.getcwd() #I do this to get the full path, but it doesnt have the symlinks in it, hence why I keep current dir with symlinks

        # Check if there is a run.sh script to execute
        if os.path.exists(os.path.join(current_dir, "run.sh")):
            if slurm == False:
                results.append(pool.apply_async(async_run_command,  (os.path.join(os.path.dirname(os.path.realpath(__file__)),'iterate_files.sh'), current_dir_with_symlinks)))
            else:
                results.append(pool.apply_async(srun_async_run_command,  (os.path.join(os.path.dirname(os.path.realpath(__file__)),'iterate_files.sh'), current_dir_with_symlinks)))
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


    for r in results:
        if type(r) != type(""): #If there is some error it needs to be passed along here
            print(r.get()[1],end="" if print_json else "\n")
            if r.get()[0] != 0:
                final_ret += 1
        else:
            print(r)

    if print_json == False:
        print("Total number of failed tests: %d" % final_ret)
    return final_ret

# Main function to parse arguments and execute the script
def main():
    global print_json, print_logs, basedir, e4s_print_color, skip_to, test_only, skip_if, slurm, slurm_flags

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
    args = parser.parse_args()

    # Apply arguments to global variables
    basedir = args.directory
    print_json = args.json
    print_logs = args.print_logs
    e4s_print_color = args.e4s_print_color
    skip_to = args.skip_to or ""
    skip_if = args.skip_if or ""
    test_only = args.test_only or ""
    slurm_flags = args.slurm_flags or ""
    slurm = args.slurm 
    processes = args.processes

    # Begin JSON output if needed
    if print_json:
        print("[",end="")

    os.environ['print_logs'] = str(print_logs).lower()
    os.environ['print_json'] = str(print_json).lower()
    os.environ['e4s_print_color'] = str(e4s_print_color).lower()
    os.environ["slurm_flags"] = str(slurm_flags)

    # Call the main directory iteration function
    iterate_directories(basedir, processes=processes)

    # End JSON output if needed
    if print_json:
        print("]",end="")

    # Exit with the final return code
    sys.exit(final_ret)

# Execute the main function if the script is run directly
if __name__ == "__main__":
    main()


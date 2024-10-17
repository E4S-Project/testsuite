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
from multiprocessing import Process
from multiprocessing.connection import wait
import time

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
def async_worker(queue, timeout=False, print_json=False, timestamp=""):
    # Change to the specified directory and update environment variables

    while (queue.qsize() > 0): # here
        worker_pipe = queue.get() #here
        current_dir_with_symlinks = worker_pipe.recv()
        worker_pipe.send(current_dir_with_symlinks)
        os.chdir(current_dir_with_symlinks)
        os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this and scripts such as setup.sh/compile.sh require this to be set
        os.environ['testdir'] = current_dir_with_symlinks
        os.environ['testtime'] = timestamp

        log_suffix = f"{os.path.basename(current_dir_with_symlinks)}_{timestamp}.log"

        # Paths to individual log files
        setup_log = f"./setup-{log_suffix}"
        clean_log = f"./clean-{log_suffix}"
        compile_log = f"./compile-{log_suffix}"
        run_log = f"./run-{log_suffix}"

        # Build a single command to run all stages sequentially, logging output to separate files
        stages = [
            f'echo "-----Setup-----"     && echo "-----Setup-----"   >&2    && ./setup.sh',
            f'echo "-----Cleaning-----"  && echo "-----Cleaning-----" >&2   && ./clean.sh   > {clean_log}' if os.path.exists('./clean.sh') else '',
            f'echo "-----Compiling-----" && echo "-----Compiling-----" >&2  && ./compile.sh > {compile_log}' if os.path.exists('./compile.sh') else '',
            f'echo "-----Running-----"   && echo "-----Running-----" >&2    && ./run.sh     > {run_log}'
        ]
        
        # Filter out any empty stages
        command = ' && '.join([stage for stage in stages if stage])
        

        
        result = subprocess.Popen(
            command, 
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        #    stdout=worker_pipe,
         #   stderr=worker_pipe,
            shell=True,
            text=True
        )
        
        stdout, stderr = result.communicate()
        worker_pipe.send(stdout)
        worker_pipe.close()
    return -1

# Function to iterate through directories in a non-recursive manner, adding calls to iterate_files.sh to a worker queue.
#Given a directory, this finds all sub directories that contain a run.sh, and then the worker processes go through each 
#sub directory calling setup.sh, compile.sh, run.sh.
def iterate_directories(testdir, processes=4, scheduler=False, scheduler_flags="", print_json=False, print_logs=False, e4s_print_color=True, skip_to="",skip_if="",test_only="",timeout=False, json_name=""):
    final_ret = 0
    results = [] #This maintains the order of prints and aynchronous job calls.
                 #It contains jobs through the results.append... lines below
                 #AND it contains strings that get printed in order. See the results for loop below for more clarification

    if not os.path.isdir(testdir):
        raise NotADirectoryError(f"{testdir} is not a directory")

    testdir = os.path.abspath(testdir)               #Note everything should be done with full paths,
                                                     #and only reduced to a basename later
    os.environ['testdir']=os.path.basename(testdir)
    
    # Use a stack to simulate recursion
    stack = [testdir]

    testsuite_dir = os.path.dirname(os.path.realpath(__file__)) #this is the testsuite directory

    manager = multiprocessing.Manager()
    queue = manager.Queue()

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
            main_pipe, worker_pipe = multiprocessing.Pipe()
            main_pipe.send(current_dir_with_symlinks)
            queue.put(worker_pipe)
            results.append(main_pipe)
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

    Processes = []
    for i in range(processes):
        Processes.append(Process(target=async_worker, args=(queue, timeout, print_json, timestamp)))
        Processes[i].start()
    

    for r in results: #If r is a string, print it, if r is not a string, it represents a 
                      #results from an asynchronous job call, and thus process its output

        if type(r) != type(""):
            while True:
                ready = wait([r], .1) #Block only temporarily 
                if ready: #Pipe is closed or a message is ready or both
                    try:
                        msg = r.recv() #This should complete if the pipe has any data
                        print(msg)
                    except EOFError: #If the pipe has no data and is closed
                        print("Pipe has been closed")
                        break
                else:
                    pass

        else:
            print(r)
    # End JSON output if needed
    print("JOininh")
    for process in Processes:
        process.join()
    print("Joined")

    if print_json:
        with open(json_output_file,"a+") as file:
            file.write("]")
            file.close()
    print("Total number of failed tests: %d" % final_ret)

    return final_ret

def source(settings_file):
    #The settings file needs to be put in the environment. Since settings.sh is sourced
    #by a bash script, then when I use the cli arg, I would have to handle that cli arg
    #in bash, which I didn't want to do, or I have to source it in python, which is 
    #kinda ridiculous but only took like 10 minutes and was kind of fun.

    #Gets the environment in the first command, then gets the env after sourcing
    #a file. Finds the set difference. Puts them in the python processes sys environment
    settings_file = os.path.realpath(settings_file)
    os.environ["TESTSUITE_SETTINGS_FILE"] = settings_file
    unsourced_command = shlex.split("bash -c 'env'")
    sourced_command = shlex.split(f"bash -c 'source {settings_file} && env'")
    unsourced_output = set(subprocess.run(unsourced_command, capture_output=True, check=True, text=True).stdout.split("\n"))
    sourced_output = set(subprocess.run(sourced_command, capture_output=True, check=True, text=True).stdout.split("\n"))

    difference_in_outputs = sourced_output.difference(unsourced_output)
    for line in difference_in_outputs:
        key, _, value = line.partition("=")
        os.environ[key]= value

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
    parser.add_argument('--timeout', type=int, default=300, help='Timeout value in seconds for each test, default is 300, 0 for no timeout')
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
    if args.test_only:
        test_only=args.test_only.split()
    else:
        test_only = ""
                
    timeout = int(args.timeout)

    source(args.settings)

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


#!/usr/bin/env python3
import os
import sys
import signal
import argparse
import subprocess
import datetime
import multiprocessing
import shlex
from multiprocessing import Process
from multiprocessing.connection import wait
import time
import json

print_color=True
def red(string):
    if print_color:
        return '\033[01m\033[31m'+string+'\033[0m'
    else:
        return string
def green(string):
    if print_color:
        return '\033[01m\033[32m'+string+'\033[0m'
    else:
        return string
def yellow(string):
    if print_color:
        return '\033[01m\033[33m'+string+'\033[0m'
    else:
        return string


#This program searches through directories. Upon finding
# a subdirectory containing run.sh, it adds it to the worker queue. Each worker
# proccess then calls setup.sh, clean.sh compile.sh and run.sh.
# Outputs are either printed to the screen in a standard manner or in a json form, --json.
# Options for a timeout time are given, with a default of 600 seconds.

# Function to execute shell scripts and handle errors, asynchronously by the worker queue
def async_worker(queue, timeout=False, print_json=False, print_logs=False, timestamp="", verbose=False):
    while (queue.qsize() > 0): 
        #Change to the specified directory and update environment variables
        worker_pipe = queue.get() #Get the pipe
        current_dir_with_symlinks = worker_pipe.recv() #Get the test name
        os.chdir(current_dir_with_symlinks) 
        os.environ['PWD']=current_dir_with_symlinks #chdir doesn't change this and scripts such as setup.sh/compile.sh require this to be set
        
        test_name = os.path.basename(current_dir_with_symlinks)
        log_suffix = f"{test_name}_{timestamp}.log"

        # Paths to individual log files
        clean_log = f"./clean-{log_suffix}"
        compile_log = f"./compile-{log_suffix}"
        run_log = f"./run-{log_suffix}"

        clean_bool = os.path.exists('./clean.sh')
        compile_bool = os.path.exists('./compile.sh')

        # Build a single command to run all stages sequentially, logging output to separate files
        stages = [
            f'echo "{test_name}" && echo "-----Setup-----"   >&2                              && ./setup.sh                 &&  echo "Setup completed"',
            f'echo "Cleaning {current_dir_with_symlinks}"  && echo "-----Cleaning-----" >&2   && ./clean.sh   > {clean_log} && echo "Clean completed"' if clean_bool else '',
            f'echo "Compiling {current_dir_with_symlinks}" && echo "-----Compiling-----" >&2  && ./compile.sh > {compile_log} && echo "Compile completed"' if compile_bool else '',
            f'echo "Running {current_dir_with_symlinks}"   && echo "-----Running-----" >&2    && ./run.sh     > {run_log} && echo "Run completed"'
        ]
         
        # Filter out any empty stages
        command =  ' && '.join([stage for stage in stages if stage])

        start_time = time.time()
        timed_out = False
        
        stderr_pipe = subprocess.PIPE if verbose else subprocess.DEVNULL
        
        result = subprocess.Popen(
            command, 
            stdout=subprocess.PIPE,
            stderr=stderr_pipe,
            shell=True,
            text=True,
            executable='/bin/bash',
            start_new_session=True
            )

        #This sets stdout to be nonblocking, so it doesn't block on rev()
        os.set_blocking(result.stdout.fileno(),False)
        completed_stages = {"setup":""}
        notFound=False
        while result.poll() == None: #While result is running
            most_recent_line = result.stdout.readline().strip()
            if "Setup completed" in most_recent_line:
                completed_stages.pop("setup") #Only include setup in json if it didn't complete
            elif "Cleaning" in most_recent_line:
                completed_stages["clean"] = ""
            elif "Clean completed" in most_recent_line:
                completed_stages["clean"] = "pass"
            elif "Compiling" in most_recent_line:
                completed_stages["compile"] = ""
            elif "Compile completed" in most_recent_line:
                completed_stages["compile"] = "pass"
            elif "Running" in most_recent_line:
                completed_stages["run"] = ""
            elif "Run completed" in most_recent_line:
                completed_stages["run"] = "pass"
            elif "completed" in most_recent_line:
                most_recent_line = ""
            if "Error: No package matches the query" in most_recent_line:
                most_recent_line=""
                notFound=True
                os.killpg(os.getpgid(result.pid), signal.SIGTERM)
                result.wait()

            if most_recent_line:
                worker_pipe.send(most_recent_line)
            if timeout and ((time.time() - start_time) > timeout): #If one of the scripts is taking too long
                timed_out = True
                os.killpg(os.getpgid(result.pid), signal.SIGTERM)
                result.wait()
        
        if verbose:
            stderr_output = result.stderr.read()
            if stderr_output:
                worker_pipe.send(red("\n--- STDERR ---"))
                worker_pipe.send(red(stderr_output))
                worker_pipe.send(red("-----------------------\n"))

        return_code = result.wait()

        final_stage = next(reversed(completed_stages.keys()))
        if return_code == None:
            print("Return code wasn't set by terminate, this shoudln't print")
        elif return_code == 215 or notFound: 
            return_code = 215
            completed_stages[final_stage]="missing"
            worker_pipe.send(final_stage.capitalize() + yellow(" Missing"))
        elif return_code != 0:
            completed_stages[final_stage]="fail"
            failure = " Timed out" if timed_out else " Failed ("+str(return_code)+")"
            worker_pipe.send(final_stage.capitalize() + red(failure))
        else:
            worker_pipe.send(green("Success"))
        worker_pipe.send([completed_stages,return_code])
        worker_pipe.close()
    return 0

def print_results(results):
    final_ret = 0
    skipped = 0
    success = 0
    json = []
    for r in results: #If r is a string, print it, if r is not a string, it is a pipe represent output from a process.
        if type(r) != type(""):
            print("===")
            test_name = r.recv()
            print(test_name)
            while True:
                ready = wait([r], .1) #Block only temporarily 
                if ready: #Pipe is closed or a message is ready or both
                    try:
                        msg = r.recv() #This should complete if the pipe has any data
                        if isinstance(msg, str):
                            print(msg)
                        elif isinstance(msg, list):
                            completed_stages, return_code = msg
                            skipped += int(return_code == 215)
                            final_ret += int(return_code != 215 and return_code != 0)
                            success += int(return_code == 0)
                            json.append({"test":test_name, "test_stages":completed_stages})
                            break
                    except EOFError: #If the pipe has no data and is closed
                        #print("Pipe has been closed")
                        break
                else:
                    pass
        else:
            print(r)
    return final_ret, skipped, success, json

#Function to iterate through directories in a non-recursive manner, adding calls to iterate_files.sh to a worker queue.
#Given a directory, this finds all sub directories that contain a run.sh, and then the worker processes go through each 
#sub directory calling setup.sh, compile.sh, run.sh.
def iterate_directories(testdir, processes=4, print_json=False, print_logs=False, skip_to="",skip_if="",test_only="", timeout=False, json_name="", verbose=False):
    final_ret = 0
    results = [] #This maintains the order of prints and the job pipes.

    if not os.path.isdir(testdir):
        raise NotADirectoryError(f"{testdir} is not a directory")

    testdir = os.path.abspath(testdir)#Directory to be tested. Note everything should be done with full paths
    testsuite_dir = os.path.dirname(os.path.realpath(__file__)) #/testsuite/

    manager = multiprocessing.Manager()
    queue = manager.Queue()

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
    # Begin JSON output if needed, make directory if needed
    json_output_file = None
    if print_json:
        os.makedirs(os.path.join(testsuite_dir,"json-outputs"), exist_ok=True)
        if json_name == "":
            json_output_file = os.path.join(testsuite_dir,'json-outputs',f"testsuite-{timestamp}.json")
        elif json_name != "":
            json_output_file = os.path.join(testsuite_dir,'json-outputs',f"{json_name}-{timestamp}.json")

    # Use a stack to search through the directories
    stack = [testdir]
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
                    stack.append(os.path.join(current_dir_with_symlinks, d))

    Processes = []
    for i in range(processes):
        Processes.append(Process(target=async_worker, args=(queue, timeout, print_json, print_logs, timestamp, verbose)))
        Processes[i].start()
    
    final_ret, skipped, success, json_results = print_results(results)

    for process in Processes:
        process.join()

    if print_json:
        with open(json_output_file,"a+") as file:
            string = json.dumps(json_results)
            file.write(string.replace('}}, {"test"','}},\n {"test"'))

    print(green("Total number of successful tests: %d" % success))
    print(yellow("Total number of skipped tests: %d" % skipped))
    print(red("Total number of failed tests: %d" % final_ret))


    return final_ret

def main():

    # Argument parsing
    parser = argparse.ArgumentParser(description='Run all tests in the specified directory.')
    parser.add_argument('directory', nargs='?', default='validation_tests', help='Test directory to use')
    parser.add_argument('--json', action='store_true', help='Print JSON output.')
    parser.add_argument('--json-name', default="", help='Optional name for json output')
    parser.add_argument('--settings', type=str, help='Path to settings file', default="settings.sh")
    parser.add_argument('--skip-to', type=str, help='Skip to specified test.')
    parser.add_argument('--skip-if', type=str, help='Skip tests with the given substring in the directory name.')
    parser.add_argument('--test-only', type=str, help='Run only specified tests.')
    parser.add_argument('--processes', type=int, default=4, help='Run tests on multiple proccesses, default is 4')
    parser.add_argument('--timeout', type=int, default=600, help='Timeout value in seconds for each test, default is 600')
    parser.add_argument('--print-logs', action='store_true', help='Print all logs.')
    parser.add_argument('--color-off', action='store_false', dest='print_color', help='Disable color output.')
    parser.add_argument('--verbose', action='store_true', help='Print stderr from tests')
    args = parser.parse_args()

    basedir = args.directory
    
    print_json = args.json
    json_name = args.json_name
    if (json_name != "") and args.json == False:
        raise ValueError("--json must be set if you wish to use --json-name")
     
    print_logs = args.print_logs
    global print_color
    print_color = args.print_color
    timeout = int(args.timeout)
    processes = args.processes
    verbose=args.verbose

    #These control which tests are run
    skip_to = args.skip_to or ""
    skip_if = args.skip_if or ""
    if args.test_only:
        test_only=args.test_only.split()
    else:
        test_only = ""

    # Call the main directory iteration function
    final_ret = iterate_directories(basedir, processes=processes, print_json=print_json, print_logs=print_logs, skip_to=skip_to, skip_if=skip_if, test_only=test_only, timeout=timeout, json_name=json_name, verbose=verbose)

    # Exit with the final return code
    sys.exit(final_ret)

# Execute the main function if the script is run directly
if __name__ == "__main__":
    main()


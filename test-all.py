#!/usr/bin/env python3
import os
import sys
import signal
import argparse
import subprocess
import datetime
import multiprocessing
from multiprocessing import Process
import time
import json
import queue
import atexit

# --- Global Variables & Failsafe ---

# This Event is a multiprocess-safe flag used to signal a graceful shutdown (e.g., on Ctrl-C).
shutdown_event = multiprocessing.Event()

# This global list holds the worker processes so they can be managed during shutdown.
Processes = []

def cleanup_terminal():
    """
    This function is registered to run unconditionally on script exit.
    It acts as a failsafe to prevent the user's terminal from being left
    in a broken state by a misbehaving test script.
    """
    print("\n--- Running final terminal cleanup ---")
    os.system('stty sane')

# Register the failsafe cleanup. It will run on normal exit, error, or most signals.
atexit.register(cleanup_terminal)


# --- Helper Functions ---

# Terminal color formatting functions
def red(string): return '\033[01m\033[31m' + string + '\033[0m' if print_color else string
def green(string): return '\033[01m\033[32m' + string + '\033[0m' if print_color else string
def yellow(string): return '\033[01m\033[33m' + string + '\033[0m' if print_color else string

def signal_handler(sig, frame):
    """The signal handler for Ctrl-C (SIGINT). Its only job is to set the shutdown event."""
    print(red('\n\nCtrl-C detected! Initiating graceful shutdown...'))
    shutdown_event.set()

def _determine_failure_stage(all_logs, timed_out, active_stages):
    """
    Analyzes log files to determine which stage of a test failed.
    A failure is attributed to the last stage for which a log file was created.
    """
    stage_to_log_map = {"setup": all_logs[0], "clean": all_logs[1], "compile": all_logs[2], "run": all_logs[3]}
    failure_reason = "timeout" if timed_out else "fail"
    
    statuses = {stage: "pass" for stage in active_stages}
    for stage in ["setup", "clean", "compile", "run"]:
        if stage not in active_stages:
            statuses[stage] = "skipped"

    failing_stage = "unknown"
    
    # Work backwards through the stages that were actually supposed to run.
    for stage in reversed(active_stages):
        log_file = stage_to_log_map[stage]
        if os.path.exists(log_file):
            failing_stage = stage
            statuses[stage] = failure_reason
            
            # Mark all subsequent ACTIVE stages as skipped
            failure_index = active_stages.index(failing_stage)
            for i in range(failure_index + 1, len(active_stages)):
                statuses[active_stages[i]] = "skipped"
            break
            
    if failing_stage == "unknown" and active_stages:
        failing_stage = active_stages[0]
        statuses[failing_stage] = failure_reason
        # Mark all subsequent ACTIVE stages as skipped
        for i in range(1, len(active_stages)):
            statuses[active_stages[i]] = "skipped"

    return statuses, failing_stage


# --- Producer/Consumer Functions ---

def async_worker(test_queue, results_queue, timeout, print_json, print_logs, timestamp, verbose):
    """
    The "Producer" function, run by each parallel worker process.
    It pulls a test path from the test_queue, executes the test, and puts
    a comprehensive result tuple onto the results_queue.
    """
    while not test_queue.empty():
        try:
            current_dir = test_queue.get_nowait()
        except queue.Empty:
            break
        
        os.chdir(current_dir)
        os.environ['PWD'] = current_dir
        test_name = os.path.basename(current_dir)
        log_suffix = f"{test_name}_{timestamp}.log"
        setup_log, clean_log, compile_log, run_log = f"./setup-{log_suffix}", f"./clean-{log_suffix}", f"./compile-{log_suffix}", f"./run-{log_suffix}"
        all_logs = [setup_log, clean_log, compile_log, run_log]

        clean_bool = os.path.exists('./clean.sh')
        compile_bool = os.path.exists('./compile.sh')

        active_stage_names = ['setup']
        if clean_bool: active_stage_names.append('clean')
        if compile_bool: active_stage_names.append('compile')
        active_stage_names.append('run')
        
        # This shell command  sources setup.sh to inherit its
        # environment, then checks the SPACK_LOAD_RESULT variable. If the spack load
        # failed, it exits immediately with the failure code. Otherwise, it proceeds.
        setup_stage = f"export SPACK_LOAD_RESULT=0; source ./setup.sh > {setup_log} 2>&1; exit_code=$SPACK_LOAD_RESULT; if [ $exit_code -ne 0 ]; then exit $exit_code; fi"
        clean_stage = f"./clean.sh > {clean_log} 2>&1" if clean_bool else "true"
        compile_stage = f"./compile.sh > {compile_log} 2>&1" if compile_bool else "true"
        run_stage = f"./run.sh > {run_log} 2>&1"
        
        full_command_chain = f"{setup_stage} && {clean_stage} && {compile_stage} && {run_stage}"
        command = f"bash -c '{full_command_chain}'"

        timed_out, return_code = False, -1
        try:
            result = subprocess.Popen(command, shell=True, executable='/bin/bash', text=True, start_new_session=True)
            result.communicate(timeout=timeout)
            return_code = result.returncode
        except subprocess.TimeoutExpired:
            timed_out, return_code = True, 124
            os.killpg(os.getpgid(result.pid), signal.SIGTERM)
            result.wait()
        except Exception:
            return_code = 1

        if return_code == 0:
            completed_stages = {stage: "pass" for stage in active_stage_names}
            results_queue.put(("success", test_name, completed_stages, return_code, []))
        else:
            log_contents = []
            if return_code == 215:
                completed_stages = {stage: "skipped" for stage in active_stage_names}
                completed_stages["setup"] = "missing"
                results_queue.put(("missing", test_name, completed_stages, return_code, []))
            else:
                if verbose:
                    for log_file in all_logs:
                        if os.path.exists(log_file):
                            try:
                                with open(log_file, 'r', errors='ignore') as f:
                                    log_contents.append((os.path.basename(log_file), f.read()))
                            except Exception: pass
                completed_stages, failing_stage = _determine_failure_stage(all_logs, timed_out, active_stage_names)
                results_queue.put(("failure", test_name, completed_stages, return_code, log_contents, failing_stage, timed_out))

def print_results(results_queue, num_tests):
    """
    The "Consumer" function, run by the main process. It pulls results
    from the queue and prints them serially. This prevents race conditions
    related to printing and terminal state.
    """
    final_ret, skipped, success = 0, 0, 0
    json_results = []
    processed_tests = 0

    while processed_tests < num_tests:
        if shutdown_event.is_set():
            print(yellow("\nShutdown detected. Aborting result processing."))
            break
        try:
            # Use a timeout on get() to remain responsive to the shutdown_event.
            result_tuple = results_queue.get(timeout=1.0)
            processed_tests += 1
            
            # Proactively reset terminal state before printing. This fixes visual corruption
            # caused by a race condition where a finishing test breaks the terminal
            # before the next result is printed.
            os.system('stty sane')

            message_type, test_name, completed_stages, return_code, *extra_payload = result_tuple

            print("===")
            print(test_name)

            if message_type == "success":
                print(green("Success"))
                success += 1
            elif message_type == "missing":
                print(yellow("Setup Missing"))
                skipped += 1
            elif message_type == "failure":
                log_contents, failing_stage, timed_out = extra_payload[0], extra_payload[1], extra_payload[2]
                reason = " Timed out" if timed_out else f" Failed ({return_code})"
                print(red(failing_stage.capitalize() + reason))
                final_ret += 1
                if log_contents:
                    print(yellow("\n--- Log Files on Failure ---"))
                    for log_name, content in log_contents:
                        print(yellow(f"--- Contents of {log_name} ---"))
                        for line in content.splitlines():
                            print(line.strip())

            json_results.append({"test": test_name, "test_stages": completed_stages})
        except queue.Empty:
            # This is the normal case when no result is ready. We loop to check the shutdown flag.
            continue
            
    if shutdown_event.is_set():
        print(red("Terminating any remaining active worker processes..."))
        for p in Processes:
            if p.is_alive():
                p.terminate()

    return final_ret, skipped, success, json_results


# --- Main Orchestration ---

def find_tests_to_run(testdir, skip_to, skip_if, test_only):
    """Walks the directory tree to find all valid tests based on filters."""
    tests_to_run = []
    stack = [os.path.abspath(testdir)]
    while stack:
        current_dir = stack.pop(0)
        if os.path.exists(os.path.join(current_dir, "run.sh")):
            base_name = os.path.basename(current_dir)
            if (not skip_to or base_name >= skip_to) and \
               (not skip_if or skip_if not in base_name) and \
               (not test_only or base_name in test_only):
                tests_to_run.append(current_dir)
        else:
            try:
                for d in sorted(os.listdir(current_dir)):
                    full_path = os.path.join(current_dir, d)
                    if os.path.isdir(full_path):
                        stack.append(full_path)
            except FileNotFoundError: pass
    return tests_to_run

def main():
    signal.signal(signal.SIGINT, signal_handler)
    
    parser = argparse.ArgumentParser(description='Run all tests in the specified directory.')
    parser.add_argument('directory', nargs='?', default='validation_tests', help='Test directory to use')
    parser.add_argument('--json', action='store_true', help='Print JSON output.')
    parser.add_argument('--json-name', default="", help='Optional name for json output')
    parser.add_argument('--skip-to', type=str, help='Skip to specified test.')
    parser.add_argument('--skip-if', type=str, help='Skip tests with the given substring.')
    parser.add_argument('--test-only', type=str, help='Run only specified tests, space-separated.')
    parser.add_argument('--processes', type=int, default=4, help='Number of parallel processes.')
    parser.add_argument('--timeout', type=int, default=600, help='Timeout in seconds for each test.')
    parser.add_argument('--print-logs', action='store_true', help='(Deprecated) Print all logs.')
    parser.add_argument('--color-off', action='store_false', dest='print_color', help='Disable color output.')
    parser.add_argument('--verbose', action='store_true', help='On failure, print test logs.')
    args = parser.parse_args()

    global print_color
    print_color = args.print_color
    test_only = args.test_only.split() if args.test_only else []
    
    # Create the Manager and Queues in the main scope to ensure they live for the entire
    # duration of the script, preventing crashes from premature garbage collection.
    manager = multiprocessing.Manager()
    results_queue, test_queue = manager.Queue(), manager.Queue()

    # Find all tests first, then populate the queue.
    tests_to_run = find_tests_to_run(args.directory, args.skip_to, args.skip_if, test_only)
    for test_path in tests_to_run: test_queue.put(test_path)
    num_tests = len(tests_to_run)
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")

    # Start the worker processes.
    global Processes
    Processes = []
    for i in range(args.processes):
        p = Process(target=async_worker, args=(test_queue, results_queue, args.timeout, args.json, args.print_logs, timestamp, args.verbose))
        p.start()
        Processes.append(p)

    # Call the consumer to process results and get the final totals.
    final_ret, skipped, success, json_results = print_results(results_queue, num_tests)

    # After all results are processed, wait for worker processes to exit cleanly.
    for p in Processes:
        if p.is_alive(): p.join(timeout=5.0)
        if p.is_alive(): p.kill()

    # Handle JSON output at the very end.
    if args.json:
        testsuite_dir = os.path.dirname(os.path.realpath(__file__))
        json_output_dir = os.path.join(testsuite_dir, "json-outputs")
        os.makedirs(json_output_dir, exist_ok=True)
        filename = f"testsuite-{timestamp}.json" if not args.json_name else f"{args.json_name}-{timestamp}.json"
        json_output_file = os.path.join(json_output_dir, filename)

        # Sort results alphabetically by test name for consistent, comparable output.
        json_results.sort(key=lambda item: item.get("test"))

        with open(json_output_file, "w") as file:
            file.write('[\n')
            file.write(',\n'.join(json.dumps(item) for item in json_results))
            file.write('\n]\n')

    # Print the final summary.
    print(green("\nTotal number of successful tests: %d" % success))
    print(yellow("Total number of skipped tests: %d" % skipped))
    print(red("Total number of failed tests: %d" % final_ret))
    sys.exit(final_ret)

if __name__ == "__main__":
    main()

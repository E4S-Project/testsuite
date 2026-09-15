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

def safe_stty_sane():
    """Resets the terminal state, but only if running in an interactive terminal."""
    if sys.stdin and sys.stdin.isatty():
        os.system('stty sane')

def cleanup_terminal():
    """
    This function is registered to run unconditionally on script exit.
    It acts as a failsafe to prevent the user's terminal from being left
    in a broken state by a misbehaving test script.
    """
    print("\n--- Running final terminal cleanup ---")
    safe_stty_sane()

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

def run_stage_with_metrics(cmd, log_path, cwd, env=None, timeout=None):
    """
    Executes a test stage command, capturing stdout/stderr into log_path,
    and measuring wall time, user/system CPU time, peak memory (RSS),
    CPU percent utilization, and voluntary/involuntary context switches
    via standard POSIX/Linux os.wait4.
    Appends a standardized '--- Resource Utilization ---' summary block to the log.
    """
    start_wall = time.perf_counter()
    timed_out = False
    return_code = -1
    rusage = None

    with open(log_path, "w") as log_file:
        proc = subprocess.Popen(
            cmd,
            shell=True,
            executable='/bin/bash',
            cwd=cwd,
            env=env,
            stdout=log_file,
            stderr=subprocess.STDOUT,
            start_new_session=True
        )

        start_wait = time.time()
        while True:
            pid, status, ru = os.wait4(proc.pid, os.WNOHANG)
            if pid != 0:
                rusage = ru
                if os.WIFEXITED(status):
                    return_code = os.WEXITSTATUS(status)
                elif os.WIFSIGNALED(status):
                    return_code = -os.WTERMSIG(status)
                else:
                    return_code = 1
                break

            if timeout is not None and (time.time() - start_wait) > timeout:
                timed_out = True
                try:
                    os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
                    time.sleep(0.2)
                    pid_check, _, _ = os.wait4(proc.pid, os.WNOHANG)
                    if pid_check == 0:
                        os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
                    _, status, ru = os.wait4(proc.pid, 0)
                    rusage = ru
                except Exception:
                    pass
                return_code = 124
                break

            time.sleep(0.05)

    end_wall = time.perf_counter()
    wall_sec = end_wall - start_wall
    user_sec = rusage.ru_utime if rusage else 0.0
    sys_sec = rusage.ru_stime if rusage else 0.0
    cpu_sec = user_sec + sys_sec
    cpu_pct = (cpu_sec / wall_sec * 100.0) if wall_sec > 0 else 0.0
    max_rss = rusage.ru_maxrss if rusage else 0
    vol_cs = rusage.ru_nvcsw if rusage else 0
    invol_cs = rusage.ru_nivcsw if rusage else 0

    metrics = {
        "elapsed_sec": round(wall_sec, 2),
        "user_cpu_sec": round(user_sec, 2),
        "sys_cpu_sec": round(sys_sec, 2),
        "cpu_percent": round(cpu_pct, 1),
        "max_rss_kb": max_rss,
        "vol_ctx_switches": vol_cs,
        "invol_ctx_switches": invol_cs
    }

    rss_mb = max_rss / 1024.0
    m, s = divmod(wall_sec, 60)
    h, m = divmod(m, 60)
    time_str = f"{int(h):02d}:{int(m):02d}:{s:05.2f}" if h > 0 else f"{int(m):02d}:{s:05.2f}"

    summary_block = (
        "\n--- Resource Utilization ---\n"
        f"Elapsed Time (wall clock): {wall_sec:.2f}s ({time_str})\n"
        f"CPU Utilization: {cpu_pct:.1f}%\n"
        f"User CPU Time: {user_sec:.2f}s\n"
        f"System CPU Time: {sys_sec:.2f}s\n"
        f"Max Memory (RSS): {max_rss} KB ({rss_mb:.2f} MB)\n"
        f"Context Switches (vol/invol): {vol_cs} / {invol_cs}\n"
    )

    try:
        with open(log_path, "a") as log_file:
            log_file.write(summary_block)
    except Exception:
        pass

    return return_code, timed_out, metrics

def _clean_file(path):
    if os.path.exists(path):
        try:
            os.remove(path)
        except Exception:
            pass

def _get_log_contents(all_logs, verbose):
    log_contents = []
    if verbose:
        for log_file in all_logs:
            if os.path.exists(log_file):
                try:
                    with open(log_file, 'r', errors='ignore') as f:
                        log_contents.append((os.path.basename(log_file), f.read()))
                except Exception:
                    pass
    return log_contents

def verify_mpi_environment():
    """Triggers setup.sh in a dedicated validation mode to check MPI globally."""
    driver_dir = os.path.dirname(os.path.abspath(__file__))
    setup_script = os.path.join(driver_dir, "setup.sh")
    
    # Run setup.sh with the special intercept variable active
    cmd = f"E4S_MPI_SANITY_TEST=1 bash {setup_script}"
    
    result = subprocess.run(cmd, shell=True, cwd=driver_dir)
    return result.returncode == 0



# --- Producer/Consumer Functions ---

def async_worker(test_queue, results_queue, timeout, print_json, print_logs, timestamp, verbose, setup_mode, skip_internal):
    """
    The "Producer" function, run by each parallel worker process.
    It pulls a test path from the test_queue, executes the test stages sequentially,
    measures execution metrics (time, CPU, memory, context switches) for each stage,
    and puts a comprehensive result tuple onto the results_queue.
    """
    while not test_queue.empty():
        try:
            current_dir = test_queue.get_nowait()
        except queue.Empty:
            break

        os.environ['PWD'] = current_dir
        test_name = os.path.basename(current_dir)
        log_suffix = f"{test_name}_{timestamp}.log"
        setup_log = os.path.join(current_dir, f"setup-{log_suffix}")
        clean_log = os.path.join(current_dir, f"clean-{log_suffix}")
        compile_log = os.path.join(current_dir, f"compile-{log_suffix}")
        run_log = os.path.join(current_dir, f"run-{log_suffix}")
        all_logs = [setup_log, clean_log, compile_log, run_log]

        clean_bool = os.path.exists(os.path.join(current_dir, 'clean.sh'))
        compile_bool = os.path.exists(os.path.join(current_dir, 'compile.sh'))

        active_stage_names = ['setup']
        if clean_bool: active_stage_names.append('clean')
        if compile_bool: active_stage_names.append('compile')
        active_stage_names.append('run')

        completed_stages = {stage: "skipped" for stage in active_stage_names}
        metrics = {}
        env_json = os.path.join(current_dir, f".env_{test_name}_{timestamp}_{os.getpid()}.json")
        test_env = os.environ.copy()

        start_test_time = time.time()

        # 1. SETUP STAGE
        setup_cmd = (
            f"export E4S_TEST_SKIP_INTERNAL={skip_internal}; "
            f"export E4S_TEST_SETUP_MODE={setup_mode}; "
            f"export SPACK_LOAD_RESULT=0; "
            f"source ./setup.sh; "
            f"exit_code=$SPACK_LOAD_RESULT; "
            f"if [ $exit_code -ne 0 ]; then exit $exit_code; fi; "
            f"python3 -c 'import os, json, sys; json.dump(dict(os.environ), open(sys.argv[1], \"w\"))' \"{env_json}\""
        )

        remaining_timeout = max(1.0, timeout - (time.time() - start_test_time))
        rc, timed_out, stage_metrics = run_stage_with_metrics(setup_cmd, setup_log, current_dir, env=test_env, timeout=remaining_timeout)
        metrics["setup"] = stage_metrics

        if timed_out:
            completed_stages["setup"] = "timeout"
            log_contents = _get_log_contents(all_logs, verbose)
            results_queue.put(("failure", test_name, completed_stages, 124, log_contents, "setup", True, metrics))
            _clean_file(env_json)
            continue

        if rc == 215:
            completed_stages["setup"] = "missing"
            results_queue.put(("missing", test_name, completed_stages, rc, [], metrics))
            _clean_file(env_json)
            continue
        elif rc == 216:
            completed_stages["setup"] = "spacktest"
            results_queue.put(("spacktest", test_name, completed_stages, rc, [], metrics))
            _clean_file(env_json)
            continue
        elif rc != 0:
            completed_stages["setup"] = "fail"
            log_contents = _get_log_contents(all_logs, verbose)
            results_queue.put(("failure", test_name, completed_stages, rc, log_contents, "setup", False, metrics))
            _clean_file(env_json)
            continue

        completed_stages["setup"] = "pass"

        # Load environment exported by setup.sh
        if os.path.exists(env_json):
            try:
                with open(env_json, 'r') as f:
                    test_env = json.load(f)
            except Exception:
                pass
            _clean_file(env_json)

        # 2. CLEAN STAGE
        if clean_bool:
            remaining_timeout = max(1.0, timeout - (time.time() - start_test_time))
            rc, timed_out, stage_metrics = run_stage_with_metrics("./clean.sh", clean_log, current_dir, env=test_env, timeout=remaining_timeout)
            metrics["clean"] = stage_metrics
            if timed_out or rc != 0:
                completed_stages["clean"] = "timeout" if timed_out else "fail"
                log_contents = _get_log_contents(all_logs, verbose)
                results_queue.put(("failure", test_name, completed_stages, rc, log_contents, "clean", timed_out, metrics))
                continue
            completed_stages["clean"] = "pass"

        # 3. COMPILE STAGE
        if compile_bool:
            remaining_timeout = max(1.0, timeout - (time.time() - start_test_time))
            rc, timed_out, stage_metrics = run_stage_with_metrics("./compile.sh", compile_log, current_dir, env=test_env, timeout=remaining_timeout)
            metrics["compile"] = stage_metrics
            if timed_out or rc != 0:
                completed_stages["compile"] = "timeout" if timed_out else "fail"
                log_contents = _get_log_contents(all_logs, verbose)
                results_queue.put(("failure", test_name, completed_stages, rc, log_contents, "compile", timed_out, metrics))
                continue
            completed_stages["compile"] = "pass"

        # Sync before run stage
        try:
            os.sync()
        except AttributeError:
            pass
        time.sleep(0.5)

        # 4. RUN STAGE
        remaining_timeout = max(1.0, timeout - (time.time() - start_test_time))
        rc, timed_out, stage_metrics = run_stage_with_metrics("./run.sh", run_log, current_dir, env=test_env, timeout=remaining_timeout)
        metrics["run"] = stage_metrics
        if timed_out or rc != 0:
            completed_stages["run"] = "timeout" if timed_out else "fail"
            log_contents = _get_log_contents(all_logs, verbose)
            results_queue.put(("failure", test_name, completed_stages, rc, log_contents, "run", timed_out, metrics))
            continue

        completed_stages["run"] = "pass"
        results_queue.put(("success", test_name, completed_stages, 0, [], metrics))

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
            result_tuple = results_queue.get(timeout=1.0)
            processed_tests += 1

            safe_stty_sane()

            message_type = result_tuple[0]
            test_name = result_tuple[1]
            completed_stages = result_tuple[2]
            return_code = result_tuple[3]
            extra_payload = result_tuple[4:]

            print("===")
            print(test_name)

            metrics = {}
            if message_type == "success":
                print(green("Success"))
                success += 1
                if len(extra_payload) >= 2 and isinstance(extra_payload[1], dict):
                    metrics = extra_payload[1]
            elif message_type == "missing":
                print(yellow("Setup Missing"))
                skipped += 1
                if len(extra_payload) >= 2 and isinstance(extra_payload[1], dict):
                    metrics = extra_payload[1]
            elif message_type == "spacktest":
                print(yellow("Spack tests skipped"))
                skipped += 1
                if len(extra_payload) >= 2 and isinstance(extra_payload[1], dict):
                    metrics = extra_payload[1]
            elif message_type == "failure":
                log_contents = extra_payload[0] if len(extra_payload) > 0 else []
                failing_stage = extra_payload[1] if len(extra_payload) > 1 else "unknown"
                timed_out = extra_payload[2] if len(extra_payload) > 2 else False
                if len(extra_payload) > 3 and isinstance(extra_payload[3], dict):
                    metrics = extra_payload[3]

                reason = " Timed out" if timed_out else f" Failed ({return_code})"
                print(red(failing_stage.capitalize() + reason))
                final_ret += 1
                if log_contents:
                    print(yellow("\n--- Log Files on Failure ---"))
                    for log_name, content in log_contents:
                        print(yellow(f"--- Contents of {log_name} ---"))
                        for line in content.splitlines():
                            print(line.strip())

            entry = {"test": test_name, "test_stages": completed_stages}
            if metrics:
                entry["metrics"] = metrics
            json_results.append(entry)
        except queue.Empty:
            continue

    if shutdown_event.is_set():
        print(red("Terminating any remaining active worker processes..."))
        for p in Processes:
            if p.is_alive():
                p.terminate()

    return final_ret, skipped, success, json_results
            
    if shutdown_event.is_set():
        print(red("Terminating any remaining active worker processes..."))
        for p in Processes:
            if p.is_alive():
                p.terminate()

    return final_ret, skipped, success, json_results


# --- Main Orchestration ---

def find_tests_to_run(testdir, skip_to, skip_if, test_only, skip_tests):
    """Walks the directory tree to find all valid tests based on filters."""
    tests_to_run = []
    stack = [os.path.abspath(testdir)]
    while stack:
        current_dir = stack.pop(0)
        if os.path.exists(os.path.join(current_dir, "run.sh")):
            base_name = os.path.basename(current_dir)
            if (not skip_to or base_name >= skip_to) and \
               (not skip_if or skip_if not in base_name) and \
               (not test_only or base_name in test_only) and \
               (not skip_tests or base_name not in skip_tests):
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
    parser.add_argument('--skip-tests', type=str, help='Skip specific tests by exact name, quoted, space-separated.')
    parser.add_argument('--test-only', type=str, help='Run only specified tests, quoted, space-separated.')
    parser.add_argument('--processes', type=int, default=4, help='Number of parallel processes.')
    parser.add_argument('--timeout', type=int, default=600, help='Timeout in seconds for each test.')
    parser.add_argument('--print-logs', action='store_true', help='(Deprecated) Print all logs.')
    parser.add_argument('--color-off', action='store_false', dest='print_color', help='Disable color output.')
    parser.add_argument('--verbose', action='store_true', help='On failure, print test logs.')
    parser.add_argument('--setup-mode', choices=['spack', 'module'], default='spack', 
                    help='Choose how to load packages in the test environment (default: spack)')
    parser.add_argument('--skip-internal', action='store_true', help='Skip spack-provided tests.')                
    args = parser.parse_args()

    if verify_mpi_environment():
        print("MPI environment verified. Proceeding to parallel tests.")
        # Set the bypass variable. Parallel workers inherit this.
        os.environ['E4S_MPI_SANITY_OK'] = '1'
    else:
        print("\nAborting test suite run. MPI sanity check failed.")
        sys.exit(1)

    global print_color
    print_color = args.print_color
    test_only = set(args.test_only.split()) if args.test_only else set()
    skip_tests = set(args.skip_tests.split()) if args.skip_tests else set()
    
    # Create the Manager and Queues in the main scope to ensure they live for the entire
    # duration of the script, preventing crashes from premature garbage collection.
    manager = multiprocessing.Manager()
    results_queue, test_queue = manager.Queue(), manager.Queue()

    # Find all tests first, then populate the queue.
    tests_to_run = find_tests_to_run(args.directory, args.skip_to, args.skip_if, test_only, skip_tests)
    for test_path in tests_to_run: test_queue.put(test_path)
    num_tests = len(tests_to_run)
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")

    # Start the worker processes.
    global Processes
    Processes = []
    for i in range(args.processes):
        p = Process(target=async_worker, args=(test_queue, results_queue, args.timeout, args.json, args.print_logs, timestamp, args.verbose, args.setup_mode, args.skip_internal))
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

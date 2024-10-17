import os
import subprocess
import shlex
import asyncio
def run_all_stages(current_dir_with_symlinks, print_json=False, print_logs=False, timestamp=""):
    """Run all stages in one subprocess.run call with separate logging for each stage."""
    
    os.chdir(current_dir_with_symlinks)
    os.environ['PWD'] = current_dir_with_symlinks
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

    # Run the entire sequence of commands in a single subprocess
    result = subprocess.run(
        command, 
        shell=True,  # Important for running complex command strings
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )



    # Capture output
    stdout = result.stdout
    stderr = result.stderr
    return_code = result.returncode
    
    print("Stdout: ")

    print("Stderr: ",stderr) 

    if return_code: #Code has failed, find last stage that failed.
        if "-----Running-----" in stdout:
            print("Test failed at running stage")
            print("Run failed")
            if return_code == 215:
                print("Required Spack Packages Not Found")
        elif "-----Compiling-----" in stdout:
            print("Test failed at compiling stage")
            print("Compilation failed")
            if return_code == 215:
                print("Required Spack Packages Not Found")
        elif "-----Cleaning-----" in stdout:
            print("Test failed at cleaning stage")
            print("Clean failed")
            if return_code == 215:
                print("Required Spack Packages Not Found")
        elif "-----Setup-----" in stdout:
            print("Test failed at setup stage")
            print("Setup failed")
            if return_code == 215:
                print("Required Spack Packages Not Found")
    
    # Optionally print each log if requested
    if print_logs:
        if os.path.exists(setup_log):
            print("--- SETUP LOG ---")
            with open(setup_log, "r") as f:
                print(f.read())
        
        if os.path.exists(clean_log):
            print("--- CLEAN LOG ---")
            with open(clean_log, "r") as f:
                print(f.read())
        
        if os.path.exists(compile_log):
            print("--- COMPILE LOG ---")
            with open(compile_log, "r") as f:
                print(f.read())
        
        if os.path.exists(run_log):
            print("--- RUN LOG ---")
            with open(run_log, "r") as f:
                print(f.read())

    # Handle JSON output and error reporting
    if print_json:
        print("{")
        json_status = []
        json_status.append(f'"setup": "pass"' if os.path.exists(setup_log) and return_code == 0 else '"setup": "fail"')
        if os.path.exists('./clean.sh'):
            json_status.append(f'"clean": "pass"' if return_code == 0 else '"clean": "fail"')
        if os.path.exists('./compile.sh'):
            json_status.append(f'"compile": "pass"' if return_code == 0 else '"compile": "fail"')
        json_status.append(f'"run": "pass"' if return_code == 0 else '"run": "fail"')
        print(",".join(json_status))
        print("}")

    # Return the final return code to indicate success or failure
    result.wait()
    return return_code, result

# Example usage
return_code, result = run_all_stages(
    current_dir_with_symlinks="validation_tests/alquimia",
    print_json=False,
    print_logs=False,
    timestamp="2024-10-12"
)


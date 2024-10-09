import subprocess
import os
import filecmp
import difflib
import argparse
import glob

#Quick little testing script so I can ensure the output is as I expected

# Define base test cases with their corresponding output files
base_tests = {
    "0000": "python test-all.py valTest/",
    "0010": "python test-all.py valTest/ --json",
    "0001": "python test-all.py valTest/ --slurm",
    "0011": "python test-all.py valTest/ --slurm --json",
    "0100": "python test-all.py timeTest/ --timeout=10",
    "0110": "python test-all.py timeTest/ --timeout=10 --json",
    "0101": "python test-all.py timeTest/ --timeout=10 --slurm",
    "0111": "python test-all.py timeTest/ --timeout=10 --slurm --json",
}

# Define additional test cases for --print-logs
def extend_tests():
    extended_tests = {}
    for code, base_command in base_tests.items():
        # Add the --print-logs version of each test case
        log_test_code = "1" + code[1:]
        log_command = base_command.replace("test-all.py", "test-all.py --print-logs")
        extended_tests[log_test_code] = log_command

    return extended_tests

# Directories
correct_dir = "correct-outputs"
output_dir = "current-outputs"
json_dir = "json-outputs"

def create_dirs():
    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(correct_dir, exist_ok=True)
    os.makedirs(json_dir, exist_ok=True)

def run_test(test_code, command, is_first_run=False):
    output_file = os.path.join(correct_dir if is_first_run else output_dir, test_code)
    json_file = os.path.join(json_dir, f"{test_code}-current.json")

    # Prepare command if it includes --json
    if "--json" in command:
        command += f" --json-name={test_code}-current"
        clean_json_files(test_code)  # Clean previous json output for this test_code

    # Run the command and save stdout to file
    print(f"Running test {test_code}: {command}")
    with open(output_file, "w") as f:
        subprocess.run(command.split(), stdout=f)

    # For --json, move generated json file to our json_dir
    if "--json" in command:
        move_json_file(test_code)

def clean_json_files(test_code):
    # Remove any existing JSON file for the current test run
    json_files = glob.glob(os.path.join(json_dir, f"{test_code}-current*.json"))
    for file in json_files:
        os.remove(file)

def move_json_file(test_code):
    # Find the json file and move it to the json_dir with the appropriate test_code name
    json_files = glob.glob(f"json-outputs/{test_code}-current*.json")
    for json_file in json_files:
        new_name = os.path.join(json_dir, f"{test_code}-current.json")
        os.rename(json_file, new_name)

def show_diff(file1, file2):
    # Show diff between two files in git-style
    with open(file1) as f1, open(file2) as f2:
        diff = difflib.unified_diff(f1.readlines(), f2.readlines(), fromfile=file1, tofile=file2)
        for line in diff:
            print(line, end='')

def compare_outputs(test_code):
    output_file = os.path.join(output_dir, test_code)
    correct_file = os.path.join(correct_dir, test_code)

    if not os.path.exists(correct_file):
        print(f"Warning: No correct output file for test {test_code}")
        return False

    if not filecmp.cmp(output_file, correct_file):
        print(f"Test {test_code} FAILED - Output does not match.")
        show_diff(correct_file, output_file)
        return False

    # If JSON is used, compare JSON files
    json_file = os.path.join(json_dir, f"{test_code}-current.json")
    correct_json_file = os.path.join(json_dir, f"{test_code}.json")
    if os.path.exists(json_file) and os.path.exists(correct_json_file):
        if not filecmp.cmp(json_file, correct_json_file):
            print(f"Test {test_code} FAILED - JSON Output does not match.")
            show_diff(correct_json_file, json_file)
            return False

    print(f"Test {test_code} PASSED")
    return True

def run_tests(first_run=False, with_print_logs=False):
    # Merge base and extended tests if --print-logs is provided
    tests = base_tests.copy()
    if with_print_logs:
        tests.update(extend_tests())

    for test_code, command in tests.items():
        run_test(test_code, command, is_first_run=first_run)
        if not first_run:
            compare_outputs(test_code)  # Compare the outputs after each test


def main():
    create_dirs()

    parser = argparse.ArgumentParser()
    parser.add_argument("--print-logs", action="store_true", help="Include print-logs tests")
    args = parser.parse_args()

    # Check if correct-outputs exists, if not, generate correct outputs
    if not os.listdir(correct_dir):
        print("Generating correct outputs...")
        run_tests(first_run=True)
    if (not ("1000" in os.listdir(correct_dir)) and args.print_logs):
        print("generating correct outputs with print_logs")
        for test_code, command in extend_tests().items():
            run_test(test_code, command, is_first_run=True)

    # Run the tests and compare outputs
    print("Running tests...")
    run_tests(with_print_logs=args.print_logs)

if __name__ == "__main__":
    main()


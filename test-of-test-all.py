import subprocess
import filecmp
import os

# This and test-of-all-test.py are used to test the output of test-all.py

# Define test cases with their corresponding output files
tests = {
    "000": "python test-all.py valTest/",
    "010": "python test-all.py valTest/ --json",
    "001": "python test-all.py valTest/ --slurm",
    "011": "python test-all.py valTest/ --slurm --json",
    "100": "python test-all.py timeTest/ --timeout=10",
    "110": "python test-all.py timeTest/ --timeout=10 --json",
    "101": "python test-all.py timeTest/ --timeout=10 --slurm",
    "111": "python test-all.py timeTest/ --timeout=10 --slurm --json"
}

# Directory where the correct outputs are stored
correct_dir = "correct_outputs"
# Directory where the current test outputs are stored
output_dir = "current_outputs"

os.makedirs(output_dir, exist_ok=True)
os.makedirs(correct_dir, exist_ok=True)

def run_tests():
    for test_code, command in tests.items():
        output_file = os.path.join(output_dir, test_code)
        # Run the command and save the output to a file
        with open(output_file, "w") as f:
            subprocess.run(command.split(), stdout=f)
        # Compare the output with the correct file
        correct_file = os.path.join(correct_dir, test_code)
        if not os.path.exists(correct_file):
            print(f"Warning: No correct output file for test {test_code}")
        elif not filecmp.cmp(output_file, correct_file):
            print(f"Test {test_code} FAILED - Output does not match.")
        else:
            print(f"Test {test_code} PASSED")

if __name__ == "__main__":
    run_tests()


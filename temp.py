import subprocess
import os

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

# Directory where the outputs will be stored
output_dir = "correct_outputs"

def run_tests_first_time():
    # Ensure the output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    for test_code, command in tests.items():
        output_file = os.path.join(output_dir, test_code)
        # Run the command and save the output to a file
        print(f"Running test {test_code}: {command}")
        with open(output_file, "w") as f:
            subprocess.run(command.split(), stdout=f)

if __name__ == "__main__":
    run_tests_first_time()


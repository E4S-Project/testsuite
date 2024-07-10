#!/usr/bin/env python3

import json
import os
import sys

def print_help():
    print("Usage: merge_json.py [OPTIONS] file1.json file2.json ...")
    print("Concatenate several .json files into a single file.")
    print("Each file contains a partial list that needs to be combined into a single list.")
    print("\nOptions:")
    print("  --show-skipped   Add missing test directory entries.")
    print("  --help           Show this help message and exit.")

def fix_incomplete_json(json_string):
    json_string = json_string.rstrip()
    if json_string.endswith(","):
        json_string = json_string[:-1] + "]"
    elif not json_string.endswith("]"):
        json_string += "]"
    return json_string

def main():
    # If no arguments or --help is provided, print help message and exit
    if len(sys.argv) < 2 or "--help" in sys.argv:
        print_help()
        sys.exit()

    # Get the list of JSON files and options from command line arguments
    json_files = [arg for arg in sys.argv[1:] if not arg.startswith("--")]
    options = [arg for arg in sys.argv[1:] if arg.startswith("--")]

    # If no valid JSON files are provided, print help message and exit
    if not json_files or not all(file.endswith(".json") for file in json_files):
        print("Error: No valid JSON files provided.")
        print_help()
        sys.exit()

    combined_data = []
    for filename in json_files:
        try:
            with open(filename, 'r') as f:
                content = f.read()
                fixed_content = fix_incomplete_json(content)
                data = json.loads(fixed_content)
                combined_data.extend(data)
        except json.JSONDecodeError:
            print(f"Couldn't process file: {filename}")
            last_char = content.rstrip()[-1]
            print(f"Last non-whitespace character in the file: '{last_char}'")

    # If --show-skipped is provided, check and add the missing directory name entries
    if "--show-skipped" in options:
        # Get a list of all subdirectories in validation_tests
        subdirs = [name.rstrip('/') for name in os.listdir("validation_tests") if os.path.isdir(os.path.join("validation_tests", name))]

        # Create a set of all unique 'test' values in the existing data
        test_values = {d.get("test") for d in combined_data}

        # For each subdirectory, if there isn't already a dictionary with "test": subdirectory, add one
        for subdir in subdirs:
            if subdir not in test_values:
                combined_data.append({"test": subdir, "test_stages": {"clean":"pass","compile":"pass","run":"fail","note":"skipped"}})

    # Sort by "test" field, place entries without 'test' key at the start
    combined_data.sort(key=lambda x: x.get("test", ""))

    with open("combined.json", 'w') as f:
        json.dump(combined_data, f, indent=4)

if __name__ == "__main__":
    main()

      
#!/bin/bash

# --- Configuration ---
VALIDATION_TESTS_DIR="validation_tests" 
CPU_TESTS_DIR="cpu_tests"
GPU_TESTS_DIR="gpu_tests"
CUDA_TESTS_DIR="cuda_tests"
ROCM_TESTS_DIR="rocm_tests"

# --- Functions ---

create_symlink() {
  local dest_dir="$1"    # e.g., cpu_tests/my_test
  local target_dir="$2"  # e.g., ../validation_tests/my_test

  # Create the symlink, overwriting if it exists (-f) and using a relative path (-s)
  echo "Creating symlink: $dest_dir -> $target_dir"
  ln -sf "$target_dir" "$dest_dir"
}

# --- Main ---

# Create the target directories if they don't exist
mkdir -p "$CPU_TESTS_DIR" "$GPU_TESTS_DIR" "$CUDA_TESTS_DIR" "$ROCM_TESTS_DIR"

# Iterate through the subdirectories in validation_tests
find -L "$VALIDATION_TESTS_DIR" -maxdepth 1 -mindepth 1 -type d ! -print0 | while IFS= read -r -d $'\0' subdir; do
  # Extract the directory name without the full path
  dir_name=$(basename "$subdir")

  # Construct the relative path for the symlink target
  relative_path="../${VALIDATION_TESTS_DIR}/${dir_name}"

  # Check for suffixes and create links accordingly
  if [[ "$dir_name" == *-cuda || "$dir_name" == "cuda" ]]; then
    create_symlink "$GPU_TESTS_DIR/$dir_name" "$relative_path"
    create_symlink "$CUDA_TESTS_DIR/$dir_name" "$relative_path"
  elif [[ "$dir_name" == *-rocm ]]; then
    create_symlink "$GPU_TESTS_DIR/$dir_name" "$relative_path"
    create_symlink "$ROCM_TESTS_DIR/$dir_name" "$relative_path"
  else
    create_symlink "$CPU_TESTS_DIR/$dir_name" "$relative_path"
  fi
done

echo "Done."

    

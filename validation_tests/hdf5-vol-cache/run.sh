#!/bin/bash
. ./setup.sh
set -x
set -e
for file in *; do
  # Check if the file is a compiled binary
  if file "$file" | grep -q 'ELF' && [[ "$file" != *.o ]]; then
    echo "Executing $file"
    ${TEST_RUN} ./$file
  fi
done

 

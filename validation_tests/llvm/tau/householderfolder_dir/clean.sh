#!/bin/bash

read -r -d '' FILES << EOF
householder_folder
profile.*
*.o
*.log
.symbols
EOF

echo rm -f $FILES
rm -f $FILES

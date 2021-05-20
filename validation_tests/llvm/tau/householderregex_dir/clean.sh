#!/bin/bash

read -r -d '' FILES << EOF
householder
householder_exclude
profile.*
*.o
*.log
.symbols
EOF

echo rm -f $FILES
rm -f $FILES

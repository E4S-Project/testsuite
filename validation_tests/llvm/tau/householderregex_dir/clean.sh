#!/bin/bash

read -r -d '' FILES << EOF
householder
profile.0.0.0
*.o
*.log
.symbols
EOF

echo rm -f $FILES
rm -f $FILES

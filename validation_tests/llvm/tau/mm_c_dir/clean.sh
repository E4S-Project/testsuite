#!/bin/bash

read -r -d '' FILES << EOF
mm_c
profile.0.0.0
*.o
*.log
.symbols
EOF

echo rm -f $FILES
rm -f $FILES

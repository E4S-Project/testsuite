#!/bin/bash

read -r -d '' FILES << EOF
mm_c
profile.*
*.o
*.log
.symbols
EOF

echo rm -f $FILES
rm -f $FILES

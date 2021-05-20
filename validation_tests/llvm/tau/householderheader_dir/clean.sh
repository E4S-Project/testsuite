#!/bin/bash

read -r -d '' FILES << EOF
householderheader
profile.*
*.o
*.log
.symbols
EOF

echo rm -f $FILES
rm -f $FILES

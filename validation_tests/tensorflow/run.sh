#!/bin/bash
. ./setup.sh


if [ $(nvidia-smi >& /dev/null; echo $?) == 0 ]; then
    HARD=GPU
    BRAND=NVIDIA
elif [ $(rocminfo >& /dev/null; echo $?) == 0 ]; then
    HARD=GPU
    BRAND=AMD
else    
    HARD=CPU
    BRAND=NA
fi

VERSION=$(python -c "import tensorflow as tf; print(tf.__version__ )"| grep -o '^[^.]')

echo "Running: python tf.py $HARD $BRAND $VERSION"
python tf.py $HARD $BRAND $VERSION


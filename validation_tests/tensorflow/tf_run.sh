#!/bin/bash

if [ nvidia-smi ]; then
        HARD=GPU
    else
        HARD=CPU
fi

VERSION=$(python -c "import tensorflow as tf; print(tf.__version__ )"| grep -o '^[^.]')

python tf.py $HARD $VERSION


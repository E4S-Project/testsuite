#!/bin/bash

spack load \trrej6a
spack load mpich
module load hdf5

h5pcc -o ph5example ./ph5example.c



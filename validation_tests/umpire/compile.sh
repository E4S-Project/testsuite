#!/bin/bash

spack load umpire 

g++ malloc.cxx -lumpire -lumpire_op -lumpire_resource -lumpire_strategy -lumpire_util -lumpire_tpl_judy



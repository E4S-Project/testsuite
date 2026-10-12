#!/bin/bash
. ./setup.sh
set -e
set -x
export OMP_NUM_THREADS=8
grep -v HOH 1aki.pdb > 1AKI_clean.pdb
echo "15" | ${TEST_RUN_SEQ} gmx_mpi pdb2gmx -f 1AKI_clean.pdb -o 1AKI_processed.gro -water spce
${TEST_RUN_SEQ} gmx_mpi editconf -f 1AKI_processed.gro -o 1AKI_newbox.gro -c -d 1.0 -bt cubic
${TEST_RUN_SEQ} gmx_mpi solvate -cp 1AKI_newbox.gro -cs spc216.gro -o 1AKI_solv.gro -p topol.top
${TEST_RUN_SEQ} gmx_mpi grompp -f ions.mdp -c 1AKI_solv.gro -p topol.top -o ions.tpr -maxwarn 1
echo "13" | ${TEST_RUN_SEQ} gmx_mpi genion -s ions.tpr -o 1AKI_solv_ions.gro -p topol.top -pname NA -nname CL -neutral
${TEST_RUN_SEQ} gmx_mpi grompp -f minim.mdp -c 1AKI_solv_ions.gro -p topol.top -o em.tpr -maxwarn 1
${TEST_RUN} gmx_mpi mdrun -v -deffnm em
echo "10 0 " | ${TEST_RUN_SEQ} gmx_mpi energy -f em.edr -o potential.xvg


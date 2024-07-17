#!/bin/bash
. ./setup.sh
set -e
set -x
export OMP_NUM_THREADS=8
grep -v HOH 1aki.pdb > 1AKI_clean.pdb
echo "15" | gmx_mpi pdb2gmx -f 1AKI_clean.pdb -o 1AKI_processed.gro -water spce
gmx_mpi editconf -f 1AKI_processed.gro -o 1AKI_newbox.gro -c -d 1.0 -bt cubic
gmx_mpi solvate -cp 1AKI_newbox.gro -cs spc216.gro -o 1AKI_solv.gro -p topol.top
gmx_mpi grompp -f ions.mdp -c 1AKI_solv.gro -p topol.top -o ions.tpr
echo "13" | gmx_mpi genion -s ions.tpr -o 1AKI_solv_ions.gro -p topol.top -pname NA -nname CL -neutral
gmx_mpi grompp -f minim.mdp -c 1AKI_solv_ions.gro -p topol.top -o em.tpr
${TEST_RUN} gmx_mpi mdrun -v -deffnm em
echo "10 0 " | gmx_mpi energy -f em.edr -o potential.xvg 
 

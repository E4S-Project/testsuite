CFLAGS           =
FFLAGS		 =
CPPFLAGS         =
FPPFLAGS         =

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

#-------------------------------------------------------------------------

runex19_hypre:
	-@${MPIEXEC} -n 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type hypre > ex19_1.tmp 2>&1; \
	   if (${DIFF} output/ex19_hypre.out ex19_1.tmp) then \
           echo "C/C++ example src/snes/examples/tutorials/ex19 run successfully with hypre"; \
           else  printf "${PWD}\nPossible problem with ex19 running with hypre, diffs above\n=========================================\n"; fi; \
	   ${RM} -f ex19_1.tmp
runex19_ml:
	-@${MPIEXEC} -n 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type ml > ex19_1.tmp 2>&1; \
	   if (${DIFF} output/ex19_ml.out ex19_1.tmp) then  \
           echo "C/C++ example src/snes/examples/tutorials/ex19 run successfully with ml"; \
           else  printf "${PWD}\nPossible problem with ex19 running with ml, diffs above\n=========================================\n"; fi; \
           ${RM} -f ex19_1.tmp
runex19_superlu_dist:
	-@${MPIEXEC} -n 1 ./ex19 -da_grid_x 20 -da_grid_y 20 -pc_type lu -pc_factor_mat_solver_type superlu_dist > ex19.tmp 2>&1; \
	   if (${DIFF} output/ex19_superlu.out ex19.tmp) then \
           echo "C/C++ example src/snes/examples/tutorials/ex19 run successfully with superlu_dist"; \
           else  printf "${PWD}\nPossible problem with ex19 running with superlu_dist, diffs above\n=========================================\n"; fi; \
	   ${RM} -f ex19.tmp

include ${PETSC_DIR}/lib/petsc/conf/test

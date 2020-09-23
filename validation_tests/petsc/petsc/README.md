# PETSc-HYPRE-SuperLU-ML example
This example sets up and solves a 2D driven cavity problem on the unit square.  A finite
difference method with a 5 point stencil is used to discretize the velocity-vorticity
formulation and create operators and vectors in the PETSc framework.  The PETSc
SNES (Scalable Nonlinear Equations Solvers) components are then used to solve the
nonlinear system that arises.  For each iteration of the nonlinear solution, a system
a linear equations is solved with either HYPRE or SuperLU, ML from Trilinos.

This example is built to run in parallel, so launch it with mpirun and your desired options.

To run with Hypre:
```
mpirun -np 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type hypre
```
To run with SuperLU:
```
mpirun -np 2 ./ex19 -da_grid_x 20 -da_grid_y 20 -pc_type lu -pc_factor_mat_solver_type superlu_dist
```
To run with ML from Trilinos:
```
mpirun -np 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type hypre
```

Useful non-default options:
|   Flag                | Meaning                                               |
|:----------------------| :-----------------------------------------------------|
| -pctype [type]        | Set the pc solver type for example: hypre, ml, lu.    |
| -da_refine n          | Set the number of times to refine the mesh.           |
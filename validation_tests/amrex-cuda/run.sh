#!/bin/bash -ex
#./build/Amr/Advection_AmrCore/Advection_AmrCore ./build/Amr/Advection_AmrCore/inputs
#./build/Basic/HeatEquation_EX0_C/HeatEquation_EX0_C build/Basic/HeatEquation_EX0_C/inputs
#./build/Basic/HelloWorld_C/HelloWorld_C
timeout -k 15 300 ./build/Basic/main_C/main_C
#./build/LinearSolvers/ABecLaplacian_C/ABecLaplacian_C build/LinearSolvers/ABecLaplacian_C/inputs
#./build/LinearSolvers/NodalPoisson/NodalPoisson build/LinearSolvers/NodalPoisson/inputs-rt
timeout -k 15 300./build/LinearSolvers/NodeTensorLap/NodeTensorLap
timeout -k 15 300./build/GPU/CNS/RT/GPU_CNS_RT ./build/GPU/CNS/RT/inputs-rt
#./build/GPU/CNS/Sod/GPU_CNS_Sod ./build/GPU/CNS/Sod/inputs-rt

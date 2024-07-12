# SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: LicenseRef-NvidiaProprietary
#
# NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
# property and proprietary rights in and to this material, related
# documentation and any modifications thereto. Any use, reproduction,
# disclosure or distribution of this material and related documentation
# without an express license agreement from NVIDIA CORPORATION or
# its affiliates is strictly prohibited.

#
# just typing 'make' prints out this help message
#

UNAME := $(shell uname -a)
ifeq ($(findstring CYGWIN_NT, $(UNAME)), CYGWIN_NT)
help:
	@echo "To build and run a specific example, do the following:"
	@echo "    "
	@echo "make buflimit_test                 : MPI buffering example"
	@echo "make [NPROCS=n] copy_test          : MPI point-to-point copy test"
	@echo "make [NPROCS=n] mpihello_test      : MPI Hello world example"
	@echo "make [NPROCS=n] mpp_test           : MPI communication performance test"
	@echo ""
	@echo "where NPROCS is set to the number of processors to test, default is 2."
else
help:
	@echo "To build and run a specific example, do the following:"
	@echo "    "
	@echo "make buflimit_test                 : MPI buffering example"
	@echo "make [NPROCS=n] copy_test          : MPI point-to-point copy test"
	@echo "make [NPROCS=n] gop_test           : MPI collectives test"
	@echo "make [NPROCS=n] mpihello_test      : MPI Hello world example"
	@echo "make [NPROCS=n] mpp_test           : MPI communication performance test"
	@echo ""
	@echo "make scalapack_test                : Scalapack LU Test"
	@echo ""
	@echo "make [NPROCS=n] all                : all of the above tests"
	@echo ""
	@echo "where NPROCS is set to the number of processors to test, default is 2."
endif

NPROCS = 2

buflimit_test:
	cd samples/buflimit; make build; make run; make clean

copy_test:
	cd samples/copytest; make build; make NPROCS=$(NPROCS) run; make clean

ifneq ($(findstring CYGWIN_NT, $(UNAME)), CYGWIN_NT)
gop_test:
	cd samples/goptest; make build; make NPROCS=$(NPROCS) run; make clean
endif

mpihello_test:
	cd samples/mpihello; make build; make NPROCS=$(NPROCS) run; make clean

mpp_test:
	cd samples/mpptest; make build; make NPROCS=$(NPROCS) run; make clean

ifneq ($(findstring CYGWIN_NT, $(UNAME)), CYGWIN_NT)
scalapack_test:
	cd scalapack; make build; make NPROCS=$(NPROCS) run; make clean
endif

ifeq ($(findstring CYGWIN_NT, $(UNAME)), CYGWIN_NT)
all: buflimit_test copy_test mpihello_test mpp_test
else
all: buflimit_test copy_test gop_test mpihello_test mpp_test scalapack_test
endif

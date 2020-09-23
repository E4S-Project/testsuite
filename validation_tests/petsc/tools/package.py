# Copyright 2013-2020 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack import *

class XsdkExamples(CMakePackage):
    """XSDK Examples show usage of libraries in the XSDK package."""

    homepage = "http://xsdk.info"
    url      = "http://github.com/xsdk-project/xsdk-examples/xsdk-examples-0.1.0.tar.gz"

    maintainers = ['balos1', 'luszczek']

    version('0.1.0', '8a2561d48bea92cdecf16e428f876f30')

    variant('cuda', default=False, description='Enable CUDA dependent packages')

    depends_on('xsdk@develop', when='@develop')
    depends_on('xsdk@0.5.0', when='@0.1.0')
    depends_on('mpi')

    def cmake_args(self):
        spec = self.spec
        args = [
            '-DCMAKE_C_COMPILER=%s' % spec['mpi'].mpicc,
            '-DMPI_DIR=%s' % spec['mpi'].prefix,
            '-DSUNDIALS_DIR=%s'     % spec['sundials'].prefix,
            '-DPETSC_DIR=%s'         % spec['petsc'].prefix,
            '-DPETSC_INCLUDE_DIR=%s' % spec['petsc'].prefix.include,
            '-DPETSC_LIBRARY_DIR=%s' % spec['petsc'].prefix.lib,
            '-DSUPERLUDIST_INCLUDE_DIR=%s' % spec['superlu-dist'].prefix.include,
            '-DSUPERLUDIST_LIBRARY_DIR=%s' % spec['superlu-dist'].prefix.lib,
        ]
        if 'trilinos' in spec:
            args.extend([
                '-DTRILINOS_DIR:PATH=%s' % spec['trilinos'].prefix,
            ])
        return args

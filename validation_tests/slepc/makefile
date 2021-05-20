hello: hello.o chkopts
	-${CLINKER} -o hello hello.o -L${ARPACK_NG_LIB_PATH}  ${SLEPC_SYS_LIB}
	${RM} hello.o

include ${SLEPC_ROOT}/lib/slepc/conf/slepc_common

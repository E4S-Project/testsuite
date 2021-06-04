all: test-householderregex test-mm_c test-householderfolder test-householderheader test-householder
clean: clean-householder3 clean-householderregex clean-mm_c clean-householderfolder clean-householderheader clean-householder


test-%: %_dir
	@export QUIET_TEST_OUTPUT=1; cd $<; bash compile.sh && bash run.sh

clean-%: %_dir
	cd $<; bash clean.sh

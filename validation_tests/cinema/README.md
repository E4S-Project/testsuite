# E4S test script for Cinema capabilities
  
This test does the following:

1. clones a Cinema repository at a specific commit
2. runs a specific test from the test suite
3. cleans up
4. reports the results of the test

## Requirements

1. runs under spack v0.17.0
2. spack is available on the command line, BUT
3. the shell environment is not loaded. Instead, *spack load* is managed
   through the script

## Known issues

Under certain conditions, the version of git managed by the *spack*
environment can interfere with *git* operations. Therefore, this
script should be run in an environment in which the script can manage
*spack load* operations.

# Information 

For more information, see the Cinema git group: github.com/cinemascience


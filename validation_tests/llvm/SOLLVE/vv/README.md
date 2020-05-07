## Setup

We assume that the sollve/git module defines a ```$SOLLVE``` environment variable.

For instance:
```
set     SWTOP   $HOME/$ARCH
set     SOLLVE  $SWTOP/sollve_vv
```

Therefore, after sourcing ```setup.sh```, we can use this environment variable.

## Compile

Nothing to do.

Maybe: add a sanity check?

## Run

We go to the installation directory (using the ```$SOLLVE``` environment variable that was defined earlier), run the tests, generate the report, move it here and move back to this directory.



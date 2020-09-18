# Opbox job-to-job communication example

## Description

This is a simple program to demonstrate job-to-job comunication in
Opbox.  The servers collect their nodeids, write them to a file, sit
for 30 seconds and responds to any client pings.  The clients read
the nodeid file, connect to all servers and launch a OpInterjobPing.
There are scripts for kahuna and mutrino.


## Build Instructions

This is a connectivity test not a performance test.  Since there
will likely be connection errors, send failures and general
hanging, it is best to build with the debugging turned on.

At the very least NNTI and Opbox should be built with:

`-DCMAKE_BUILD_TYPE=DEBUG`
`-DNNTI_BUILD_TYPE=DEBUG`

Building the entire FAODEL stack with these options can't hurt.


## Prerequisites

This test requires the entire FAODEL stack: sbl, common, whookie, nnti,
lunasa, and opbox.


## Options

This example has no command line options.

This example has one option controlled by Configuration.

job2job.nodeid_file - the name of the file where nodeids are written and read


## Execution

This example comes with helper scripts for kahuna and mutrino.
Similar systems should work with these scripts (maybe with slight
modifications).

These script have a usage message if given no parameters.

The script parameters are positional and cannot be reordered.  There
are at least 2 required parameters to each script - transport and
number of clients/servers.  The remaining parameters have reasonable
defaults.

transport                  - select a transport mpi/ibverbs/ugni.  loads the related Configuration file.
num_{servers,clients}      - the number of servers/clients to launch
{servers,clients}_per_node - the number of servers/clients to launch on each node
hosts_to_run_on            - a list of nodes to run on.  must be in platform format.


kahuna intra-allocation - MPMD mpi:

```
salloc --nodes=4 --ntasks-per-node=1 --time=30
$ ## load your modules here
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/job2job
$ ./kahuna-mpmd.sh mpi 2 2
```

kahuna intra-allocation - MPMD ibverbs:

```
salloc --nodes=4 --ntasks-per-node=1 --time=30
$ ## load your modules here
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/job2job
$ ./kahuna-mpmd.sh ibverbs 2 2
```

kahuna intra-allocation - separate server/client jobs:

```
salloc --nodes=4 --ntasks-per-node=1 --time=30
$ ## load your modules here
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/job2job
$ ./kahuna-server.sh ibverbs 2 &
$ ./kahuna-client.sh ibverbs 2
```

kahuna inter-allocation:

```
salloc --nodes=2 --ntasks-per-node=1 --time=30
$ ## load your modules here
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/job2job
$ ./kahuna-server.sh ibverbs 2
```
```
salloc --nodes=2 --ntasks-per-node=1 --time=30
$ ## load your modules here
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/job2job
$ ./kahuna-client.sh ibverbs 2
```

mutrino intra-allocation - MPMD mpi:

```
msub -l nodes=4:haswell -l walltime=1:00:00 -I
$ ## load your modules here
$ cd /projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/examples/job2job
$ ./mutrino-mpmd.sh mpi 2 2
```

mutrino intra-allocation - MPMD ugni:

```
msub -l nodes=4:haswell -l walltime=1:00:00 -I
$ ## load your modules here
$ cd /projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/examples/job2job
$ ./mutrino-mpmd.sh ugni 2 2
```

mutrino intra-allocation - separate server/client jobs:

```
msub -l nodes=4:haswell -l walltime=1:00:00 -I
$ ## load your modules here
$ cd /projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/examples/job2job
$ ./mutrino-server.sh ugni 2 &
$ ./mutrino-client.sh ugni 2
```

mutrino inter-allocation:

```
msub -l nodes=4:haswell -l walltime=1:00:00 -I
$ ## load your modules here
$ cd /projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/examples/job2job
$ ./mutrino-server.sh ugni 2
```
```
msub -l nodes=4:haswell -l walltime=1:00:00 -I
$ ## load your modules here
$ cd /projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/examples/job2job
$ ./mutrino-client.sh ugni 2
```


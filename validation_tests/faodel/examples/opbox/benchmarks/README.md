# Opbox Benchmarks

This is where we will collect all of the benchmarks for the different aspects of OpBox.

This document should include a section for each benchmark describing what the benchmark does, any required or optional prerequisites, any configuration options and detailed execution instructions.

## Short Message Benchmark (OpBenchmarkPing)

### Description

This is a simple latency benchmark for small (<MTU) sized messages.  Messages are sent in bursts of messages until the total number are sent.  The payload size of the message can be set from the command-line.


### Build Instructions

For best performance all the packages should be built with the highest levels of optimization and the least amount of overhead.

All faodel packages should be built with:

`-DCMAKE_BUILD_TYPE=RELEASE`

In addition, NNTI should be built with:

`-DNNTI_BUILD_TYPE=NORMAL`


### Prerequisites

None.


### Options

`--count`    -- the number of messages to send (default: 25)

`--length`   -- the payload size of the message (default: 128 bytes)

`--inflight` -- the number of outstanding messages allowed (default: 10)


### Execution

kahuna mlx5:

```
salloc --nodes=2 --ntasks-per-node=16 --time=480
$ export CONFIG=/home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/opbox.ibverbs.conf
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/benchmarks/short_message
$ mpirun -npernode 1 -np 2 ./short_message --count 20 --length 128 --inflight 1
```

kahuna omnipath:

```
salloc -p omnipath --nodes=2 --ntasks-per-node=16 --time=480
$ export CONFIG=/home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/opbox.ibverbs.conf
$ cd /home/thkorde/projects/atdm/src/opbox/build/kahuna/gnu/examples/benchmarks/short_message
$ mpirun -npernode 1 -np 2 ./short_message --count 20 --length 128 --inflight 1
```

mutrino aries:

```
msub -l nodes=2:haswell -l walltime=2:00:00 -I
$ export CONFIG=/projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/opbox.ugni.conf
$ cd /projects/trios/thkorde/src/projects/atdm/src/opbox/build/mutrino/gnu/examples/benchmarks/short_message
$ aprun -N 1 -n 2 ./short_message --count 20 --length 128 --inflight 1
```

curie gemini:

```
msub -lwalltime=6:00:00,nodes=2:ppn=16 -I
$ export CONFIG=/home/thkorde/projects/atdm/src/opbox/build/curie/gnu/opbox.ugni.conf
$ cd /home/thkorde/projects/atdm/src/opbox/build/curie/gnu/examples/benchmarks/short_message
$ aprun -N 1 -n 2 ./short_message --count 20 --length 128 --inflight 1
```

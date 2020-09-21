OpBox Examples
==============

The OpBox project provides a communication layer that uses state
machines to orchestrate data transfers between nodes. This directory
contains some basic examples of how developers can use OpBox. The
current set of examples includes:

link_example
------------
This example simply shows whether opbox links properly with a
standalone application. If builds don't work, start here.

my_simple_ping
--------------
This mpi example implements a minimal ping-pong communication between
a pair of nodes. It has a complete and minimal Op that uses C++
futures to pass back results to the application.

rapidfire
---------
This example uses a single op to send a series of pingpong messages
between a pair of nodes. The advantage here is that the op is created
once and then handed off to the system for asynchronous completion (ie
fire-and-forget).

ringer
------
The ringer example demonstrates how an op can be used to send
information around a circle of nodes, with each one updating its part
of the data.

rdma_ping
---------
The rdma_ping example shows how to orchestrate RDMA transfers between
nodes.

scatter_gather
--------------
The scatter_gather example demonstrates how a collection of nodes can
allocate buffers and then use RDMAs to move data between the buffers.

dirman
------
The DirectoryManager service is a service built into OpBox that
provides users with a simple way to share runtime info. It is not
designed to be reliable or dynamic. This example has clients register
basic information about a service with a server.

job2job
-------
OpBox is designed to be able to support communication between two
different jobs on HPC platforms. This example provides a client/server
example as well as configuration scripts for common platforms we use.

dirty_use
---------
OpBox should be able to survive most uses, even if our users don't do
things the way we expect. This directory provides a few examples of
use, where aplications don't clean up after themselves.


Support Notes
=============
The OpBox examples are all launched with mpi and use a library called
Globals (in support) to start their environment properly).

Services Examples
=================
FAODEL provides a few built-in services to make it easier to write jobs. 
Backburner is a thread that allows users to queue up work that should
be run at a later point in time (eg to avoid deadlock). Mpi_sync_start
is a service that uses mpi to pass configuration info between the nodes
and ensure synchronization at startup.

backburner_example
------------------
This example shows how to insert several simple tasks into backburner
and verify they've completed.

mpi_sync_start_basic
--------------------
This example just starts bootstrap with mysyncstart enabled. It does a
barrier at startup to make sure ranks don't begin work before all
the ranks are running.

mpi_sync_start_advanced
-----------------------
This example shows how you can have mpi_sync_start translate an mpi
rank id into a Faodel nodeid in the configuration so nodes don't have
to manually figure it out.

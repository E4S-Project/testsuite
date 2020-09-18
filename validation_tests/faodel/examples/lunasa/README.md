Lunasa Examples
================

The Lunasa project provides the programmer a simple and efficient interface
to explicitly reuse network-registered memory.  The performance benefits of
eliminating repeated registration and de-registration operations are, in
many cases, significant.  Memory allocated by Lunasa is accessed and managed 
through instances of the DataObject class.  These objects are commonly 
referred to as Lunasa Data Objects (LDOs).

This directory contains some basic examples of how to use the services 
provided by Lunasa.  The current set of examples are:

bootstrap
--------------
A straightforward example of how to initialize Lunasa so that it is 
ready and able to allocate memory.

simple_ldo_send
-----------------
This is an example demonstrating how memory acquired from Lunasa can be
transferred from one node to another using a zero-copy send.

*NOTE*: this example is (currently) dependent on NNTI.  If Lunasa is not
built with support for NNTI, this example is not built.

simple_user_ldo_put
-----------------
This is an example demonstrating how an LDO can be created from user-allocated
memory.  It also demonstrates how that memory can be transferred across the 
network with an RDMA operation.  

*NOTE*: this example is (currently) dependent on NNTI.  If Lunasa is not
built with support for NNTI, this example is not built.

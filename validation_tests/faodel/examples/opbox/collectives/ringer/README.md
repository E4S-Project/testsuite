Ringer Example
==============

The ringer example demonstrates how to use an op to implement a simple
ring of communication. The origin node generates a list of all the
nodes in the ring and then transmits it to the first node in the
list. Each node unpacks the message, inserts its own results, and then
transmits it to the next node on the list. The last node transmits the
message back to the origin, where it is unpacked and printed out.

RingInfo Class
--------------
The RingInfo class is an object that is
passed around and modified by nodes in the ring. It contains the list
of nodes in the ring as well as a vector of each node's data
contribution. The RingInfo class is serialized through Boost, and
inserted into the body section of a standard message using the
AllocateBoostMessage() and UnpackBoostMessage() helper templates.

Notes
-----

- **Boost Includes**: Since RingInfo has a vector in it, you need to make
  sure you include boost/serialization/vector.hpp
- **Request vs Forward**: The first time the message is created the
  origin can use the AllocateBoostRequestMessage to fill in the
  data. However, the other nodes need to use AllocateBoostMessage
  because they need to fill in fields that would not be availble in a
  reply.

Whookie Examples
================

The Whookie project provides a simple way to exchange information with
a running node via an http interface. The current set of examples
includes:

bootstrap_example
-----------------
This example uses bootstrap to start up a node with a whookie
server. It prints out a url for a service named "bob" that users can
access with a web browser client (eg, curl, lynx, or firefox). The
service prints out a simple info page. The application runs for a
short amount of time and then shuts itself down.

killit_example
--------------
The killit example is similar to the bootstrap example, except that it
demonstrates how a whookie can be used to actively change the state of
the application. The killit hook in this example triggers a shutdown
of the application.

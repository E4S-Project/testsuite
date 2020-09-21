#!/bin/bash

# This script walks through the basics of using the faodel command line tool
# to launch different services, query them, and shut them down. It assumes
# that you've installed faodel somewhere and that the faodel tool is in your
# current path. You can run this on your local machine, but you'll need a
# transport like IB for this to work.
#
# Type 'faodel help' to get a list of complete list of tool options,
# and  'faodel help operation' to get info about a specific operation.
#
# Note: page delays can be turned off on this script by supply one or more args


#
# The faodel tool doesn't require MPI, but most of our platforms need 
# to have it launched onto a compute node.  Look for mpirun and srun 
# take care of that.
#
type mpirun > /dev/null 2>&1
if [ $? -eq 0 ] ; then
  LAUNCHER=mpirun
else
  type srun > /dev/null 2>&1
  if [ $? -eq 0 ] ; then
    LAUNCHER=srun
  else
    echo "$0: Couldn't find mpirun or srun.  Exiting."
    exit
  fi
fi

NO_PAUSE=$#
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

GreenText() {
    echo -e ${GREEN}
}
YellowText() {
    echo -e ${YELLOW}
}
PlainText() {
    echo -e ${NC}
}
PressEnter() {
    if [ $NO_PAUSE -eq 0 ]; then
        GreenText
        read -p "Press enter to continue$"
    fi
    YellowText
}



YellowText
cat <<EOF
Step 1: Launch a dirman server
------------------------------
 DirMan is a service for tracking different resources in the cluster and
 will help us control what resources we have. We can launch a plain dirman
 server with the faodel tool. By default it creates a file named
 '.faodel-dirm in your current directory to tell nodes where to look. Other
 operations in the faodel tool will use this to locate the server. This
 example uses the .faodel-dirman file to reference dirman to simplify
 command line options. 

EOF
PlainText

# Remove any existing dirman info file from previous runs
rm -f .faodel-dirman

# Launch a new dirman server
echo "Launching a new dirman server"
$LAUNCHER -n 1 faodel dirman-start &

# Wait until the file exists
until [  -f .faodel-dirman ]; do sleep 1; done

echo "DirMan seems to be running!"




PressEnter
cat <<EOF
Step 2: Query DirMan and Define a DHT
-------------------------------------
 Now that DirMan is running we can create new resources for it and query
 it to learn where things are located. In this step we want to define a
 new pool resource named '/my/dht1' and specify that it is a DHT that needs
 four members in order to function. At this stage we don't know what nodes
 are going to be participating- that info will get filled in during the
 next step when we launch Kelpie servers and tell them to join this 
 pool.

 After the resource is defined we should be able to get info from it in
 dirman. There's a delay in creation, so they might not all show up in
 a list command immediately. Operations to the pool will block until the
 pool is fully functional, though.

EOF
PlainText

# See what's in the root dir
$LAUNCHER -n 1 faodel rlist /

# Define a resource that needs four members
$LAUNCHER -n 1 faodel rdef "dht:/my/dht1&min_members=4"

# Show that it's empty
$LAUNCHER -n 1 faodel rlist /my/dht1



PressEnter
cat <<EOF
Step 3: Launch Kelpie nodes to serve in DHT
-------------------------------------------
 Once the pool is defined we can launch some kelpie servers and tell them to
 join it. The kelpie-start tool launches a single kelpie server that will try
 to join all the pools you list on the command line. It will block until each
 pool can be obtained from DirMan, so the above delay in defining the above
 pool shouldn't matter. While the faodel tool does not use mpi, we still use
 the job launcher here to make sure the instances get launched in our 
 allocation.

EOF
PlainText

$LAUNCHER -n 4 faodel kelpie-start /my/dht1 &

# Give the nodes a chance to register and then check
sleep 2
$LAUNCHER -n 1 faodel rlist /my/dht1



PressEnter
cat <<EOF
Step 4: Copy some data into the kelpie nodes
--------------------------------------------
 While Kelpie is meant to be a service you manipulate through code, you can
 use the faodel cli tool to put, get, and list data items. The kelpie-put
 option lets you create a new object and push it into the store. You can
 specify that the data comes from either stdin or a file. In this example
 we will create 10 items in the pool, all with row-named keys that have
 the prefix "mykey". This task can be slow because each instance has to 
 discover the pool's info and safely publish data to it.

EOF
PlainText

echo "Creating 10 simple blobs and inserting them into kelpie"
for i in $(seq 1 10); do
    echo "This is blob $i" | $LAUNCHER -n 1 faodel kput -p "/my/dht1" -k "mykey$i" 
done



PressEnter
cat <<EOF
Step 5: See what keys we have
-----------------------------
 We can use the kelpie-list command to get a list of the keys that are in
 a pool. This command will let you use simple wildcards to locate all keys
 that match a certain prefix. The results of this command show all the 
 keys that were found and their user sizes.

EOF
PlainText

echo "Do a search for mykey* and see how big each item is"
$LAUNCHER -n 1 faodel klist -p "/my/dht1" -k "mykey*"



PressEnter
cat <<EOF
Step 6: Retrieve one of the items
---------------------------------
 You can use the kelpie-get command to retrieve an item that is stored in
 a pool. The default is to pipe the contents to stdout.

 Warning: If the item is not available, this command will block until 
          the object is published into the system. 

EOF
PlainText

echo "Retrieving mykey6"
$LAUNCHER -n 1 faodel kget -p "/my/dht1" -k "mykey6"



PressEnter
cat <<EOF
Step 7: Shutdown the Kelpie pool and DirMan Server
--------------------------------------------------
 When we're done with a workflow we need to terminate all our services. For 
 this example we first stop our pool amd then stop DirMan.

 Warning: If you use kstop nodes will still be listed in DirMan. A more 
          thorough approach would be to instead use resource-kill, which
          erases the DirMan info and then shuts down the pool.
 
EOF
PlainText

$LAUNCHER -n 1 faodel kstop "/my/dht1"
$LAUNCHER -n 1 faodel dirman-stop


echo "All done with this example."

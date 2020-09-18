// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "kelpie/Kelpie.hh"


using namespace std;



void ex1_get_pool_handle(){

  cout <<"example1: get pool handle\n";


  //First, we need to connect to our pool. In these examples we're only talking
  //to the local k/v store that runs on this node. This unit can be referenced
  //as "local:", "localkv:", or "lkv:". The reference can be either a url
  //string, or an actual URL.
  faodel::ResourceURL url("local:");

  kelpie::Pool lkv1 = kelpie::Connect("local:");
  kelpie::Pool lkv2 = kelpie::Connect("lkv:");
  kelpie::Pool lkv3 = kelpie::Connect(url);

  //Each of these pool handles maps to one instance, since they're all for the
  //same URL. You can do simple checks on the handles.
  if( (lkv1!=lkv2) || (lkv2!=lkv3)){
    cerr <<"Error: lkv's don't match?\n";
  }


  //Kelpie supports the concept of "buckets" which is just a simple way of
  //grouping related k/v pairs together. A bucket is just an extra tag that
  //gets hashed and prepended to the key during kv ops. The above lkv's all
  //used the same default bucket, which is often specified in a
  //FAODEL Configuration.

  //We can create a custom pool for dealing with a specific bucket. These
  //should resolve to a different lkv pool handle than the above.
  kelpie::Pool lkv_bob1 = kelpie::Connect("local:[bob]");
  kelpie::Pool lkv_bob2 = kelpie::Connect("local:[0xb886278]");
  kelpie::Pool lkv_sue1 = kelpie::Connect("local:[sue]");

  //Make sure bob, sue, and default don't match
  if( (lkv_bob1==lkv1) || (lkv_bob1==lkv_sue1) ){
    cerr<<"Error: bob and sue lkv's matched?\n";
  }

  //Check to make sure bob1 and bob2 match. The 0xb886278 hash was found
  //by running the program one time and dumping the results.
  if( lkv_bob1!=lkv_bob2 ){
    cerr<<"Error: lkvs for bob did not match. Maybe our hashes are wrong.\n";
  }

  //You can quert the pool handle to get basic information back
  cout <<"The buckets and ref counts for our pools are:\n"
       <<"  default: "<< lkv1.GetBucket().GetHex()     <<" "<<lkv1.GetRefCount()<<endl
       <<"  bob:     "<< lkv_bob1.GetBucket().GetHex() <<" "<<lkv_bob1.GetRefCount()<<endl
       <<"  sue:     "<< lkv_sue1.GetBucket().GetHex() <<" "<<lkv_sue1.GetRefCount()<<endl;

}

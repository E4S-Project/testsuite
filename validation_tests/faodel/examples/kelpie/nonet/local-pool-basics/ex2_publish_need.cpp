// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "kelpie/Kelpie.hh"


using namespace std;



void ex2_publish_and_need(){

  kelpie::rc_t rc;

  cout << "example2: publish and need\n";

  kelpie::Pool lkv    = kelpie::Connect("local:");
  kelpie::Pool lkvBob = kelpie::Connect("local:[bob]");

  //Allocate two Lunasa Data Objects (LDOs) for storing different blocks
  //of integers. Ignore the meta section of the LDO for now.
  int num_words=1024;
  lunasa::DataObject ldo1(num_words*sizeof(int));
  lunasa::DataObject ldo2(num_words*sizeof(int));

  cout << "ldo1 sizes:"
       << " meta="<<ldo1.GetMetaSize()
       << " data="<<ldo1.GetDataSize() <<endl;

  //Fill in each ldo with different data values
  int *data1 = ldo1.GetDataPtr<int *>();
  int *data2 = ldo2.GetDataPtr<int *>();
  for(int i=0; i<num_words; i++) {
    data1[i]=i;
    data2[i]=1000+i;
  }

  cout <<"Before publishing, we expect the ldo1 refcount to be 1. The ldo's refcount is "<<
         ldo1.internal_use_only.GetRefCount()<<endl;

  //Publish the two ldos using the same key, but different buckets.
  //Note: This does *not* make a copy of the data. It just places a reference
  //      to the data in the local kv store.
  kelpie::Key k1("My First Data Object");

  //lkv.Want(k1, [](bool success, kelpie::Key key, lunasa::DataObject ldo) {
  //    cout <<"Want is completed\n";
  //  }
  //);


  rc  =    lkv.Publish(k1, ldo1);
  rc |= lkvBob.Publish(k1, ldo2);
  if(rc!=0) cerr<<"Publish had problems?\n";

  cout <<"After publishing, we expect the ldo1 refcount to be 2. The ldo's refcount is "<<
         ldo1.internal_use_only.GetRefCount()<<endl;

  //Retrieve the ldos. This call blocks until the ldo becomes available.
  lunasa::DataObject ldo1_retrieved;
  lunasa::DataObject ldo2_retrieved;
  rc  =    lkv.Need(k1, &ldo1_retrieved);
  rc |= lkvBob.Need(k1, &ldo2_retrieved);
  if(rc!=0) cerr<<"Need() functions had problems?\n";


  //In this case, we should get back an ldo that points to the same chunk
  //of data as the original ldo. This demonstrates how the default behavior of
  //kelpie is to
  if( (ldo1!=ldo1_retrieved) ||
      (ldo2!=ldo2_retrieved)    )
    cerr <<"The ldo's we got back weren't identical\n";


}

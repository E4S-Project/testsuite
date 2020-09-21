// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

//This example shows how you can use an iom to store data persistently.
//
// An IOM is an i/o module that is responsible for exchanging key blobs
// with a persistent device. The simplest iom just writes each key/blob
// to its own file using plain posix commands.

#include <unistd.h>

#include "kelpie/localkv/LocalKV.hh"
#include "kelpie/ioms/IomRegistry.hh"
#include "kelpie/ioms/IomBase.hh"


using namespace std;
using namespace faodel;
using namespace kelpie;

//The configuration used in this example
std::string default_config_string = R"EOF(

# For local testing, tell kelpie to use the nonet implementation
kelpie.type nonet

default.iom.type PosixIndividualObjects
default.ioms     my_iom

# Define a place for an iom to write to. This may be modified at runtime if
# user passes a different directory on the command line
iom.my_iom.path /tmp/kelpie_data

)EOF";

struct my_data_t {
  uint32_t block_id;
  uint32_t data_bytes;
  char name[256];
  uint8_t data[0];
};

// Generate a chunk of data
lunasa::DataObject createLDO(int id, string name, int data_bytes) {
  lunasa::DataObject ldo(sizeof(my_data_t), data_bytes, lunasa::DataObject::AllocatorType::eager);

  auto mptr = ldo.GetMetaPtr<my_data_t *>();

  mptr->block_id = id;
  mptr->data_bytes = data_bytes;
  memset(mptr->name, 0, 256);
  strncpy(mptr->name, name.c_str(), 255);
  for(int i=0; i<data_bytes; i++)
    mptr->data[i] = (i&0x0FF);

  return ldo;
}


int main(int argc, char **argv) {

  //Default settings
  string path_name = "/tmp/kelpie_data";
  int NUM_ITEMS=10;
  bool is_writer=true;

  faodel::Configuration config(default_config_string);

  //Parse args
  int c;
  while((c=getopt(argc,argv,"d:n:wrh")) != -1) {
    switch(c){
    case 'd':
        path_name = string(optarg);
        config.Append("iom.my_iom.path", path_name);
        break;

    case 'n': NUM_ITEMS=atoi(optarg); break;
    case 'w': is_writer=true; break;
    case 'r': is_writer=false; break;
    default:
        cout << "storing-in-iom <options>\n"
             << "  -d dir_path  : directory to store data\n"
             << "  -n num_items : number of items to write\n"
             << "  -w|-r : is writer or reader\n";
        exit(-1);
    }
  }

  //Startup kelpie
  faodel::bootstrap::Start(config, kelpie::bootstrap);

  //Connect to our local iom pool
  kelpie::Pool piom = kelpie::Connect("[my_bucket]/local/iom/my_iom");


  if(is_writer) {
    cout <<"Writing "<<NUM_ITEMS<<" to "<<path_name<<endl;
    atomic<int> items_left(NUM_ITEMS);
    for (int i = 0; i < NUM_ITEMS; i++) {
      auto ldo = createLDO(i, "my brick_" + std::to_string(i), 1024);
      piom.Publish(kelpie::Key("my_brick_" + std::to_string(i)), ldo,
                   [&items_left](kelpie::rc_t result, kelpie::kv_row_info_t &ri, kelpie::kv_col_info_t &ci) {
                       items_left--;
                   });
    }
    //Wait until all publishes complete
    while (items_left) { sched_yield(); }
    cout << "Done writing to " << path_name << endl;

  } else {
    cout <<"Reading "<<NUM_ITEMS<<" from "<<path_name<<endl;
    for (int i = 0; i < NUM_ITEMS; i++) {
      lunasa::DataObject ldo;
      string key_name("my_brick_"+std::to_string(i));
      piom.Need(kelpie::Key(key_name), &ldo);
      auto mptr = ldo.GetMetaPtr<my_data_t *>();
      cout <<"Retrieved ldo "<<key_name<<" block_id: "<<mptr->block_id<<" name: "<<string(mptr->name)<<" data_bytes: "<<mptr->data_bytes<<endl;
    }
    cout << "Done reading from " << path_name << endl;

  }
  faodel::bootstrap::Finish();
  return 0;

}
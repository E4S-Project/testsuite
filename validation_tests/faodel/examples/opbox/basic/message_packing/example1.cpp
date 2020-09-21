// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <string.h>

#include "opbox/OpBox.hh"
#include "opbox/common/MessageHelpers.hh"

using namespace std;

//Helper function that dumps the header
void example_dumpHeader(opbox::message_t *msg_hdr);

void example1_plain_messages(){

  //Message packing can be a pain in any communication package. Ultimately,
  //you want a convenient way to send information between nodes that takes
  //into account the fact that you need to put a sequential stream of bytes
  //on the wire and make sense of them on the other end. There are dozens of
  //ways to serialize/deserialize (serdes) messages, each with its own
  //tradeoffs in efficiency and user complexity. We've used boost, Cereal,
  //thrift, protocol buffers, XDR, and others, but don't have a go-to library
  //that works for all our scenarios. Sometimes we just want to put a POD
  //struct on the wire as fast as possible. Other times we just want an easy
  //way to serialize a complex c++ structure. In OpBox we don't force you to
  //serdes one way or another- you pick the approach that works best for you.
  //What we do instead is provide you with some helper functions to make it
  //easier to do common things.

  //At the lowest level, opbox works with plain, simple messages that all
  //have the same header. OpBox's message_t provides a pod struct with a header
  //that has common information Ops generally need: the source/destination ids
  //for both the node and mailbpx, n op_id for identifying the traffic this
  //message belongs to, a small user_flags section, and a body section for up
  //to 65535 bytes of data.

  //When you work with a message, you can work directly with it's POD:
  opbox::message_t msg;

  msg.src = opbox::GetMyID();
  msg.dst = opbox::GetMyID(); //Just a pretend value for now
  msg.src_mailbox = opbox::MAILBOX_UNSPECIFIED; //When we don't expect a reply
  msg.dst_mailbox = opbox::MAILBOX_UNSPECIFIED; //When this is a new message
  msg.op_id = 2112;   //Normally this would be the op::op_id value
  msg.user_flags = 1; //Ops almost always need a place to put flags
  msg.body_len = 0;   //No payload in this example

  //We can dump out the message's info
  cout <<"Size of header is "<<sizeof(opbox::message_t)<<" bytes\n"; //32B
  cout <<"Message is "<<msg.str()<<endl;

}


void example1_body_messages(){

  //Most ops need to send more than just a few status bits. OpBox's message_t
  //was designed to allow you to overlay larger message structures on top of
  //it and have it be usable by opbox calls. All you have to do is declare
  //a pod structure and place a message_t as the first item in the structure.
  //This effectively places all the extra sections in the body char array
  //of message_t.
  //
  // Caveat 1: You MUST update the header's body length with the size of
  //           the additional data (eg sizeof(new_message)-sizeof(message_t)
  // Caveat 2: This only works with plain-old-data types (PODs). Anything
  //           like a variable-length array, string, vector, etc, you'll
  //           need to pack yourself (see example1_complex_body_messages())

  struct my_message_t {
    opbox::message_t hdr;
    int      extra_field1;
    int      extra_field2;
    uint64_t big_thing;
    char     my_block1[256];
    char     my_block2[256];
  };

  //Create an empty message
  my_message_t msg;
  memset(&msg, 0, sizeof(my_message_t));

  //Fill it in with data
  msg.hdr.src = opbox::GetMyID();
  msg.hdr.dst = opbox::GetMyID(); //Just a pretend value for now
  msg.hdr.src_mailbox = opbox::MAILBOX_UNSPECIFIED; //When we don't expect a reply
  msg.hdr.dst_mailbox = opbox::MAILBOX_UNSPECIFIED; //When this is a new message
  msg.hdr.op_id = 2112; //Normally this would be the op::op_id value
  msg.hdr.user_flags = 1; //Ops almost always need a place to put flags
  msg.extra_field1=100;
  msg.extra_field2=200;
  msg.big_thing=300;
  strcpy(msg.my_block1, "This is my first block");
  strcpy(msg.my_block2, "This is my second block");

  //Make sure body len is right
  msg.hdr.body_len = sizeof(my_message_t)-sizeof(message_t);


  cout <<"Message is "<<msg.hdr.str() << endl
       <<" field1 is "<<msg.extra_field1<<endl
       <<" block1 is '"<<msg.my_block1<<"'\n"
       <<" block2 is '"<<msg.my_block2<<"'\n";

  //OpBox functions often use *message_t as an argument as a C-style
  //way of managing different data types that start with the message_t
  //header.
  example_dumpHeader(&msg.hdr);

}

void example1_complex_body_messages(){

  //Sometimes an op needs to pack a few variable length fields and we don't
  //want to pay the price of a full-blown serialization library. We can
  //manually create these packed messages with some helper functions.
  //
  //In this example we want to send up to 16 different strings (of varying
  //length) in a message. We define a variable_message_t structure to
  //hold the message header, number of items, and lengths of each item.
  //The problem is that we don't know how big the message is until someone
  //gives us the data we need to put into it.
  //
  //The solution below uses a static Allocate function that allocates space
  //for the message based on user inputs, and then appends all of the strings
  //into the tail end of the message. We remove the default constructor to
  //prevent people from accidentally allocating messages without enough space,
  //and provide a getItem() function to the struct for extracting each
  //string from the packed data.
  //
  // Caveat 1: This level of manual packing is not for the faint of heart,
  //           and there are plenty of ways things can go wrong.
  // Caveat 2: It can be tricky to get deallocation to work correctly if
  //           you manually allocate the struct. If you allocate with something
  //           like reinterpret_cast<variable_message_t>(new char[len]), you
  //           may have trouble with delete vs delete[] in the destructor.
  //           using ::operator new(size) allows you to use the built-in
  //           destructor.

  struct variable_message_t {
    opbox::message_t hdr;
    int num_items;
    int lengths[16];
    char packed_data[0];

    variable_message_t() = delete; //We don't know how big we'll be

    //Walk through our struct and pull out the desired string
    string getItem(int id){
      assert((id>=0)&&(id<num_items));
      int offset=0;
      for(int i=0; i<id; i++)
        offset+=lengths[i];
      return string(&packed_data[offset], lengths[id]);
    }

    //Use a custom allocator to create space for the message
    //and pack the body. This packs the strings in as well
    static variable_message_t * Allocate(vector<string> names){
      assert(names.size()<16);

      //Figure out how big this message will be
      int len=sizeof(variable_message_t);
      for(auto s : names) len+=s.size();

      //Allocate space for this message. The operator new allocation
      //here means we get a pointer that the built-in delete can handle
      auto msg = reinterpret_cast<variable_message_t *>(::operator new(len));
      msg->num_items = names.size();

      //Append the strings to the packed_data section
      int offset=0;
      for(int i=0; i<names.size(); i++){
        msg->lengths[i]=names[i].size();
        names[i].copy(&msg->packed_data[offset], names[i].size());
        offset+=names[i].size();
      }
      //Remember to set the body length!
      msg->hdr.body_len = len-sizeof(message_t);
      return msg;
    }
  };

  //Allocate the message and fill in the strings
  auto msg = variable_message_t::Allocate( {"frank","bob","joey","edna"});

  //Update the rest of the fields in the message, as normal
  msg->hdr.src = opbox::GetMyID();
  msg->hdr.dst = opbox::GetMyID(); //Just a pretend value for now
  msg->hdr.src_mailbox = opbox::MAILBOX_UNSPECIFIED; //When we don't expect a reply
  msg->hdr.dst_mailbox = opbox::MAILBOX_UNSPECIFIED; //When this is a new message
  msg->hdr.op_id = 2112; //Normally this would be the op::op_id value
  msg->hdr.user_flags = 1; //Ops almost always need a place to put flags

  //Use the getItem() operation to pull out individual items
  for(int i=0; i<msg->num_items; i++)
    cout <<"Item "<<i<<" is length "<<msg->lengths[i]<<", value '"<<msg->getItem(i)<<"'\n";

  delete msg;

}

//This function demonstrates the C-style way of dealing with multiple
//messge structures.
void example_dumpHeader(opbox::message_t *msg_hdr){
  cout <<"Message header is "<<msg_hdr->str() <<endl;
}

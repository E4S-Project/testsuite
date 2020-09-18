// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "faodel-common/Common.hh"

using namespace std;
using namespace faodel;

void example_resourceurl() {

  cout <<"ResourceURL Examples\n";

  //A ResourceURL in FAODEL provides a way for us to reference
  //different resources in the system. A ResourceURL string uses
  //the following format
  //
  // type:<node>[bucket]/my/path/name&myop1=foo&myop2=bar
  //
  // The individual components in this string are:
  //
  // Type:  A short identifier for what this resource does in the
  //        system. It can generically be "ref" to denote this as
  //        a reference to a resource (which underlying code should
  //        lookup) or a standard type (eg, dht: rft:)
  //
  // Node:  A NodeID that is associated with the resource. eg, This may
  //        be the root node for a collection of nodes in a resource.
  //
  // Bucket: A hash of a string to provide some namespace isolation
  //        in a shared environment. The default bucket (specified in
  //        the Configuration used to start FAODEL) will be used if this
  //        field is not in the string.
  //
  // Path:  A '/' separated path for naming this resource in the global
  //        hierarchy. Higher-level services like DirMan use this hierarchy
  //        to organize related items.
  //
  // Name:  The name for the resource (last level in path). eg "mydht"
  //
  // Options: A string of additional "&key=value" options.  If keys are
  //        duplicated, the last one overwrites. Spaces often do not
  //        work correctly.



  //A Resource URL parses the string you give it at construction time:
  string url_string = "dht:<0x02>[this_is_my_secret]/a/b/c/d/e/f/item&my_setting=off";
  ResourceURL u1(url_string);

  cout << "URL1 "<<url_string<<" is"
          << "\n Type:   "<<u1.Type()
          << "\n Bucket: "<<u1.bucket.GetHex() //Buckets get converted to 32b hashes immediately
          << "\n Path:   "<<u1.path
          << "\n Name:   "<<u1.name
          << "\n option 'my_setting':      '"<<u1.GetOption("my_setting")<<"'"       //Search for an option
          << "\n option 'MY_SETTING':      '"<<u1.GetOption("MY_SETTING")<<"'"       //Settings are case-specific
          << "\n option 'missing_setting': '"<<u1.GetOption("missing_setting")<<"'"
          << endl;


  //You can manipulate a url directly
  u1.name = "other_item";
  cout <<"Modified name is "<<u1.GetFullURL()<<endl;

  //You can also modify, append, or remove options
  u1.SetOption("Option2","mushroom");
  u1.SetOption("Option3","piano");
  u1.SetOption("Option1","pizza");
  u1.RemoveOption("my_setting");
  cout <<"Modified name is "<<u1.GetFullURL()<<endl;


  //Often we need to translate a resource name (eg, /my/pool) into something that
  //defines all its characteristics. If you don't supply a resource type, FAODEL
  //assumes you're making a reference to something that will need to be looked up.
  ResourceURL u2("/my/pool");
  cout<<"I asked for '/my/pool'. Type is '"<<u2.Type()<<"'. Full Url: "<<u2.GetFullURL()<<endl;




  //Some services just manipulate the directory structure. Here we work
  //our way up to the top of the tree.
  ResourceURL utmp = u1;
  while(!utmp.IsRootLevel()){
    cout <<"Currently at "<<utmp.GetPathName()<<endl;
    utmp=utmp.GetParent();
  }
  cout <<"Root level is "<<utmp.GetPathName()<<"   path="<<utmp.path<<" name="<<utmp.name<<endl;

}

// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <string>
#include <memory>
#include <cstring>
#include <iostream>
#include <sstream>

#include "opbox/OpBox.hh"

// What kind of archive will we use?
#include <cereal/archives/binary.hpp>
// What kind of types will we use in our classes that we want to serialize?
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

using namespace std;

namespace cereal_example {
  
  class Pocket
  {
  public:
    Pocket( bool is_left = false, bool is_front = false )
      : is_left_( is_left ), is_front_( is_front )
    {};

    bool is_left_, is_front_;

    /*
     * Here's the magic for Cereal, and it looks a lot like Boost. Syntax is a little different 
     * but as long as you define a serialize method in your class, you're good to go.
     *
     * Cereal understands built-in types and STL containers out-of-the-box (see above for the include
     * statements). You have to do a little more work if your classes are more complicated.
     */
    
    template< typename Archive >
    void serialize( Archive& ar )
    {
      ar( is_left_, is_front_ );
    }
  
  };

  class FancyPants
  {
  public:

    FancyPants( const string& name = "", const string& color = "", int num_pockets = 0 )
      : name_( name ), color_( color )
    {
      for( auto i = 0; i < num_pockets; i++ )
	pockets_.push_back( Pocket ( (i & 0x01), (i & 0x02) ) );
    };

    string name_, color_;
    vector< Pocket > pockets_;

    /*
     *  Notice here how we can just pass the pockets_ vector directly to the serializer.
     *  This is because Cereal understands std::vector and Pocket has a serialize() method.
     */
    template< typename Archive >
    void serialize( Archive& ar )
    {
      ar( name_, color_ );
      ar( pockets_ );
    }
  
  };


  ostream&
  operator<<( ostream& ostr, const FancyPants& fp )
  {
    ostr << fp.color_ << ":" << fp.name_ << endl;

    for( auto &&p : fp.pockets_ ) {
      ostr << ((p.is_front_)?"front-":"back-")
	   << ((p.is_left_)?"left":"right") << " pocket" << endl;
    }
    return ostr;
  }

};

void
example7_cereal_messages()
{

  using namespace cereal_example;
  
  cout << endl
       << "*********************************"
       << " Cereal serialization "
       << "*********************************"
       << endl;
  
  lunasa::DataObject *ldo;

  FancyPants my_pants( "britches", "red", 4 );

  stringstream ss;

  /*
   * Really the only strangeness about Cereal. You're only guaranteed to have your data
   * flushed when the Archive is destroyed (RAII semantics). So they recommend you open
   * a separate scope for your Archive objects and let them go out of scope 
   * to make sure you get all your data out or in.
   */
  {
    cereal::BinaryOutputArchive oarchive( ss );
    oarchive( my_pants );
  }
  string my_cerealized_pants = ss.str();
  
  // Some opbox/lunasa boilerplate to set up an LDO with a message body of an
  // arbitrary struct. Perhaps there's a better way to do this but I couldn't
  // figure it out.
  ldo = opbox::net::NewMessage( sizeof( message_t ) + my_cerealized_pants.size() );
  message_t *msg = reinterpret_cast< message_t* >( ldo->dataPtr() );

  // Use the size of the marshaled buffer in the FB builder as the length of the payload
  msg->body_len = my_cerealized_pants.size();
  // Copy the FB builder's marshaled buffer to the LDO 
  std::memcpy( msg->body, my_cerealized_pants.data(), msg->body_len );

  cout << "Cerealized buffer size is " << msg->body_len << endl;
  cout << "LDO size is " << ldo->dataSize() << endl;

  // Get the message back out of the LDO, as though we'd received it from a sender
  auto *msg2 = reinterpret_cast< message_t* >( ldo->dataPtr() );

  string more_cerealized_pants( msg->body, msg->body_len );

  stringstream iss;
  iss.str( more_cerealized_pants );

  FancyPants my_other_pants;
  
  {  
    cereal::BinaryInputArchive iarchive( iss );
    iarchive( my_other_pants );
  }

    // All done. Let's see what happened.
  cout << "Original pants:" << endl << my_pants << endl;  
  cout << "unpacked pants:" << endl << my_other_pants;

  delete ldo;
}

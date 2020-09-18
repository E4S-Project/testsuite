@0xcd50b9d7edb13daf;

struct PocketCp {
  isLeft @0 : Bool;
  isFront @1 : Bool;
}

struct FancyPantsCp {
  name @0 : Text;
  color @1 : Text;
  pockets @2 : List( PocketCp );
}


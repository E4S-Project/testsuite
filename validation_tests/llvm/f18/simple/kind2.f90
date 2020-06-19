!! this one is expected to fail

program kind2

  !! unsupported
  REAL( KIND = 32 ) :: thirtytwo
  REAL( KIND = 64 ) :: sixtyfour
  INTEGER           :: i32, i64

  i32 = SIZEOF( thirtytwo )
  i64 = SIZEOF( sixtyfour )
  
  WRITE( 6, * ) i32, i64


end program

program kind

  REAL( KIND = 4 )  :: four
  REAL( KIND = 8 )  :: eight
  REAL( KIND = 16 ) :: sixteen
  INTEGER           :: i4, i8, i16

  i4  = SIZEOF( four )
  i8  = SIZEOF( eight )
  i16 = SIZEOF( sixteen )

  IF( i4 == 4 .AND. i8 == 8 .AND. i16 == 16 ) THEN
     call EXIT( 0 )
  ELSE
     call EXIT( 1 )
  END IF

end program

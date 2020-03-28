PROGRAM CALLREC
  IMPLICIT NONE

  INTERFACE
     recursive function fact( i ) 
       INTEGER, INTENT( IN )  :: i
     end function fact
  END INTERFACE
  
  INTEGER :: NB, RES
  NB = 8
  RES = fact( NB )

END PROGRAM

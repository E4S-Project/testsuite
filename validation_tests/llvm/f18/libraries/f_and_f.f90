PROGRAM MAIN
  IMPLICIT NONE

  INTERFACE
     FUNCTION addfloat( A, B )
       REAL*4, intent(in) :: A, B
       REAL*4 :: addfloat
     END FUNCTION addfloat

     FUNCTION adddouble( A, B )
       DOUBLE PRECISION, intent(in) :: A, B
       DOUBLE PRECISION :: adddouble
     END FUNCTION adddouble
  END INTERFACE
  
  REAL*4 :: G, H, I
  DOUBLE PRECISION :: E, D, F

  D = 5.789
  E = 8.432
  G = 5.789
  H = 8.432

  F = ADDDOUBLE( D, E )
  I = ADDFLOAT( G, H )

  WRITE (6,'(F0.4,T10)') F
  WRITE (6,'(F0.4,T10)') I

END PROGRAM MAIN

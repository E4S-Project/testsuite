PROGRAM MAIN
  IMPLICIT NONE

  INTERFACE
     FUNCTION addint2( A, B )
       INTEGER, intent(in) :: A, B
       INTEGER :: addint2
     END FUNCTION addint2

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
  INTEGER :: A, B, C

  A = 16
  B = 42
  D = 5.789
  E = 8.432
  G = 5.789
  H = 8.432

  C = ADDINT2( A, B )
  F = ADDDOUBLE( D, E )
  I = ADDFLOAT( G, H )

  WRITE (6,'(I0,T10)') C
  WRITE (6,'(F0.4,T10)') F
  WRITE (6,'(F0.4,T10)') I



END PROGRAM MAIN

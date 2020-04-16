program main
  use omp_lib

  implicit none

  integer ( kind = 4 ) id
  real ( kind = 8 ) wtime

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'HELLO_OPENMP'
  write ( *, '(a)' ) '  FORTRAN90/OpenMP version'

  wtime = omp_get_wtime ( )

  write ( *, '(a,i8)' ) &
    '  The number of processors available = ', omp_get_num_procs ( )
  write ( *, '(a,i8)' ) &
    '  The number of threads available    = ', omp_get_max_threads ( )
  id = omp_get_thread_num ( )

  write ( *, '(a,i8,a,i8)' ) '  HELLO from process ', id

!$omp parallel &
!$omp private ( id )
  id = omp_get_thread_num ( )

  write ( *, '(a,i8,a,i8)' ) '  HELLO from process ', id

!$omp end parallel
  stop
end

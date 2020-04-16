program omp_par_do
  implicit none

  integer, parameter :: n = 100
  real, dimension(n) :: dat, result
  integer :: i

  !$OMP PARALLEL DO
  do i = 1, n
     result(i) = my_function(dat(i))
  end do
  !$OMP END PARALLEL DO

contains

  function my_function(d) result(y)
    real, intent(in) :: d
    real :: y

    ! do something complex with data to calculate y
  end function my_function

end program omp_par_do


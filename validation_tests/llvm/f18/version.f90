program version
  implicit none

#ifdef __flang__
  print *, "MAJOR", __flang_major__
  print *, "MINOR", __flang_minor__
  print *, "PATCH", __flang_patchlevel__
!  print *, "VERSION", __flang_version__
#endif

end program version

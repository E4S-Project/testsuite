recursive function fact(i) result(j)
    integer, intent(in) :: i
    integer :: j
    if (i == 1) then
        j = 1
    else
        j = i * fact(i - 1)
    end if
end function fact

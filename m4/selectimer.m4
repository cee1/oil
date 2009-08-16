AC_DEFUN([OIL_CHECK_CLOCK_GETTIME],
[
    AC_MSG_CHECKING([whether support clock_gettime with $1])
    AC_LANG(C)
    AC_LANG_CONFTEST(
    [AC_LANG_PROGRAM([[#include <time.h>]], [[
    struct timespec ts;
    int ret = clock_gettime ($1, &ts);
    return ret;]])])
    
    $CC conftest.c $CFALGS -lrt -o conftest
    chmod +x conftest && ./conftest
    
    if test $? -eq 0 ; then
        ok='yes'
        $2
        true
    else
        ok='no'
        $3
        true
    fi
    AC_MSG_RESULT([$ok])
])


AC_DEFUN([OIL_CHECK_PLATFORM],
[
    AC_MSG_CHECKING([platform])
    result="unknown"

    # detect the platform
    case "x${host_cpu}" in
        xi?86 | k?)
            HAVE_I386=yes
            result="i386"
            ;;
        xmips*)
        	result="mips-unknown"
        	AC_LANG(C)
            AC_LANG_CONFTEST(
                [AC_LANG_PROGRAM([[#include <asm/sgidefs.h>]], [[
#if _MIPS_SIM == _MIPS_SIM_ABI32
    return 1;
#elif _MIPS_SIM_NABI32 == _MIPS_SIM_NABI32
    return 2;
#elif _MIPS_SIM_ABI64 == _MIPS_SIM_ABI64
    return 3;
#endif]])])
            $CC conftest.c $CFALGS -o conftest
            chmod +x conftest && ./conftest
            if test $? -eq 1 ; then
                HAVE_MIPS_O32=yes
                result="mips-o32"
            elif test $? -eq 2 ; then
                HAVE_MIPS_N32=yes
                result="mips-n32"
            elif test $? -eq 3 ; then
                HAVE_MIPS_N64=yes
                result="mips-n64"
            fi
        	;;
    esac
    AC_MSG_RESULT([$result])
    
    AM_CONDITIONAL(HAVE_I386, test "x$HAVE_I386" = "xyes")
    AM_CONDITIONAL(HAVE_MIPS_O32, test "x$HAVE_MIPS_O32" = "xyes")
    AM_CONDITIONAL(HAVE_MIPS_N32, test "x$HAVE_MIPS_N32" = "xyes")
    AM_CONDITIONAL(HAVE_MIPS_N64, test "x$HAVE_MIPS_N64" = "xyes")
])


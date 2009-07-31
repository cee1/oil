AC_PREREQ([2.59])

m4_define([oil_major_version], [0])
m4_define([oil_minor_version], [1])
m4_define([oil_micro_version], [0])
m4_define([oil_version],
        [oil_major_version.oil_minor_version.oil_micro_version])

AC_INIT([oil], [oil_version], [chenj@lemote.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])

OIL_MAJOR_VERSION=oil_major_version
OIL_MINOR_VERSION=oil_minor_version
OIL_MICRO_VERSION=oil_micro_version
OIL_VERSION=oil_version
AC_SUBST(OIL_MAJOR_VERSION)
AC_SUBST(OIL_MINOR_VERSION)
AC_SUBST(OIL_MICRO_VERSION)
AC_SUBST(OIL_VERSION)

AC_CONFIG_SRCDIR([oilcore/oilcore.h])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
CFLAGS="-O0 -g"

AM_PROG_AS
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIBM
AC_SUBST(LIBM)

LIBRT=-lrt
AC_SUBST(LIBRT)

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime memset sqrt strchr strdup])

PKG_CHECK_MODULES(GLIB, glib-2.0, HAVE_GLIB=yes, HAVE_GLIB=no)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_ARG_ENABLE(oiltest,
AC_HELP_STRING([--disable-oiltest],[disable oiltest]),
[case "${enableval}" in
    yes) HAVE_GLIB=yes ;;
    no) HAVE_GLIB=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --disable-oiltest) ;;
esac])
AM_CONDITIONAL(HAVE_GLIB, test "x$HAVE_GLIB" = "xyes")

HAVE_OILFUNCS=yes
AC_ARG_ENABLE(oilfuncs,
AC_HELP_STRING([--disable-oilfuncs],[disable pre-implemented oil functions]),
[case "${enableval}" in
    yes) HAVE_OILFUNCS=yes ;;
    no)  HAVE_OILFUNCS=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --disable-oilfuncs) ;;
esac])
AM_CONDITIONAL(HAVE_OILFUNCS, test "x$HAVE_OILFUNCS" = "xyes")

AC_MSG_CHECKING([check for cpu family])
result="unknown"
case "x${host_cpu}" in
    xi?86 | k?)
        HAVE_I386=yes
        AC_DEFINE(HAVE_I386, 1, [Defined if host is i386])
        result="i386"
        ;;
    xmips*)
    	HAVE_MIPS=yes
    	AC_DEFINE(HAVE_MIPS, 1, [Defined if host is mips])
    	result="mips"
    	;;
esac
AC_MSG_RESULT([$result])

AM_CONDITIONAL(HAVE_I386, test "x$HAVE_I386" = "xyes")
AM_CONDITIONAL(HAVE_MIPS, test "x$HAVE_MIPS" = "xyes")
GTK_DOC_CHECK([1.11])

pkgconfigdir="\$(libdir)/pkgconfig"
AC_SUBST(pkgconfigdir)

AC_CONFIG_FILES([oilcore.pc
                oilcore-uninstalled.pc
                oiltest.pc
                oiltest-uninstalled.pc
                oilfuncs.pc
                oilfuncs-uninstalled.pc
                Makefile
                oilcore/Makefile
                oilcore/tests/Makefile
                oiltest/Makefile
                oiltest/tests/Makefile
                docs/Makefile
                docs/oilcore/Makefile
                docs/oiltest/Makefile
                docs/oilfuncs/Makefile
                oilfuncs/Makefile
                oilfuncs/ref/Makefile
                oilfuncs/i386/Makefile
                oilfuncs/i386/mmx/Makefile
                oilfuncs/mips/Makefile
                oilfuncs/mips/gs2f/Makefile
                oilfuncs/tests/Makefile],
                [rm -f ${ac_top_srcdir}/oilfuncs/oilfuncs.reg])
AC_OUTPUT


EXTRA_DIST = classes.def
check_PROGRAMS = testoiltest
TESTS = testoiltest

testoiltest_SOURCES =   \
    testoiltest.c       \
    testoilparam.c      \
    testoiltestconfig.c

testoiltest_CPPFLAGS =      \
    $(GLIB_CFLAGS)          \
    -I$(top_srcdir)/oilcore \
    -I$(top_srcdir)/oiltest

testoiltest_LDADD =                                                                 \
    $(GLIB_LIBS)                                                                    \
    $(top_builddir)/oilcore/liboilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la   \
    ../liboiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la

testoiltest_LDFLAGS = -static

CLEANFILES = testoiltest

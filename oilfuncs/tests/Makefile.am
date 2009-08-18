check_PROGRAMS = simple full
TESTS = simple full

simple_SOURCES = simple.c
simple_CPPFLAGS =           \
    -I$(top_srcdir)/oilcore \
    -I..
simple_LDADD =                                                                      \
    $(top_builddir)/oilcore/liboilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la   \
    ../liboilfuncs-test-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la
simple_LDFLAGS = -static

full_SOURCES = full.c
full_CPPFLAGS =             \
    $(GLIB_CFLAGS)          \
    -I$(top_srcdir)/oilcore \
    -I$(top_srcdir)/oiltest \
    -I..
full_LDADD =                                                                        \
    $(GLIB_LIBS)                                                                    \
    $(top_builddir)/oilcore/liboilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la   \
    $(top_builddir)/oiltest/liboiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la   \
    ../liboilfuncs-test-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la
full_LDFLAGS = -static  


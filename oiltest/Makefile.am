lib_LTLIBRARIES = liboiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la
liboiltest_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_SOURCES = \
    oilutil.c                                                   \
    oilutil.h                                                   \
    oiltestconfig.c                                             \
    oiltestconfig.h                                             \
    oilparameter.c                                              \
    oilparameter.h                                              \
    oilchecker.c                                                \
    oilchecker.h                                                \
    oilprofiler.c                                               \
    oilprofiler.h                                               \
    oiltester.c                                                 \
    oiltester.h                                                 \
    oiltest.h

liboiltest_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_CPPFLAGS = \
    $(GLIB_CFLAGS)                                               \
    -I$(top_srcdir)/oilcore                                      \
    -I$(top_srcdir)/oiltest                                      \
    -DG_LOG_DOMAIN=\"OilTest\"

liboiltest_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_LIBADD =  \
    $(LIBM)                                                     \
    $(LIBRT)                                                    \
    $(GLIB_LIBS)                                                \
    $(top_builddir)/oilcore/liboilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la

oilincludedir = $(includedir)/oil
oilinclude_HEADERS =        \
    oiltest.h               \
    oilutil.h               \
    oilparameter.h          \
    oiltestconfig.h         \
    oilchecker.h            \
    oilprofiler.h           \
    oiltester.h

bin_SCRIPTS = oil-regclasses oil-regimpls

SUBDIRS = tests


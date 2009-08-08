SUBDIRS = ref tests

lib_LTLIBRARIES = liboilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la liboilfuncs-test-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la

liboilfuncs_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_SOURCES = oilfuncs-register-classes.c oilfuncs-register-implements.c oilfuncs-slots.c oilfuncs-slots.h
liboilfuncs_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_LIBADD =                     \
    $(top_builddir)/oilcore/liboilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la   \
    ref/liboilref.la
liboilfuncs_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_CPPFLAGS = -I$(top_srcdir)/oilcore

liboilfuncs_test_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_SOURCES = oilfuncs-attach-classes.c oilfuncs-marshal.c oilfuncs-marshal.h
liboilfuncs_test_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_LIBADD =    \
    $(GLIB_LIBS)                                                        \
    $(top_builddir)/oiltest/liboiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.la
liboilfuncs_test_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_CPPFLAGS =  \
    $(GLIB_CFLAGS)                                                      \
    -I$(top_srcdir)/oilcore                                             \
    -I$(top_srcdir)/oiltest

domain = oilfuncs
EXTRA_DATA = $(domain).def
CLEANFILES = $(domain).reg
impl_reglist = "[include]\n"

if HAVE_I386
SUBDIRS += i386
liboilfuncs_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_LIBADD += i386/liboili386.la
impl_reglist += "i386/i386.reg\n"
endif

if HAVE_MIPS
SUBDIRS += mips
liboilfuncs_@OIL_MAJOR_VERSION@_@OIL_MINOR_VERSION@_la_LIBADD += mips/liboilmips.la
impl_reglist += "mips/mips.reg\n"
endif

oilincludedir = $(includedir)/oil
oilinclude_HEADERS = oilfuncs.h oilfuncs-slots.h

REG_CLASSES = $(top_srcdir)/oiltest/oil-regclasses
$(domain)-slots.c $(domain)-slots.h $(domain)-register-classes.c $(domain)-attach-classes.c $(domain)-marshal.c $(domain)-marshal.h: $(domain).def; python $(REG_CLASSES) -f $(domain).def

REG_IMPLEMENTS = $(top_srcdir)/oiltest/oil-regimpls
$(domain)-register-implements.c: $(domain).reg; python $(REG_IMPLEMENTS) -f $(domain).reg

$(domain).reg:; echo -e $(impl_reglist) > $(domain).reg


ACLOCAL_AMFLAGS = -I m4
SUBDIRS = oilcore

oilincludedir = $(includedir)/oil
oilinclude_HEADERS = oilconfig.h

if HAVE_GLIB
SUBDIRS += oiltest
endif

if HAVE_OILFUNCS
SUBDIRS += oilfuncs
endif

if ENABLE_GTK_DOC
SUBDIRS += docs
endif

EXTRA_DIST =                    \
    autogen.sh                  \
    COPYING README              \
    ChangeLog                   \
    oilcore-uninstalled.pc      \
    oiltest-uninstalled.pc      \
    oilfuncs-uninstalled.pc

pkgconfig_DATA =                                        \
    oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc  \
    oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc  \
    oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc

oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc: oilcore.pc
	cp oilcore.pc oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc

oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc: oilcore-uninstalled.pc
	cp oilcore-uninstalled.pc oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc

oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc: oiltest.pc
	cp oiltest.pc oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc

oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc: oiltest-uninstalled.pc
	cp oiltest-uninstalled.pc oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc

oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc: oilfuncs.pc
	cp oilfuncs.pc oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc

oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc: oilfuncs-uninstalled.pc
	cp oilfuncs-uninstalled.pc oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc

CLEANFILES =                                                            \
    oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc                  \
    oilcore-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc      \
    oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc                  \
    oiltest-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc      \
    oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@.pc                 \
    oilfuncs-@OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@-uninstalled.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc


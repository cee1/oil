AUTOMAKE_OPTIONS = 1.6
DOC_MODULE = oilcore
DOC_MODULE_VERSION = @OIL_MAJOR_VERSION@.@OIL_MINOR_VERSION@

DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.sgml

DOC_SOURCE_DIR = ../../oilcore

# Extra options to pass to gtkdoc-scangobj. Not normally needed.
SCANGOBJ_OPTIONS =

# SCAN_OPTIONS = --deprecated-guards="OIL_DISABLE_DEPRECATED" --ignore-decorators="_"
SCAN_OPTIONS =

MKDB_OPTIONS = --sgml-mode --output-format=xml --name-space=oil --ignore-files="hashtable.c hastable.h"

# Extra options to supply to gtkdoc-mktmpl
# e.g. MKTMPL_OPTIONS=--only-section-tmpl
MKTMPL_OPTIONS =

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS =

# Extra options to supply to gtkdoc-fixref. Not normally needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS =

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB = $(top_srcdir)/oilcore/*.h
CFILE_GLOB = $(top_srcdir)/oilcore/*.c

# Header files to ignore when scanning.
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h
IGNORE_HFILES = hashtable.h test_common.h

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES = oilclasses.png

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.sgml building.sgml changes-2.0.sgml
content_files =

# SGML files where gtk-doc abbrevations (#GtkWidget) are expanded
# These files must be listed here *and* in content_files
# e.g. expand_content_files=running.sgml
expand_content_files =

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS =
GTKDOC_LIBS =

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST +=

# Files not to distribute
# for --rebuild-types in $(SCAN_OPTIONS), e.g. $(DOC_MODULE).types
# for --rebuild-sections in $(SCAN_OPTIONS) e.g. $(DOC_MODULE)-sections.txt
#DISTCLEANFILES +=

# Comment this out if you want your docs-status tested during 'make check'
#TESTS = $(GTKDOC_CHECK)



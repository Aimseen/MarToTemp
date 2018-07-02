
ACLOCAL_AMFLAGS = -I m4

SUBDIRS= \
	libltdl \
	gtest \
	src \
	tests \
	doc

noinst_SCRIPTS=run-example

EXTRA_FILES=NEWS run-example.in

edit = sed \
	-e 's|@LIBTOOL[@]|$(LIBTOOL)|g' \
	-e 's|@abs_top_builddir[@]|$(abs_top_builddir)|g' \
	-e 's|@prefix[@]|$(prefix)|g'

run-example: Makefile
	rm -f $@ $@.tmp
	srcdir=''; \
	  test -f ./$@.in || srcdir=$(srcdir)/; \
	  $(edit) $${srcdir}$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

run-example: run-example.in

reindent:
	true ## indent really breaks some C++ constructions :-(
	true #indent -kr -brf -l100 -lc100 -nut \
	true #	$(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	true #astyle $(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i \
		$(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h \
		$(srcdir)/src/*/*/*.h

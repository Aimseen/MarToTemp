
ACLOCAL_AMFLAGS = -I m4

SUBDIRS= \
	libltdl \
	gtest \
	src \
	tests \
	doc

noinst_SCRIPTS=run-example

EXTRA_DIST=NEWS run-example.in
CLEANFILES=run-example

# Workaround a libtool bug which can left this file after a 'make distclean'
DISTCLEANFILES=libltdl/.deps/lt__strl.Plo

edit = sed \
	-e 's|@LIBTOOL[@]|$(LIBTOOL)|g' \
	-e 's|@abs_top_builddir[@]|$(abs_top_builddir)|g' \
	-e 's|@libdir[@]|$(libdir)|g' \
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

.PHONY: reindent
reindent:
	true ## indent really breaks some C++ constructions :-(
	true #indent -kr -brf -l100 -lc100 -nut \
	true #	$(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	true #astyle $(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i \
		$$(find $(srcdir)/src -iname RngStream.h -prune -o \( -iname "*.h" -o -iname "*.cpp" \) -print)

.PHONY: update-forward-decl-header
update-forward-decl-header: reindent
	for f in $$(ls $(srcdir)/src/core/marto/*.h | sort | grep -v forwardDecl.h | grep -v -- '-impl.h' ) ; do \
		echo ; \
		echo "/* $$(basename $$f) */"; \
		grep '^\(template .*\)\?class.*\|^template.*>$$' $$f | \
			grep -v 'no-forward-decl' | \
			sed -e 's,^\(\(template .*\)\?class [^ {]*\)[ {].*,\1;,' \
				-e 's/^class \([^ ]*\)::\([^:]*\)/namespace \1 { class \2 }/' ; \
	done > $@
	lead='^[/][/] Start of auto-generated part' ; \
	tail='^[/][/] End of auto-generated part' ; \
	nl="$$(echo -e '\n ')" ; \
	sed -e "/$$lead/,/$$tail/{ /$$lead/{p; r $@$$nl}; /$$tail/s/^/\n/p; d }" -i $(srcdir)/src/core/marto/forwardDecl.h

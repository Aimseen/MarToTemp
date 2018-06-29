
ACLOCAL_AMFLAGS = -I m4

SUBDIRS= \
	libltdl \
	gtest \
	src \
	tests \
	doc

EXTRA_FILES=NEWS

reindent:
	true ## indent really breaks some C++ constructions :-(
	true #indent -kr -brf -l100 -lc100 -nut \
	true #	$(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	true #astyle $(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i \
		$(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h \
		$(srcdir)/src/*/*/*.h

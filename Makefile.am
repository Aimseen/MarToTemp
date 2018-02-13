
ACLOCAL_AMFLAGS = -I m4

SUBDIRS= \
	gtest \
	src \
	tests \
	doc

EXTRA_FILES=NEWS

reindent:
	true ## indent really breaks some C++ constructions :-(
	true #indent -kr -brf -l100 -lc100 -nut \
	true #	$(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h
	astyle $(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h

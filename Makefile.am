
ACLOCAL_AMFLAGS = -I m4

SUBDIRS= \
	src \
	tests \
	doc

EXTRA_FILES=NEWS

reindent:
	astyle $(srcdir)/src/*/*.cpp $(srcdir)/src/*/*.h $(srcdir)/src/*/*/*.h

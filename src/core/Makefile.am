
include_HEADERS=marto.h

nobase_include_HEADERS=marto/state.h marto/transition.h

lib_LTLIBRARIES= libmarto.la

check_PROGRAMS=test_core

libmarto_la_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) -D_GNU_SOURCE $(MARTOLIB_BUILD_CPPFLAGS) \
  -I$(srcdir)
libmarto_la_CFLAGS=@AM_CFLAGS@ $(CFLAGS) $(MARTOLIB_BUILD_CFLAGS)
libmarto_la_LDFLAGS = -version-info 0:0:0 $(MARTOLIB_BUILD_LDFLAGS)
libmarto_la_LIBADD = $(MARTOLIB_BUILD_LIBS)

libmarto_la_SOURCES=\
	state.cpp \
	transition.cpp

test_core_SOURCES=test_core.cpp
test_core_LDADD=libmarto.la


include_HEADERS=psi4.h

nobase_include_HEADERS=psi4/state.h psi4/transition.h

lib_LTLIBRARIES= libpsi4.la

libpsi4_la_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) -D_GNU_SOURCE $(PSI4LIB_BUILD_CPPFLAGS) \
  -I$(srcdir)
libpsi4_la_CFLAGS=@AM_CFLAGS@ $(CFLAGS) $(PSI4LIB_BUILD_CFLAGS)
libpsi4_la_LDFLAGS = -version-info 0:0:0 $(PSI4LIB_BUILD_LDFLAGS)
libpsi4_la_LIBADD = $(PSI4LIB_BUILD_LIBS)

libpsi4_la_SOURCES=\
	state.cpp \
	transition.cpp

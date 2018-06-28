
lib_LTLIBRARIES= libmarto-transition.la

libmarto_transition_la_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) -D_GNU_SOURCE $(TRANSITIONLIB_BUILD_CPPFLAGS) \
  -I$(srcdir) \
  -I$(srcdir)/../core
libmarto_transition_la_CFLAGS=@AM_CFLAGS@ $(CFLAGS) $(TRANSITIONLIB_BUILD_CFLAGS)
libmarto_transition_la_LDFLAGS = -version-info 0:0:0 $(TRANSITIONLIB_BUILD_LDFLAGS)
libmarto_transition_la_LIBADD = $(TRANSITIONLIB_BUILD_LIBS) -ldl

libmarto_transition_la_SOURCES=\
	libmarto-transition.cpp

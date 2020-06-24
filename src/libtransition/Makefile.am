
pkglib_LTLIBRARIES= std_transitions.la

std_transitions_la_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) -D_GNU_SOURCE $(TRANSITIONLIB_BUILD_CPPFLAGS) \
  -I$(srcdir) \
  -I$(srcdir)/../core
std_transitions_la_CFLAGS=@AM_CFLAGS@ $(CFLAGS) $(TRANSITIONLIB_BUILD_CFLAGS)
std_transitions_la_LDFLAGS = $(TRANSITIONLIB_BUILD_LDFLAGS) \
	-avoid-version -module -shared -export-dynamic
#	-export-symbols $(srcdir)/plugins.syms
std_transitions_la_LIBADD = $(TRANSITIONLIB_BUILD_LIBS) -ldl

std_transitions_la_SOURCES=\
	arrivalReject.cpp \
	departure.cpp \
	jsq2.cpp \
	routingNQueuesReject.cpp \
	std_transitions.cpp \
	std_transitions.h


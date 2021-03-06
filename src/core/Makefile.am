
include_HEADERS=marto.h

nobase_include_HEADERS=\
      marto/allocator.h \
      marto/allocator-impl.h \
      marto/event.h \
      marto/event-impl.h \
      marto/except.h \
      marto/forwardDecl.h \
      marto/global.h \
      marto/history.h \
      marto/historyStream.h \
      marto/historyStream-impl.h \
      marto/macros.h \
      marto/parameters.h \
      marto/parameters-impl.h \
      marto/queue.h \
      marto/queue-impl.h \
      marto/random.h \
      marto/randomDeterministic.h \
      marto/randomLecuyer.h \
      marto/state.h \
      marto/transition.h \
      marto/types.h

lib_LTLIBRARIES= libmarto.la

check_PROGRAMS=test_core test_setup test_history test_parameters #test_transition
check_PROGRAMS+=test_random test_simple-forward

libmarto_la_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) -D_GNU_SOURCE $(MARTOLIB_BUILD_CPPFLAGS) \
  -I$(srcdir) \
  -DPKGLIBDIR=\"$(pkglibdir)\"
libmarto_la_CFLAGS=@AM_CFLAGS@ $(CFLAGS) $(MARTOLIB_BUILD_CFLAGS)
libmarto_la_LDFLAGS = -version-info 0:0:0 $(MARTOLIB_BUILD_LDFLAGS)
libmarto_la_LIBADD = $(MARTOLIB_BUILD_LIBS) -dlopen ../libtransition/std_transitions.la
libmarto_la_DEPENDENCIES = $(LTDLDEPS)

libmarto_la_SOURCES=\
	state.cpp \
	transition.cpp \
	event.cpp \
	global.cpp \
	history.cpp \
	parameters.cpp \
	queue.cpp \
        RngStream.h \
	randomDeterministic.cpp \
	randomLecuyer.cpp

# directly included from randomLecuyer.cpp
EXTRA_libmarto_la_SOURCES=RngStream.c

pkgconfig_DATA=marto.pc

test_core_SOURCES=test_core.cpp
test_core_LDADD=libmarto.la

test_setup_SOURCES=test_setup.cpp
test_setup_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) $(GTESTPROGS_BUILD_CPPFLAGS)
test_setup_LDADD=libmarto.la $(GTESTPROGS_BUILD_LDFLAGS)

test_history_SOURCES=test_history.cpp
test_history_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) $(GTESTPROGS_BUILD_CPPFLAGS)
test_history_LDADD=libmarto.la $(GTESTPROGS_BUILD_LDFLAGS)

test_parameters_SOURCES=test_parameters.cpp
test_parameters_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) $(GTESTPROGS_BUILD_CPPFLAGS)
test_parameters_LDADD=libmarto.la $(GTESTPROGS_BUILD_LDFLAGS)

test_random_SOURCES=test_random.cpp
test_random_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) $(GTESTPROGS_BUILD_CPPFLAGS)
test_random_LDADD=libmarto.la $(GTESTPROGS_BUILD_LDFLAGS)

test_simple_forward_SOURCES=test_simple-forward.cpp test_transition.h
test_simple_forward_CPPFLAGS=$(AM_CPPFLAGS) $(CPPFLAGS) $(GTESTPROGS_BUILD_CPPFLAGS)
test_simple_forward_LDADD=libmarto.la $(GTESTPROGS_BUILD_LDFLAGS)

#test_transition_SOURCES=test_transition.cpp
#test_transition_LDADD=libmarto.la

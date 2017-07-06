

AM_CPPFLAGS=@AM_CPPFLAGS@ -Werror
AM_CFLAGS=@AM_CFLAGS@ $(STD_C99_FLAGS)
AM_CXXFLAGS=@AM_CXXFLAGS@ $(STD_CXX0X_FLAGS)
AM_LDFLAGS=@AM_LDFLAGS@ $(MARTOPROGS_BUILD_LDFLAGS)
AM_LIBS=@AM_LIBS@ $(MARTOPROGS_BUILD_LIBS)

#check_headers_c_headers_SRC=  check-headers/c/marto.h.c     check-headers/c/marto_error.h.c
#check_headers_cxx_headers_SRC=check-headers/c++/marto.h.c++ check-headers/c++/marto_error.h.c++
check_headers_cxx_headers_SRC=check-headers/c++/marto.h.c++
check_headers_cppflags=@AM_CPPFLAGS@ $(MARTOPROGS_BUILD_CPPFLAGS)
check_headers_cflags=  @AM_CFLAGS@   $(MARTOPROGS_BUILD_CFLAGS)
check_headers_cxxflags=@AM_CXXFLAGS@ $(MARTOPROGS_BUILD_CXXFLAGS)
check_headers_ldflags= @AM_LDFLAGS@  $(MARTOPROGS_BUILD_LDFLAGS)
check_headers_libs=                  $(MARTOPROGS_BUILD_LIBS)

include ../check-headers/check-headers.am

noinst_PROGRAMS+=   \
  $(check_headers_prgs)


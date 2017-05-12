#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([MARTO], [0.0.1], [https://gitlab.inria.fr/MarTo/marto], [marto])
AC_CONFIG_SRCDIR([src/core/marto.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.9 tar-pax check-news color-tests parallel-tests subdir-objects])
AMX_SILENT_RULES([AM_DEFAULT_VERBOSITY=1])
AC_CONFIG_HEADERS([config.h])
ACX_NTG_INIT
ACX_NTG_ADD_DEFAULT_GROUP([core])

AM_PROG_AR
LT_INIT

AC_CONFIG_TESTDIR([tests], [.])

# Checks for programs.
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AX_PTHREAD([],[
  AC_MSG_ERROR([The C compiler must be able to compile PThread programs])])

AC_MSG_CHECKING([whether $CXX works])
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE(
            [AC_LANG_PROGRAM([],[return 0;])],
            [have_cxx=yes],
            [have_cxx=no])
AC_LANG_POP([C++])
AC_MSG_RESULT([$have_cxx])
AM_CONDITIONAL([HAVE_CXX],[test $have_cxx = yes])

# Default compilation flags
CFLAGS="-O2 -g -Wall"
AC_SUBST([CFLAGS])

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldl':
dnl AC_CHECK_LIB([dl], [main])


# Checks for header files.
AC_CHECK_HEADERS([])

# Checks for typedefs, structures, and compiler characteristics.

dnl ----------------------------------------------------------------
dnl Define libs and progs created by this software
dnl ----------------------------------------------------------------
ACX_LIB_NEW_LIB([MARTOLIB], [-L\${libdir} -lmarto], [marto])
ACX_LIB_ADD_PUBLIC_FLAGS([MARTOLIB], [-I\${includedir}])

ACX_LIB_NEW_PRG([MARTOPROGS])

dnl -----------------------------------------------------------------------
dnl Dependencies between libs
dnl -----------------------------------------------------------------------
ACX_LIB_LINK([MARTOPROGS], [MARTOLIB])
ACX_LIB_ADD_BUILD_FLAGS([MARTOPROGS],
        [-I\$(top_srcdir)/src/core],[],[],
        [\$(top_builddir)/src/core/libmarto.la])

# Checks for library functions.
AC_CONFIG_FILES([
	src/core/Makefile
	Makefile
	tests/Makefile
	tests/check-headers/Makefile
	tests/marto/Makefile

	tests/atlocal
])
AC_OUTPUT

AC_MSG_NOTICE([*************************************])
AC_MSG_NOTICE([*             Summary               *])
AC_MSG_NOTICE([*************************************])
ACX_LIB_SHOW_EXTERNAL_LIBS
AC_MSG_NOTICE([*************************************])
ACX_LIB_SHOW_NEW_LIBS
AC_MSG_NOTICE([*************************************])
ACX_LIB_SHOW_NEW_PRGS
AC_MSG_NOTICE([*************************************])
list=""
for flag in CC CXX CFLAGS CPPFLAGS CXXFLAGS; do
  for f in AM@&t@_$flag $flag ; do
    val=`eval echo '"$'$f'"'`
    val=`echo "$val" | sed 's/^ *//'`
    if test x"$val" != x ; then
       list="$list
  $f: $val"
    fi
  done
done


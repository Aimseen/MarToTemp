#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([PSI4], [0.0.1], [https://gitlab.inria.fr/PSI/psi4], [psi4])
AC_CONFIG_SRCDIR([src/core/psi4.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.9 tar-pax check-news color-tests parallel-tests])
AMX_SILENT_RULES([AM_DEFAULT_VERBOSITY=1])
AC_CONFIG_HEADERS([config.h])

AM_PROG_AR
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AX_PTHREAD([],[
  AC_MSG_ERROR([The C compiler must be able to compile PThread programs])])

# Default compilation flags
CFLAGS="-O2 -g -Wall"
AC_SUBST([CFLAGS])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldl':
dnl AC_CHECK_LIB([dl], [main])


# Checks for header files.
AC_CHECK_HEADERS([])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([
	src/core/Makefile
	Makefile
])
AC_OUTPUT

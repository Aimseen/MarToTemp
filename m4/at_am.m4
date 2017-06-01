dnl at_am.m4 - Macros to manage autotest testsuite with automake.   -*- Autoconf -*-
dnl
dnl Copyright © 2017 Vincent Danjean <vdanjean@debian.org>
dnl
dnl     Copying and distribution of this file, with or without modification,
dnl     are permitted in any medium without royalty provided the copyright
dnl     notice and this notice are preserved.
dnl
dnl @author Vincent Danjean <Vincent.Danjean@imag.fr>
dnl @version 2017-05-19
dnl @license AllPermissive
dnl @category InstalledPackages
dnl
dnl ==================================================================
dnl These macros aims to help to manage autotest testsuites with automake

AC_DEFUN([AT_AM_INIT],
[AC_REQUIRE([AM_MAKE_INCLUDE])dnl
])dnl

AC_DEFUN([AT_AM_TESTSUITES],[dnl
  AC_CONFIG_TESTDIR([$1],[$2])
])

dnl acx_atx_ntg - Macros to manage named test groups for autoconf.   -*- Autoconf -*-
dnl
dnl Copyright © 2008,2009 Vincent Danjean <vdanjean@debian.org>
dnl
dnl     Copying and distribution of this file, with or without modification,
dnl     are permitted in any medium without royalty provided the copyright
dnl     notice and this notice are preserved.
dnl
dnl @author Vincent Danjean <Vincent.Danjean@imag.fr>
dnl @version 2008-06-15
dnl @license AllPermissive
dnl @category InstalledPackages
dnl
AC_DEFUN([ACX_NTG_INIT],
[AT_NTG_TESTGROUPS=
AC_SUBST([AT_NTG_TESTGROUPS])dnl
])
dnl
AC_DEFUN([ACX_NTG_ADD_DEFAULT_GROUP],
[AC_REQUIRE([ACX_NTG_INIT])dnl
AT_NTG_TESTGROUPS="$AT_NTG_TESTGROUPS $1"
])

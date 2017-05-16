
dnl Try to avoid typo in macro names
m4_pattern_forbid([^_?AT_IP])

m4_define([AT_IP_EXPORT_RUNTIME],[
  AT_ENV_EXPORT([LD_LIBRARY_PATH],["$at_ip_libdir:$LD_LIBRARY_PATH"])
  AT_ENV_EXPORT([PKG_CONFIG_PATH],["$at_ip_pkgconfigdir:$PKG_CONFIG_PATH"])
  AT_ENV_EXPORT([MANPATH],["$at_ip_mandir:$MANPATH"])
])


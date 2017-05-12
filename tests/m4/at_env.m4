dnl
dnl M4 macros for autotest
dnl
dnl Version: 2017-06-12
dnl
dnl These macros allows one to define or remove environment variables
dnl while executing autotest checks.
dnl The effect of AT_ENV_EXPORT and AT_ENV_UNSET is limited to the current
dnl test (i.e. between the AT_SETUP and AT_CLEANUP)
dnl
dnl Available macros:
dnl * AT_ENV_SHOW(VAR1, VAR2, ...)
dnl   shows in the autotest log the value of the environment variables
dnl   VAR1, VAR2, ...
dnl * AT_ENV_EXPORT(VAR1, value1, VAR2, value2, ...)
dnl   VAR1, VAR2, ... variables will be present in the environment while
dnl   executing checks with the respective values value1, value2, ...
dnl   No quoting is done. Basically, the following is executed:
dnl   VAR1=value1; export VAR1; ...
dnl   The exported variables are displayed in the autotest log
dnl * AT_ENV_UNSET(VAR1, VAR2, ...)
dnl   unsets VAR1, VAR2, ... variables in the check environment
dnl 
dnl Changes:
dnl 2017-06-12: add documentation
dnl
m4_pattern_forbid([^_?AT_ENV])
m4_define([AT_ENV_SHOW],[dnl
  m4_if([[$1]],[[]],,[
    eval _at_envval='"$'"$1"'"'
    AS_ECHO(["environment: $1='$(AS_ECHO(["$_at_envval"]) | sed "s/'/'\\\\''/g")'"])
    AT_ENV_SHOW(m4_shift($@))])
])
m4_define([_AT_ENV_SHOW_REC],[dnl
  m4_if([[$1]],[[]],,[
    AT_ENV_SHOW([$1])
    _AT_ENV_SHOW_REC(m4_shift(m4_shift($@)))])
])
m4_define([_AT_ENV_EXPORT_REC],[dnl
  m4_if([[$1]],[[]],,[
    $1=$2
    export $1
    AS_ECHO(["$at_srcdir/AT_LINE: AS_ESCAPE([[export $1=$2]])"])
    _AT_ENV_EXPORT_REC(m4_shift(m4_shift($@)))])
])
m4_define([AT_ENV_EXPORT],[dnl
  _AT_ENV_EXPORT_REC($@)
  _AT_ENV_SHOW_REC($@)
])
m4_define([AT_ENV_UNSET],[dnl
  m4_if([[$1]],[[]],,[
    unset $1
    AS_ECHO(["$at_srcdir/AT_LINE: AS_ESCAPE([[unset $1]])"])
    AT_ENV_UNSET(m4_shift($@))])
])
  

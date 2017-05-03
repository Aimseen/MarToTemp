
m4_include([m4/atenv.m4])
m4_include([check-headers/check-headers.m4])

dnl Try to avoid typo in macro names
m4_pattern_forbid([^_?PSI4_AT_])
dnl But some used shell variables must be allowed
m4_pattern_allow([^(PSI4_AT_TESTSUITES|PSI4_AT_EX_CURRENT_LIST)$])

dnl **********************************************
dnl Hook system
dnl **********************************************

dnl Cleanup macro hooks (called internally later)
dnl _PSI4_AT_HOOK_INIT_PREFIX(prefix, blocklist)
m4_define([_PSI4_AT_HOOK_INIT_PREFIX],[
  m4_foreach([_PSI4_AT_HOOK_BLOCK], [$2], [
    m4_foreach([_PSI4_AT_HOOK_WHEN], [[BEFORE],[AFTER]], [
      m4_foreach([_PSI4_AT_HOOK_TYPE], [[_PSI4_AT_HOOK_CODE],[_PSI4_AT_HOOK_MACRO]], [
        m4_define(_PSI4_AT_HOOK_TYPE[$1_]_PSI4_AT_HOOK_BLOCK[_]_PSI4_AT_HOOK_WHEN,[])
      ])
    ])
  ])
])
dnl _PSI4_AT_HOOK_INIT(block1, block2, ...)
m4_define([_PSI4_AT_HOOK_INIT],[
  m4_define([_PSI4_AT_HOOK_LIST_BLOCK],[$@])
  _PSI4_AT_HOOK_INIT_PREFIX([],[_PSI4_AT_HOOK_LIST_BLOCK])
  _PSI4_AT_HOOK_INIT_PREFIX([ONCE],[_PSI4_AT_HOOK_LIST_BLOCK])
])
dnl Run hook associated to a block
dnl _PSI4_AT_HOOK_RUN(block, when)
m4_define([_PSI4_AT_HOOK_RUN],[
  _PSI4_AT_HOOK_CODEONCE_$1_$2
  _PSI4_AT_HOOK_MACROONCE_$1_$2(m4_shift(m4_shift($@)))
  m4_define([_PSI4_AT_HOOK_CODEONCE_$1_$2],[])
  m4_define([_PSI4_AT_HOOK_MACROONCE_$1_$2],[])
  _PSI4_AT_HOOK_CODE_$1_$2
  _PSI4_AT_HOOK_MACRO_$1_$2(m4_shift(m4_shift($@)))
])

dnl User macros
dnl ===========

dnl Add code before or after a block
dnl PSI4_AT_HOOK(block, when, code)
m4_define([PSI4_AT_HOOK],[
  m4_define([_PSI4_AT_HOOK_CODE_$1_$2],[$3])
])
dnl Add a macro to be excuted with the same parameter as the targer block
dnl PSI4_AT_HOOK_MACRO(block, when, macro-name)
m4_define([PSI4_AT_HOOK_MACRO],[
  m4_define([_PSI4_AT_HOOK_MACRO_$1_$2],[$3($]][[@)])
])
dnl Add code before or after a block, only for one occurence
dnl PSI4_AT_HOOK(block, when, code)
m4_define([PSI4_AT_HOOK_ONCE],[
  m4_define([_PSI4_AT_HOOK_CODEONCE_$1_$2],[$3])
])
dnl Idem for macro
dnl PSI4_AT_HOOK_MACRO(block, when, macro-name)
m4_define([PSI4_AT_HOOK_MACRO_ONCE],[
  m4_define([_PSI4_AT_HOOK_MACROONCE_$1_$2],[$3($]][[@)])
])

dnl **********************************************
dnl Chose whether we are doing tests after or before the installation
dnl **********************************************

m4_define([PSI4_AT_INSTALLED],[m4_if(_PSI4_AT_INSTALLED,true,[$1],[$2])])
m4_define([PSI4_AT_BUILDING],[PSI4_AT_INSTALLED([$2],[$1])])

dnl **********************************************
dnl Convenience macros to do tests for our testsuite
dnl **********************************************

dnl PSI4_AT_BANNER(name, testsuite, keywords, subdir, all-target, options-for-all-target)
m4_define([PSI4_AT_BANNER],[
  AT_BANNER([$1])
  m4_define([_PSI4_AT_TESTSUITE],[$2])
  m4_define([_PSI4_AT_KEYWORDS],[$2 $3])
  m4_define([_PSI4_AT_SUBDIR],[$4])
  m4_define([_PSI4_AT_WRAPPER],[$6])
  _PSI4_AT_HOOK_INIT([SETUP],[CHECK_BUILD],[CHECK_RUN])

  m4_if($5,[clean],[
    PSI4_AT_SETUP([clean compilation into _PSI4_AT_SUBDIR], [clean])
    PSI4_AT_CHECK_BUILD([clean])
    AT_CLEANUP
  ],[
    m4_if($5,[],[],[
      PSI4_AT_SETUP([clean compilation into _PSI4_AT_SUBDIR], [clean])
      PSI4_AT_CHECK_BUILD([clean])
      AT_CLEANUP

      PSI4_AT_SETUP([precompile programs into _PSI4_AT_SUBDIR], [build])
      PSI4_AT_CHECK_BUILD([$5],[-k $6],[ignore])
      AT_CLEANUP
    ])
  ])
])
dnl PSI4_AT_SETUP(name, extra_keywords)
m4_define([PSI4_AT_SETUP],[
  AT_SETUP([$1])
  _PSI4_AT_HOOK_RUN([SETUP],[BEFORE],$@)
  AT_KEYWORDS([_PSI4_AT_KEYWORDS $2])
  m4_if(_PSI4_AT_TESTSUITE,[],[],[
    AT_SKIP_IF([AS_CASE([" $PSI4_AT_TESTSUITES "],[*" _PSI4_AT_TESTSUITE "*],[false],[:])])
  ])
  _PSI4_AT_HOOK_RUN([SETUP],[AFTER],$@)
])

dnl PSI4_AT_CLEANUP
m4_define([PSI4_AT_CLEANUP],[
  AT_CLEANUP
])
dnl PSI4_AT_CHECK_MAKE(make-option/target, exit-code, stdout, stderr)
m4_define([PSI4_AT_CHECK_MAKE],[
   AT_CHECK([${MAKE:-make} -C ${abs_top_builddir}/_PSI4_AT_SUBDIR $1],
     [$2],[$3],[$4])
])
dnl PSI4_AT_CHECK_BUILD(target, options, exit-code)
m4_define([PSI4_AT_CHECK_BUILD],[
  _PSI4_AT_HOOK_RUN([CHECK_BUILD],[BEFORE],$@)
  PSI4_AT_CHECK_MAKE([$2 $1], m4_if([$3],[],[0],[$3]), [ignore], [ignore])
  _PSI4_AT_HOOK_RUN([CHECK_BUILD],[AFTER],$@)
])
dnl PSI4_AT_CHECK_RUN(name, arguments, command-to-parse-stdout, stdout, stderr)
m4_define([PSI4_AT_CHECK_RUN],[
  _PSI4_AT_HOOK_RUN([CHECK_RUN],[BEFORE],$@)
  AT_CHECK([_PSI4_AT_WRAPPER timeout 15 $abs_top_builddir/_PSI4_AT_SUBDIR/$1 $2], 0,[m4_if([$3],[],[$4],[stdout])],[$5])
  m4_if([$3],[],[],[
    AT_CHECK([$3], 0, [$4], [])
  ])
  _PSI4_AT_HOOK_RUN([CHECK_RUN],[AFTER],$@)
])
dnl PSI4_AT_TEST_PROG(prog-name, test-name, keywords, , arguments, command-to-parse-stdout, stdout, stderr)
m4_define([PSI4_AT_TEST_PROG],[
  PSI4_AT_SETUP([$2],[$3 $1])
  PSI4_AT_CHECK_BUILD([$1])
  PSI4_AT_CHECK_RUN([$1],[$5],[$6],[$7],[$8])
  PSI4_AT_CLEANUP
])
dnl PSI4_AT_TEST_BUILD_FAILED(prog-name, test-name, keywords)
m4_define([PSI4_AT_TEST_BUILD_FAILED],[
  PSI4_AT_SETUP([$2],[$3 $1])
  PSI4_AT_CHECK_BUILD([$1], [], [1])
  PSI4_AT_CLEANUP
])



m4_include([m4/atenv.m4])
m4_include([check-headers/check-headers.m4])

dnl Try to avoid typo in macro names
m4_pattern_forbid([^_?MARTO_AT_])
dnl But some used shell variables must be allowed
m4_pattern_allow([^(MARTO_AT_TESTSUITES|MARTO_AT_EX_CURRENT_LIST)$])

dnl **********************************************
dnl Hook system
dnl **********************************************

dnl Cleanup macro hooks (called internally later)
dnl _MARTO_AT_HOOK_INIT_PREFIX(prefix, blocklist)
m4_define([_MARTO_AT_HOOK_INIT_PREFIX],[
  m4_foreach([_MARTO_AT_HOOK_BLOCK], [$2], [
    m4_foreach([_MARTO_AT_HOOK_WHEN], [[BEFORE],[AFTER]], [
      m4_foreach([_MARTO_AT_HOOK_TYPE], [[_MARTO_AT_HOOK_CODE],[_MARTO_AT_HOOK_MACRO]], [
        m4_define(_MARTO_AT_HOOK_TYPE[$1_]_MARTO_AT_HOOK_BLOCK[_]_MARTO_AT_HOOK_WHEN,[])
      ])
    ])
  ])
])
dnl _MARTO_AT_HOOK_INIT(block1, block2, ...)
m4_define([_MARTO_AT_HOOK_INIT],[
  m4_define([_MARTO_AT_HOOK_LIST_BLOCK],[$@])
  _MARTO_AT_HOOK_INIT_PREFIX([],[_MARTO_AT_HOOK_LIST_BLOCK])
  _MARTO_AT_HOOK_INIT_PREFIX([ONCE],[_MARTO_AT_HOOK_LIST_BLOCK])
])
dnl Run hook associated to a block
dnl _MARTO_AT_HOOK_RUN(block, when)
m4_define([_MARTO_AT_HOOK_RUN],[
  _MARTO_AT_HOOK_CODEONCE_$1_$2
  _MARTO_AT_HOOK_MACROONCE_$1_$2(m4_shift(m4_shift($@)))
  m4_define([_MARTO_AT_HOOK_CODEONCE_$1_$2],[])
  m4_define([_MARTO_AT_HOOK_MACROONCE_$1_$2],[])
  _MARTO_AT_HOOK_CODE_$1_$2
  _MARTO_AT_HOOK_MACRO_$1_$2(m4_shift(m4_shift($@)))
])

dnl User macros
dnl ===========

dnl Add code before or after a block
dnl MARTO_AT_HOOK(block, when, code)
m4_define([MARTO_AT_HOOK],[
  m4_define([_MARTO_AT_HOOK_CODE_$1_$2],[$3])
])
dnl Add a macro to be excuted with the same parameter as the targer block
dnl MARTO_AT_HOOK_MACRO(block, when, macro-name)
m4_define([MARTO_AT_HOOK_MACRO],[
  m4_define([_MARTO_AT_HOOK_MACRO_$1_$2],[$3($]][[@)])
])
dnl Add code before or after a block, only for one occurence
dnl MARTO_AT_HOOK(block, when, code)
m4_define([MARTO_AT_HOOK_ONCE],[
  m4_define([_MARTO_AT_HOOK_CODEONCE_$1_$2],[$3])
])
dnl Idem for macro
dnl MARTO_AT_HOOK_MACRO(block, when, macro-name)
m4_define([MARTO_AT_HOOK_MACRO_ONCE],[
  m4_define([_MARTO_AT_HOOK_MACROONCE_$1_$2],[$3($]][[@)])
])

dnl **********************************************
dnl Chose whether we are doing tests after or before the installation
dnl **********************************************

m4_define([MARTO_AT_INSTALLED],[m4_if(_MARTO_AT_INSTALLED,true,[$1],[$2])])
m4_define([MARTO_AT_BUILDING],[MARTO_AT_INSTALLED([$2],[$1])])

dnl **********************************************
dnl Convenience macros to do tests for our testsuite
dnl **********************************************

dnl MARTO_AT_BANNER(name, testsuite, keywords, subdir, all-target, options-for-all-target)
m4_define([MARTO_AT_BANNER],[
  AT_BANNER([$1])
  m4_define([_MARTO_AT_TESTSUITE],[$2])
  m4_define([_MARTO_AT_KEYWORDS],[$2 $3])
  m4_define([_MARTO_AT_SUBDIR],[$4])
  m4_define([_MARTO_AT_WRAPPER],[$6])
  _MARTO_AT_HOOK_INIT([SETUP],[CHECK_BUILD],[CHECK_RUN])

  m4_if($5,[clean],[
    MARTO_AT_SETUP([clean compilation into _MARTO_AT_SUBDIR], [clean])
    MARTO_AT_CHECK_BUILD([clean])
    AT_CLEANUP
  ],[
    m4_if($5,[],[],[
      MARTO_AT_SETUP([clean compilation into _MARTO_AT_SUBDIR], [clean])
      MARTO_AT_CHECK_BUILD([clean])
      AT_CLEANUP

      MARTO_AT_SETUP([precompile programs into _MARTO_AT_SUBDIR], [build])
      MARTO_AT_CHECK_BUILD([$5],[-k $6],[ignore])
      AT_CLEANUP
    ])
  ])
])
dnl MARTO_AT_SETUP(name, extra_keywords)
m4_define([MARTO_AT_SETUP],[
  AT_SETUP([$1])
  _MARTO_AT_HOOK_RUN([SETUP],[BEFORE],$@)
  AT_KEYWORDS([_MARTO_AT_KEYWORDS $2])
  m4_if(_MARTO_AT_TESTSUITE,[],[],[
    AT_SKIP_IF([AS_CASE([" $MARTO_AT_TESTSUITES "],[*" _MARTO_AT_TESTSUITE "*],[false],[:])])
  ])
  _MARTO_AT_HOOK_RUN([SETUP],[AFTER],$@)
])

dnl MARTO_AT_CLEANUP
m4_define([MARTO_AT_CLEANUP],[
  AT_CLEANUP
])
dnl MARTO_AT_CHECK_MAKE(make-option/target, exit-code, stdout, stderr)
m4_define([MARTO_AT_CHECK_MAKE],[
   AT_CHECK([${MAKE:-make} -C ${abs_top_builddir}/_MARTO_AT_SUBDIR $1],
     [$2],[$3],[$4])
])
dnl MARTO_AT_CHECK_BUILD(target, options, exit-code)
m4_define([MARTO_AT_CHECK_BUILD],[
  _MARTO_AT_HOOK_RUN([CHECK_BUILD],[BEFORE],$@)
  MARTO_AT_CHECK_MAKE([$2 $1], m4_if([$3],[],[0],[$3]), [ignore], [ignore])
  _MARTO_AT_HOOK_RUN([CHECK_BUILD],[AFTER],$@)
])
dnl MARTO_AT_CHECK_RUN(name, arguments, command-to-parse-stdout, stdout, stderr)
m4_define([MARTO_AT_CHECK_RUN],[
  _MARTO_AT_HOOK_RUN([CHECK_RUN],[BEFORE],$@)
  AT_CHECK([_MARTO_AT_WRAPPER timeout 15 $abs_top_builddir/_MARTO_AT_SUBDIR/$1 $2], 0,[m4_if([$3],[],[$4],[stdout])],[$5])
  m4_if([$3],[],[],[
    AT_CHECK([$3], 0, [$4], [])
  ])
  _MARTO_AT_HOOK_RUN([CHECK_RUN],[AFTER],$@)
])
dnl MARTO_AT_TEST_PROG(prog-name, test-name, keywords, , arguments, command-to-parse-stdout, stdout, stderr)
m4_define([MARTO_AT_TEST_PROG],[
  MARTO_AT_SETUP([$2],[$3 $1])
  MARTO_AT_CHECK_BUILD([$1])
  MARTO_AT_CHECK_RUN([$1],[$5],[$6],[$7],[$8])
  MARTO_AT_CLEANUP
])
dnl MARTO_AT_TEST_BUILD_FAILED(prog-name, test-name, keywords)
m4_define([MARTO_AT_TEST_BUILD_FAILED],[
  MARTO_AT_SETUP([$2],[$3 $1])
  MARTO_AT_CHECK_BUILD([$1], [], [1])
  MARTO_AT_CLEANUP
])


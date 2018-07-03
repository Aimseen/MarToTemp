dnl
dnl M4 macros for autotest
dnl
dnl Version: 2017-06-12
dnl
dnl These macros allows one manipulate group of tests (i.e. Named
dnl TestSuites) and easily assigning them keywords
dnl
dnl Available macros:
dnl * AT_NTG_BANNER(name, testsuite, keywords, subdir, all-target,
dnl   		     options-for-all-target)
dnl   Starts a new group of tests. All tests (if declared with
dnl   the AT_NTG_SETUP macro) will be associated with the
dnl   provided `keywords' and `testsuite' keywords.
dnl     If the AT_NTG_TESTGROUPS shell variable does not contain the
dnl   `testsuite' word, all theses test will be skipped. This
dnl   allows one to easily defined some tests depending on features
dnl   that can be enabled or not in configure.
dnl
dnl     If `subdir' if not empty, a 'make clean' will be done in
dnl   the `subdir' directory (relative to the top_builddir directory)
dnl   as an autotest test.
dnl     If `subdir' if not empty and `all-target' is not `clean',
dnl   a 'make all-target -k options-for-all-target' will be done
dnl   in the `subdir' directory as a second autotest test.
dnl
dnl * dnl AT_NTG_SETUP(name, extra_keywords)
dnl   Starts an autotest (as AT_SETUP(name)) but it also define
dnl   the `extra_keywords' keywords *and* the keywords of current
dnl   group of tests (see AT_NTG_BANNER)
dnl
dnl * AT_NTG_CLEANUP
dnl   Ends a AT_NTG_SETUP test.
dnl   Currently a synonym for AT_CLEANUP
dnl
dnl * AT_NTG_CHECK_MAKE(make-option/target, exit-code, stdout, stderr)
dnl   Calls the AT_CHECK macro, calling 'make' into the subdir of the
dnl   current group of tests. The exit code is taken into account
dnl   (by default, 0 is required), as well as stdout and stderr to see
dnl   if the test passes.
dnl 
dnl * AT_NTG_CHECK_BUILD(target, options, exit-code)
dnl   Same as AT_NTG_CHECK_MAKE but stdout and stderr are ignored
dnl 
dnl * AT_NTG_CHECK_RUN(name, arguments, command-to-parse-stdout, stdout, stderr)
dnl   Runs the `name' program in the subdir of the current group of tests
dnl   with the `arguments' arguments.
dnl     `stdout' and `stderr' are checked from the stdout and stderr
dnl   of the program.
dnl     If `command-to-parse-stdout' is defined, its stdout output is
dnl   compared against `stdout' (instead of the stdout of the program).
dnl   `command-to-parse-stdout' can get the stdout of the program in
dnl   the "stdout" file.
dnl     In any case, a timeout of 15 seconds is used when running
dnl   the command.
dnl
dnl * AT_NTG_TEST_PROG(prog-name, test-name, keywords, , arguments, command-to-parse-stdout, stdout, stderr)
dnl   Calls AT_NTG_SETUP, AT_NTG_CHECK_BUILD, AT_NTG_CHECK_RUN, and
dnl   AT_NTG_CLEANUP in sequence
dnl  
dnl * AT_NTG_TEST_BUILD_FAILED(prog-name, test-name, keywords)
dnl   Calls AT_NTG_SETUP, AT_NTG_CHECK_BUILD and AT_NTG_CLEANUP in
dnl   sequence, expecting AT_NTG_CHECK_BUILD to have a exit status of 1.
dnl
dnl * AT_NTG_INSTALLED(tests)
dnl   skip tests (when generating the testsuite) if tests target the
dnl   'make check' goal.
dnl
dnl * AT_NTG_BUILDING(tests)
dnl   skip tests (when generating the testsuite) if tests target the
dnl   'make installcheck' goal.
dnl
dnl * AT_NTG_INSTALL_TESTSUITE
dnl   Macro to call at the start of the testsuite used for 'make installcheck'
dnl
dnl Changes:
dnl 2017-06-12: add documentation and change macro prefixes
dnl

dnl Try to avoid typo in macro names
m4_pattern_forbid([^_?AT_NTG])
dnl But some used shell variables must be allowed
m4_pattern_allow([^(AT_NTG_TESTGROUPS)$])

dnl **********************************************
dnl Hook system
dnl **********************************************

dnl Cleanup macro hooks (called internally later)
dnl _AT_NTG_HOOK_INIT_PREFIX(prefix, blocklist)
m4_define([_AT_NTG_HOOK_INIT_PREFIX],[
  m4_foreach([_AT_NTG_HOOK_BLOCK], [$2], [
    m4_foreach([_AT_NTG_HOOK_WHEN], [[BEFORE],[AFTER]], [
      m4_foreach([_AT_NTG_HOOK_TYPE], [[_AT_NTG_HOOK_CODE],[_AT_NTG_HOOK_MACRO]], [
        m4_define(_AT_NTG_HOOK_TYPE[$1_]_AT_NTG_HOOK_BLOCK[_]_AT_NTG_HOOK_WHEN,[])
      ])
    ])
  ])
])
dnl _AT_NTG_HOOK_INIT(block1, block2, ...)
m4_define([_AT_NTG_HOOK_INIT],[
  m4_define([_AT_NTG_HOOK_LIST_BLOCK],[$@])
  _AT_NTG_HOOK_INIT_PREFIX([],[_AT_NTG_HOOK_LIST_BLOCK])
  _AT_NTG_HOOK_INIT_PREFIX([ONCE],[_AT_NTG_HOOK_LIST_BLOCK])
])
dnl Run hook associated to a block
dnl _AT_NTG_HOOK_RUN(block, when)
m4_define([_AT_NTG_HOOK_RUN],[
  _AT_NTG_HOOK_CODEONCE_$1_$2
  _AT_NTG_HOOK_MACROONCE_$1_$2(m4_shift(m4_shift($@)))
  m4_define([_AT_NTG_HOOK_CODEONCE_$1_$2],[])
  m4_define([_AT_NTG_HOOK_MACROONCE_$1_$2],[])
  _AT_NTG_HOOK_CODE_$1_$2
  _AT_NTG_HOOK_MACRO_$1_$2(m4_shift(m4_shift($@)))
])

dnl User macros
dnl ===========

dnl Add code before or after a block
dnl AT_NTG_HOOK(block, when, code)
m4_define([AT_NTG_HOOK],[
  m4_define([_AT_NTG_HOOK_CODE_$1_$2],[$3])
])
dnl Add a macro to be excuted with the same parameter as the targer block
dnl AT_NTG_HOOK_MACRO(block, when, macro-name)
m4_define([AT_NTG_HOOK_MACRO],[
  m4_define([_AT_NTG_HOOK_MACRO_$1_$2],[$3($]][[@)])
])
dnl Add code before or after a block, only for one occurence
dnl AT_NTG_HOOK(block, when, code)
m4_define([AT_NTG_HOOK_ONCE],[
  m4_define([_AT_NTG_HOOK_CODEONCE_$1_$2],[$3])
])
dnl Idem for macro
dnl AT_NTG_HOOK_MACRO(block, when, macro-name)
m4_define([AT_NTG_HOOK_MACRO_ONCE],[
  m4_define([_AT_NTG_HOOK_MACROONCE_$1_$2],[$3($]][[@)])
])

dnl **********************************************
dnl Convenience macros to do tests for our testsuite
dnl **********************************************

dnl AT_NTG_BANNER(name, testsuite, keywords, subdir, all-target, options-for-all-target)
m4_define([AT_NTG_BANNER],[
  AT_BANNER([$1])
  m4_define([_AT_NTG_TESTGROUP],[$2])
  m4_define([_AT_NTG_KEYWORDS],[$2 $3])
  m4_define([_AT_NTG_SUBDIR],[$4])
  m4_define([_AT_NTG_WRAPPER],[$7])
  _AT_NTG_HOOK_INIT([SETUP],[CHECK_BUILD],[CHECK_RUN])

  m4_if($5,[clean],[
    AT_NTG_SETUP([clean directory _AT_NTG_SUBDIR], [clean])
    AT_NTG_CHECK_BUILD([clean])
    AT_NTG_CLEANUP
  ],[
    m4_if($5,[],[],[
      AT_NTG_SETUP([clean directory  _AT_NTG_SUBDIR], [clean])
      AT_NTG_CHECK_BUILD([clean])
      AT_NTG_CLEANUP

      AT_NTG_SETUP([compile programs in _AT_NTG_SUBDIR], [build])
      AT_NTG_CHECK_BUILD([$5],[-k $6],[ignore])
      AT_NTG_CLEANUP
    ])
  ])
])
dnl AT_NTG_SETUP(name, extra_keywords)
m4_define([AT_NTG_SETUP],[
  AT_SETUP([$1])
  _AT_NTG_HOOK_RUN([SETUP],[BEFORE],$@)
  AT_KEYWORDS([_AT_NTG_KEYWORDS $2])
  m4_if(_AT_NTG_TESTGROUP,[],[],[
    AT_SKIP_IF([AS_CASE([" $AT_NTG_TESTGROUPS "],[*" _AT_NTG_TESTGROUP "*],[false],[:])])
  ])
  _AT_NTG_HOOK_RUN([SETUP],[AFTER],$@)
])

dnl AT_NTG_CLEANUP
m4_define([AT_NTG_CLEANUP],[
  AT_CLEANUP
])
dnl AT_NTG_CHECK_MAKE(make-option/target, exit-code, stdout, stderr)
m4_define([AT_NTG_CHECK_MAKE],[
   AT_CHECK([${MAKE:-make} -C ${abs_top_builddir}/_AT_NTG_SUBDIR $1],
     [$2],[$3],[$4])
])
dnl AT_NTG_CHECK_BUILD(target, options, exit-code)
m4_define([AT_NTG_CHECK_BUILD],[
  _AT_NTG_HOOK_RUN([CHECK_BUILD],[BEFORE],$@)
  AT_NTG_CHECK_MAKE([$2 $1], m4_if([$3],[],[0],[$3]), [ignore], [ignore])
  _AT_NTG_HOOK_RUN([CHECK_BUILD],[AFTER],$@)
])
dnl AT_NTG_CHECK_RUN(name, arguments, command-to-parse-stdout, stdout, stderr, wrapper)
m4_define([AT_NTG_CHECK_RUN],[
  _AT_NTG_HOOK_RUN([CHECK_RUN],[BEFORE],$@)
  AT_CHECK([_AT_NTG_WRAPPER $TIMEOUT 15 $6 $abs_top_builddir/_AT_NTG_SUBDIR/$1 $2], 0,[m4_if([$3],[],[$4],[stdout])],[$5])
  m4_if([$3],[],[],[
    AT_CHECK([$3], 0, [$4], [])
  ])
  _AT_NTG_HOOK_RUN([CHECK_RUN],[AFTER],$@)
])
dnl AT_NTG_TEST_PROG(prog-name, test-name, keywords, wrapper, arguments, command-to-parse-stdout, stdout, stderr)
m4_define([AT_NTG_TEST_PROG],[
  AT_NTG_SETUP([$2],[$3 $1])
  AT_NTG_CHECK_BUILD([$1])
  AT_NTG_CHECK_RUN([$1],[$5],[$6],[$7],[$8], [$4])
  AT_NTG_CLEANUP
])
dnl AT_NTG_TEST_BUILD_FAILED(prog-name, test-name, keywords)
m4_define([AT_NTG_TEST_BUILD_FAILED],[
  AT_NTG_SETUP([$2],[$3 $1])
  AT_NTG_CHECK_BUILD([$1], [], [1])
  AT_NTG_CLEANUP
])

dnl **********************************************
dnl Chose whether we are doing tests after or before the installation
dnl **********************************************

m4_define([AT_NTG_INSTALLED],[m4_if(_AT_NTG_INSTALLED,[true],[$1],[$2])])
m4_define([AT_NTG_BUILDING],[AT_NTG_INSTALLED([$2],[$1])])
m4_define([AT_NTG_INSTALL_TESTSUITE],[dnl
	m4_define([_AT_NTG_INSTALLED],[true])])

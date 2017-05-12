dnl PSI4_AT_TEST_HEADERS(extra_keywords) 
m4_define([PSI4_AT_TEST_HEADERS],[
  PSI4_AT_SETUP([Checking whether header files compile], [$1])
  PSI4_AT_CHECK_BUILD([check_header])
  PSI4_AT_CHECK_BUILD([check_header_strict])
  PSI4_AT_CLEANUP
])

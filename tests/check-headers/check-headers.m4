dnl MARTO_AT_TEST_HEADERS(extra_keywords) 
m4_define([MARTO_AT_TEST_HEADERS],[
  MARTO_AT_SETUP([Checking whether header files compile], [$1])
  MARTO_AT_CHECK_BUILD([check_header])
  MARTO_AT_CHECK_BUILD([check_header_strict])
  MARTO_AT_CLEANUP
])

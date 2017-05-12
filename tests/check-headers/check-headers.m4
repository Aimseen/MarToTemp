dnl AT_NTG_CH_TEST_HEADERS(testgroup, directory, extra_keywords) 
m4_define([AT_NTG_CH_TEST_HEADERS],[
  AT_NTG_BANNER([Headers check], [$1], [headers $3], [$2], [clean])
  AT_NTG_SETUP([Checking whether header files compile], [$1])
  AT_NTG_CHECK_BUILD([check_header])
  AT_NTG_CHECK_BUILD([check_header_strict])
  AT_NTG_CLEANUP
])

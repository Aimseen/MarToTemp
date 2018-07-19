
include m4/at_am_init.am

SUBDIRS=check-headers

# Subdirs not to be built by default (will be built by the testsuite)
# but we want them to be cleaned up anyway
NOBUILD_SUBDIRS=marto-headers marto-installed core
DIST_SUBDIRS=$(NOBUILD_SUBDIRS) $(SUBDIRS)
clean-recursive: SUBDIRS+=$(NOBUILD_SUBDIRS)

BUILT_SOURCES=marto-installed/using-libmarto.c++
DISTCLEANFILES += marto-installed/using-libmarto.c++

marto-installed/using-libmarto.c++: $(top_srcdir)/src/core/test_core.cpp
	cp $< $@

AT_TESTSUITES=testsuite testsuite-installed

package_m4_INCLUDES= \
	check-headers/check-headers.m4 \
	m4/at_env.m4 \
	m4/at_named-test-groups.m4 \
	m4/at_install-paths.m4

testsuite_DEPENDS= \
	testsuite-basic.at \
	testsuite-marto.at \
	testsuite-marto-core.at
check-local: check-testsuite

testsuite_installed_DEPENDS= \
	testsuite.at \
	$(testsuite_DEPENDS)
testsuite_installed_ATPATH=$(bindir):$(abs_top_builddir)
installcheck-local: check-testsuite_installed

EXTRA_DIST += scripts/timeout-missing

# Support to get install paths at test time
# (useful for the installcheck target)
include m4/at_install-paths_am.am

include m4/at_am.am

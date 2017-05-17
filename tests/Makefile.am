
include m4/at_am_init.am

SUBDIRS=check-headers

# Subdirs not to be built by default (will be built by the testsuite)
# but we want them to be cleaned up anyway
NOBUILD_SUBDIRS=marto-headers marto-installed
DIST_SUBDIRS=$(NOBUILD_SUBDIRS) $(SUBDIRS)
clean-recursive: SUBDIRS+=$(NOBUILD_SUBDIRS)

AT_TESTSUITES=testsuite testsuite-installed

package_m4_INCLUDES= \
	check-headers/check-headers.m4 \
	m4/at_env.m4 \
	m4/at_named-test-groups.m4 \
	m4/at_install-paths.m4

testsuite_DEPENDS= \
	testsuite-basic.at \
	testsuite-marto.at
check-local: check-testsuite

testsuite_installed_DEPENDS= \
	testsuite.at \
	$(testsuite_DEPENDS)
testsuite_installed_ATPATH=$(bindir):$(abs_top_builddir)
installcheck-local: check-testsuite_installed

EXTRA_DIST += marto-installed/using-libmarto.c++

# Support to get install paths at test time
# (useful for the installcheck target)
include m4/at_install-paths_am.am

include m4/at_am.am

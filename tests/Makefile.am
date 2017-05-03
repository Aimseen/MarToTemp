
SUBDIRS=check-headers

# Subdirs not to be built by default (will be built by the testsuite)
# but we want them to be cleaned up anyway
NOBUILD_SUBDIRS=psi4
DIST_SUBDIRS=$(NOBUILD_SUBDIRS) $(SUBDIRS)
clean-recursive: SUBDIRS+=$(NOBUILD_SUBDIRS)

# The `:;' works around a Bash 3.2 bug when the output is not writable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	:;{ \
	  echo '# Signature of the current package.' && \
	  echo 'm4_define([AT_PACKAGE_NAME],' && \
	  echo '  [$(PACKAGE_NAME)])' && \
	  echo 'm4_define([AT_PACKAGE_TARNAME],' && \
	  echo '  [$(PACKAGE_TARNAME)])' && \
	  echo 'm4_define([AT_PACKAGE_VERSION],' && \
	  echo '  [$(PACKAGE_VERSION)])' && \
	  echo 'm4_define([AT_PACKAGE_STRING],' && \
	  echo '  [$(PACKAGE_STRING)])' && \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT],' && \
	  echo '  [$(PACKAGE_BUGREPORT)])'; \
	  echo 'm4_define([AT_PACKAGE_URL],' && \
	  echo '  [$(PACKAGE_URL)])'; \
	  echo 'm4_define([AT_PACKAGE_OPENCL_VERSION],' && \
	  echo '  [$(OPENCL_VERSION)])'; \
	} >'$(srcdir)/package.m4'

TESTSUITES_SRC = testsuite.at testsuite-basic.at \
	check-headers/check-headers.m4 \
	m4/atenv.m4 \
	m4/psi4-autotest.m4
EXTRA_DIST = $(TESTSUITES_SRC) $(srcdir)/package.m4 $(TESTSUITE) $(INSTTESTSUITE) atlocal.in
TESTSUITE = $(srcdir)/testsuite
INSTTESTSUITE = $(srcdir)/testsuite-installed
DISTCLEANFILES = atconfig

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

installcheck-local: atconfig atlocal $(INSTTESTSUITE)
	$(SHELL) '$(INSTTESTSUITE)' AUTOTEST_PATH='$(bindir):$(abs_top_builddir)' \
	$(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean
	test ! -f '$(INSTTESTSUITE)' || \
	$(SHELL) '$(INSTTESTSUITE)' --clean

AUTOM4TE = $(SHELL) $(top_srcdir)/build-aux/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): $(TESTSUITES_SRC) $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

$(INSTTESTSUITE): $(TESTSUITES_SRC) $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@



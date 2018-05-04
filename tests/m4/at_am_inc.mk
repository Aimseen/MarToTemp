#_at_m4_join: $1: sep, $2: first $3: second
_at_m4_join = $(if $(and $2,$3),$2$1$3,$2$3)

define _at_m4_gen # $1: AM_name  $2: filename
$1_TESTSUITEFLAGS ?= $$(AM_TESTSUITEFLAGS) $$(TESTSUITEFLAGS)
$1_ATPATH ?= $$(call _at_m4_join,:,$$(AM_ATPATH),$$(AUTOTEST_PATH))
$1_MAIN ?= $2.at
$$(srcdir)/$2: $$($1_MAIN) $$($1_DEPENDS) $$(srcdir)/package.m4
	$$(AUTOTEST) -I '$$(srcdir)' -o $$@.tmp $$@.at
	mv $$@.tmp $$@
.PHONY: check-$1 clean-$1 clean-local
check-$1: $$(srcdir)/$2 atconfig atlocal
	$$(SHELL) '$$<' \
		$$(if $$($1_ATPATH),AUTOTEST_PATH='$$($1_ATPATH)') \
		$$(testsuite__installed_ATFLAGS) $$(ATFLAGS) \
		$$($1_TESTSUITEFLAGS)
clean-$1:
	test ! -f '$$(srcdir)/$2' || \
	$$(SHELL) '$$(srcdir)/$2' --clean
clean-local: clean-$1
EXTRA_DIST += $$($1_MAIN) $$($1_DEPENDS)
endef

$(foreach _at_am_testsuite,$(AT_TESTSUITES),\
$(eval $(call _at_m4_gen,$(shell echo $(_at_am_testsuite) | $(SED) -e 's/[^[:alnum:]]/_/g'),$(strip $(_at_am_testsuite)))))

########################
# Doxygen support
html-local: doxygen-run

@DX_RULES@

MOSTLYCLEANFILES=$(DX_CLEANFILES)
########################

pdf-local: $(DX_DOCDIR)/MarTo.pdf

.PHONY: $(DX_DOCDIR)/MarTo.pdf
$(DX_DOCDIR)/MarTo.pdf: $(DX_DOCDIR)/latex/MarTo.pdf

$(DX_DOCDIR)/latex/MarTo.pdf: $(DX_DOCDIR)/latex/GNUmakefile
	$(MAKE) -C $(DX_DOCDIR)/latex pdf-copy

$(DX_DOCDIR)/latex/GNUmakefile: $(DX_DOCDIR)/latex Makefile.am
	echo "LU_MASTERS=MarTo" > $@
	echo "LU_MarTo_MAIN=refman.tex" >> $@
	echo "include LaTeX.mk" >> $@
	echo "pdf-copy: ../MarTo.pdf" >> $@
	echo "../MarTo.pdf: MarTo.pdf" >> $@
	echo '	cp $$< $$@' >> $@

$(DX_DOCDIR)/latex: doxygen-run 

MOSTLYCLEANFILES+=$(DX_DOCDIR)/MarTo.pdf

pdf_DATA=$(DX_DOCDIR)/MarTo.pdf

install-data-local: html
	$(MKDIR_P) $(DESTDIR)$(datadir)/$(PACKAGE)/html/search
	for f in $(DX_DOCDIR)/html/* ; do \
		if [ $$f = search ]; then continue ; fi ; \
		$(INSTALL_DATA) $$f \
			$(DESTDIR)$(datadir)/$(PACKAGE)/html; \
	done
	for f in $(DX_DOCDIR)/html/search/* ; do \
		if [ $$f = search ]; then continue ; fi ; \
		$(INSTALL_DATA) $$f \
			$(DESTDIR)$(datadir)/$(PACKAGE)/html/search; \
	done

uninstall-local:
	$(RM) -r $(DESTDIR)$(datadir)/$(PACKAGE)/html


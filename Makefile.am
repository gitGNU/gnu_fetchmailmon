## Process this file with automake to produce Makefile.in
## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

SUBDIRS = src doc sh

fetchmailmondocdir = ${prefix}/doc/fetchmailmon
fetchmailmondoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS\
	TODO

EXTRA_DIST = $(fetchmailmondoc_DATA) ChangeLog.0

.PHONY: generate-changelog
generate-changelog:
	if test -d $(top_srcdir)/.git; then \
          cd $(top_srcdir) ; sh $(top_srcdir)/maintainer/git2changelog.sh > $(distdir)/c-l; \
          mv $(distdir)/c-l $(distdir)/ChangeLog; \
        fi

# Copy all the spec files. Of cource, only one is actually used.
dist-hook: generate-changelog
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

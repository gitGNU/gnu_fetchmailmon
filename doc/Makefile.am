# Copyright (C) 2000 Idan Shoham <idan@m-tech.ab.ca>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

man_MANS = txtfetchmailmon.1 \
           dialogfetchmailmon.1

EXTRA_DIST = txtfetchmailmon.1.in \
	         dialogfetchmailmon.1.in

BUILT_SOURCES = txtfetchmailmon.ps \
                dialogfetchmailmon.ps

CLEANFILES = $(man_MANS) $(BUILT_SOURCES)

txtfetchmailmon.ps: txtfetchmailmon.1
	groff -T ps -t -mandoc $< >$@

dialogfetchmailmon.ps: dialogfetchmailmon.1
	groff -T ps -t -mandoc $< >$@

clean:
	rm -f *.1 *.ps

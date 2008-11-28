## Process this file with automake to produce Makefile.in

bin_PROGRAMS = fetchmailmond txtfetchmailmon 

BUILT_SOURCES = fetchmailmond_dbus_client_glue.h fetchmailmond_dbus_server_glue.h

EXTRA_DIST = fetchmailmond-infos.xml

fetchmailmond_dbus_client_glue.h: $(srcdir)/fetchmailmond-infos.xml
	dbus-binding-tool --mode=glib-client --output=$@ fetchmailmond-infos.xml

fetchmailmond_dbus_server_glue.h: $(srcdir)/fetchmailmond-infos.xml
	dbus-binding-tool --mode=glib-server --prefix=fetchmailmond --output=$@ fetchmailmond-infos.xml

fetchmailmond_SOURCES = \
	fetchmailmond.cc\
	fetchmailmon.h\
	fetchmailmond_dbus_server_glue.h\
	version.c\
	version.h\
	MailLogScanner.cc\
	MailLogScanner.h\
	ControllerDBusSimple.cc\
	ControllerDBusSimple.h\
	SyslogReader.cc\
	SyslogReader.h\
	SyntaxError.cc\
	SyntaxError.h

fetchmailmond_CXXFLAGS = ${CFLAGS} ${DBUS_CFLAGS}
fetchmailmond_LDADD = ${LIBS} ${DBUS_LIBS}

txtfetchmailmon_SOURCES = \
	txtfetchmailmon.c\
	fetchmailmond_dbus_client_glue.h\
	version.c\
	version.h
	
txtfetchmailmon_LDFLAGS = 
txtfetchmailmon_CFLAGS = ${CFLAGS} ${DBUS_CFLAGS}
txtfetchmailmon_LDADD = ${LIBS} ${DBUS_LIBS}

CLEAN_FILES = $(BUILT_SOURCES)

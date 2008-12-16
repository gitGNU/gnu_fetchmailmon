## Process this file with automake to produce Makefile.in

bin_PROGRAMS = fetchmailmond txtfetchmailmon 
check_PROGRAMS = txtmainloop

AM_CXXFLAGS = $(DBUS_CFLAGS)
AM_CFLAGS = $(DBUS_CFLAGS)
AM_LDFLAGS = $(DBUS_LIBS)

BUILT_SOURCES = 

EXTRA_DIST = fetchmailmond-infos.xml marshall.list

fetchmailmond_dbus_client_glue.h: $(srcdir)/fetchmailmond-infos.xml
	dbus-binding-tool --mode=glib-client --output=$@ fetchmailmond-infos.xml

fetchmailmond_dbus_server_glue.h: $(srcdir)/fetchmailmond-infos.xml
	dbus-binding-tool --mode=glib-server --prefix=fetchmailmond --output=$@ fetchmailmond-infos.xml

BUILT_SOURCES += fetchmailmond_dbus_client_glue.h fetchmailmond_dbus_server_glue.h

marshall.h: $(srcdir)/marshall.list
	glib-genmarshal --header marshall.list > $@

marshall.c: $(srcdir)/marshall.list
	glib-genmarshal --body marshall.list > $@

BUILT_SOURCES += marshall.c marshall.h

fetchmailmond_SOURCES = \
	fetchmailmond.cc\
	fetchmailmon.h\
	fetchmailmond_dbus_server_glue.h\
	version.c\
	version.h\
	MailLogScanner.cc\
	MailLogScanner.h\
	ControllerDBus.cc\
	ControllerDBus.h\
	SyntaxError.cc\
	SyntaxError.h

txtfetchmailmon_SOURCES = \
	txtfetchmailmon.c\
	fetchmailmond_dbus_client_glue.h\
	marshall.c\
	marshall.h\
	version.c\
	version.h
	
txtmainloop_SOURCES = txtmainloop.c

CLEAN_FILES = $(BUILT_SOURCES)

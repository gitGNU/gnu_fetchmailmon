## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES = -I@x_includes@ -I@wmapp_includes@


CFLAGS =\
	 -Wall -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment -Wuninitialized -Wparentheses -Wtraditional -Wpointer-arith -Wmissing-prototypes\
	 -O1\
	 -g

CXXFLAGS =\
	 -Wall -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment -Wuninitialized -Wparentheses -Wtraditional -Wpointer-arith -Wmissing-prototypes\
	 -O1\
	 -g

bin_PROGRAMS = txtfetchmailmon wmfetchmailmon

txtfetchmailmon_SOURCES = \
	txtfetchmailmon.cc\
	Controller.cc\
	Controller.h\
	ControllerText.cc\
	ControllerText.h\
	SyslogReader.cc\
	SyslogReader.h\
	SyntaxError.cc\
	SyntaxError.h

txtfetchmailmon_LDFLAGS = 

txtfetchmailmon_LDADD = 


wmfetchmailmon_SOURCES = \
	wmfetchmailmon.cc\
	Controller.cc\
	Controller.h\
	ControllerDockApp.cc\
	ControllerDockApp.h\
	SyslogReader.cc\
	SyslogReader.h\
	SyntaxError.cc\
	SyntaxError.h

wmfetchmailmon_LDFLAGS = 

wmfetchmailmon_LDADD = -L@wmapp_libraries@ -lwmapp -L@x_libraries@ -lX11 -lXext -lXpm


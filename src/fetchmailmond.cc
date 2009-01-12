/*
 *  Copyright (C) 2003 Guilhem Bonnefille <guilhem.bonnefille@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include <dbus/dbus.h>

#include <unistd.h>

#include <glib.h>
#include <glib/gi18n.h>

#include "version.h"
#include "Controller.h"
#include "ControllerDBus.h"
#include "MailLogScanner.h"

#include "fetchmailmon.h"

static GMainLoop *loop = NULL;

static MailLogScanner *scanner = NULL;

static gboolean opt_debug = FALSE;
static gboolean opt_verbose = FALSE;
static gboolean opt_version = FALSE;
const gchar *file = DEFAULT_MAILLOG_FILENAME;

static GOptionEntry entries[] = 
{
  { "debug", 'd', 0, G_OPTION_ARG_NONE, &opt_debug, N_("Enable debug output"), NULL },
  { "verbose", 'V', 0, G_OPTION_ARG_NONE, &opt_verbose, N_("Enable verbose output"), NULL },
  { "version", 'v', 0, G_OPTION_ARG_NONE, &opt_version, N_("Show version"), NULL },
  { NULL }
};

static gboolean opt_dbus_session = FALSE;
static DBusBusType dbus_bus_type = DBUS_BUS_SYSTEM;
static GOptionEntry dbus_entries[] = 
{
  { "session", '\0', 0, G_OPTION_ARG_NONE, &opt_dbus_session, N_("Use the session bus (instead of system bus)"), NULL },
  { NULL }
};

/* Convenience function to print an error and exit */
static void
die (const char *prefix, GError *error) 
{
  g_error("%s: %s", prefix, error->message);
  g_error_free (error);
  exit(EXIT_FAILURE);
}

static void
dbus_die (const char *prefix, DBusError *error) 
{
  g_error("%s: %s", prefix, error->message);
  dbus_error_free (error);
  exit(EXIT_FAILURE);
}

/**
 *@param argc number of arguments
 *@param argv arguments
 */
static void
processArgs(int argc, char *argv[])
{
  int i;
  GOptionContext *context = NULL;
  GOptionGroup *dbus_group = NULL;
  GError *error = NULL;

  context = g_option_context_new (_("- fetchmail monitor daemon"));
  // FIXME PACKAGE->GETTEXT_PACKAGE
  g_option_context_add_main_entries (context, entries, PACKAGE);

  dbus_group = g_option_group_new ("dbus", "DBus options", "Options specific to DBUS", NULL, NULL);
  g_option_group_add_entries (dbus_group, dbus_entries);
  g_option_context_add_group (context, dbus_group);
  dbus_group = NULL; // Managed by context

  // Parse command line
  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      gchar *help = g_option_context_get_help (context, FALSE, NULL);
      g_printerr ("option parsing failed: %s\n", error->message);
      g_printerr (help);
      g_free (help); help = NULL;
      exit (EXIT_FAILURE);
    }
    
  if (opt_version)
  {
    version(stdout);
    exit (EXIT_SUCCESS);
  }
  if (opt_dbus_session)
  {
    dbus_bus_type = DBUS_BUS_SESSION;
  }
  
  if (1 < argc)
    {
      file = argv[1];
    }

  i = 2;
  while (i < argc)
    {
      std::cerr << "Warning: argument ignored: "<<  argv[i]
           << std::endl;
      i++;
    }
}

DBusConnection*
init_dbus()
{
   static DBusObjectPathVTable fetchmailmon_vtable = {NULL, NULL, NULL, NULL, NULL, NULL };
   DBusError err;
   DBusConnection* conn = NULL;
   int ret;
   // initialise the errors
   dbus_error_init(&err);

   // connect to the bus
   conn = dbus_bus_get(dbus_bus_type, &err);
   if (dbus_error_is_set(&err)) { 
      dbus_die ("Connection Error", &err); 
   }
   dbus_error_init (&err);
   dbus_bus_request_name (conn, DBUS_FETCHMAILMON_IFACE, 0, &err);
   if (dbus_error_is_set(&err)) { 
      dbus_die ("Request name Error", &err); 
   }

   if (!dbus_connection_register_object_path(conn,  DBUS_FETCHMAILMON_PATH,
       					    &fetchmailmon_vtable, &err))
      dbus_die ("Registering error", &err);
      
  return conn;
}

static GIOChannel *
create_command_io( const char *filename )
{
	GIOChannel *result = NULL;
	gchar *command = NULL;
	gchar **argv = NULL;
	gint argc = 0;
	GPid child_pid;
	gint tail_stdout;
	GError *error = NULL;

    command = g_strdup_printf (TAIL_COMMAND, filename);
	
	g_debug ( "create_command_io: %s", command );
	if ( !g_shell_parse_argv ( command, &argc, &argv, &error ) )
		die("Parsing command line", error );
	
	g_free (command); command = NULL;
	
	if ( !g_spawn_async_with_pipes ( NULL, argv, NULL, (GSpawnFlags)0, NULL, NULL, &child_pid, NULL, &tail_stdout, NULL,
									 &error ) )
		die("Creating tail sub-process",error);
	
	g_strfreev ( argv ); argv = NULL;
	
	result = g_io_channel_unix_new ( tail_stdout );
	
	return result;
}

static gboolean
read_info (GIOChannel *source,
           GIOCondition condition,
           gpointer data)
{
	GIOStatus status;
	GString *buff = g_string_new ( NULL );
	gsize pos = 0;
	const gchar *filename = (const gchar *)data;
	GError *error = NULL;

    g_debug("read_info...");	
    status = g_io_channel_read_line_string (source, buff, &pos, &error );
    g_debug ("Status=%d", status);
    if ( G_IO_STATUS_ERROR == status )
        die("Reading line", error);
    if ( G_IO_STATUS_NORMAL == status ) {
        g_debug( "Read line on %s: %s", filename, buff->str );
        scanner->scanLine(buff->str);
    }
    g_debug("read_info done.");	
}

int
main(int argc, char *argv[])
{
   Controller *controller = NULL;
   DBusConnection* conn = NULL;
   GIOChannel *io_channel = NULL;

   loop = g_main_loop_new (NULL, FALSE);

   processArgs(argc, argv);   
      
   // Init DBUS layer
   conn = init_dbus();
   
   g_debug("Sleeping...");
   sleep(5);
   g_debug("Awaken");

  controller = new ControllerDBus(conn);

  scanner = new MailLogScanner();
  scanner->setController(controller);
  
  io_channel = create_command_io ( file );
  g_io_add_watch ( io_channel, G_IO_IN, read_info, (gpointer)file );

  g_main_loop_run(loop);
  
  return EXIT_SUCCESS;
}

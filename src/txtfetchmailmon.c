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

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <glib.h>
#include <glib/gi18n.h>

#include "marshall.h"
#include "fetchmailmond_dbus_client_glue.h"
#include "fetchmailmon.h"
#include "version.h"

static gboolean opt_debug = FALSE;
static gboolean opt_verbose = FALSE;
static gboolean opt_version = FALSE;

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

  context = g_option_context_new (_("- fetchmail monitor text client"));
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
  
  i = 1;
  while (i < argc)
    {
      g_printerr ("Warning: argument ignored: %s", argv[i]);
      i++;
    }
}

/* Convenience function to print an error and exit */
static void die (const char *prefix, GError *error) 
{
  g_error("%s: %s", prefix, error->message);
  g_error_free (error);
  exit(EXIT_FAILURE);
}

static GMainLoop *loop = NULL;

static void downloadedMessagesReceptionHandler(DBusGProxy* proxy,
				 const gchar *login,
				 const gchar *server,
				 int nbDownloadedMessages,
				 int nbMessages,
			 	 int percentMess,
				 int percentSize,
				 int size) 
{
  printf("%s@%s: %d of %d (%d%%), %d%% of %d octets\n",
         login, server,
         nbDownloadedMessages, nbMessages, percentMess,
         percentSize, size);
}

int
main(int argc, char *argv[])
{
  GError *error = NULL;
  DBusGConnection *connection;
  DBusGProxy *proxy;
  int ret_signal;

  g_type_init ();
  
  loop = g_main_loop_new (NULL, FALSE);

  processArgs(argc, argv);

  connection = dbus_g_bus_get (dbus_bus_type, &error);
  if (connection == NULL)
    die ("Failed to open connection to bus", error);

  proxy = dbus_g_proxy_new_for_name_owner (connection,
                                           DBUS_FETCHMAILMON_SERVICE,
                                           DBUS_FETCHMAILMON_PATH,
                                           DBUS_FETCHMAILMONSIMPLE_IFACE,
                                           &error);
  if (proxy == NULL)
    die ("Failed to create proxy for name owner", error);

  // add marshaller
  dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING_STRING_INT_INT_INT_INT_INT,
                                     G_TYPE_NONE,
                                     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                                     G_TYPE_INVALID);

  //attach to a signal
  dbus_g_proxy_add_signal(/* Proxy to use */
			  proxy,
			  /* Signal name */
			  "DownloadedMessages",
			  /* Will receive arguments */
			  G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
			  /* Termination of the argument list */
			  G_TYPE_INVALID);

  g_debug("Signal added.");
  
  dbus_g_proxy_connect_signal(/* Proxy object */
                              proxy,
                              /* Signal name */
                              "DownloadedMessages",
                              /* Signal handler to use. Note that the
                                 typecast is just to make the compiler
                                 happy about the function, since the
                                 prototype is not compatible with
                                 regular signal handlers. */
                              G_CALLBACK(downloadedMessagesReceptionHandler),
                              /* User-data (we don't use any). */
                              NULL,
                              /* GClosureNotify function that is
                                 responsible in freeing the passed
                                 user-data (we have no data). */
                              NULL);

  g_debug("Signal connected.");

  g_main_loop_run(loop);
  
  return EXIT_SUCCESS;
}

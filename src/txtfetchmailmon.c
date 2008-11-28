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

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "fetchmailmond_dbus_client_glue.h"
#include "fetchmailmon.h"
#include "version.h"

const char *file = NULL;

/// Print the usage
/// @param out file the usage must be printed.
/// @param progName the name of the program.
static void
usage(FILE *out, const char *progName)
{
  fprintf(out, "USAGE: %s [-v] [-h]\n", progName);
}

/**
 *@param argc number of arguments
 *@param argv arguments
 */
static void
processArgs(int argc, char *argv[])
{
  extern int optind;
  int c;
  int nbErrors = 0;

  while ((c = getopt(argc, argv, "vh")) != -1)
    {
      switch (c)
        {
        case 'v': // Version is requested
          version(stdout);
          exit(EXIT_SUCCESS);
          break;
        case 'h': // Help is requested
          usage(stdout, argv[0]);
          exit(EXIT_SUCCESS);
          break;
        case '?':
        default:
          nbErrors++;
          break;
        }
    }
  
  if (nbErrors > 0)
    {
      usage(stderr, argv[0]);
      exit(EXIT_FAILURE);
    }

  while (optind < argc)
    {
      fprintf(stderr, "Warning: argument ignored: %s\n", argv[optind]);
      optind++;
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
				 const gchar *server) 
{
  g_printf("downloadedMessagesReceptionHandler called with datas : \n");
  g_printf("%s\n", login);
  g_printf("%s\n", server);
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

  connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (connection == NULL)
    die ("Failed to open connection to bus", error);

  proxy = dbus_g_proxy_new_for_name_owner (connection,
                                           DBUS_FETCHMAILMON_SERVICE,
                                           DBUS_FETCHMAILMON_PATH,
                                           DBUS_FETCHMAILMONSIMPLE_IFACE,
                                           &error);
  if (proxy == NULL)
    die ("Failed to create proxy for name owner", error);


  //attach to a signal
  dbus_g_proxy_add_signal(/* Proxy to use */
			  proxy,
			  /* Signal name */
			  "DownloadedMessages",
			  /* Will receive arguments */
			  G_TYPE_STRING,
			  G_TYPE_STRING,
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

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

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <unistd.h>

#include <glib.h>
#include <glib/gi18n.h>

#include "version.h"
#include "Controller.h"
#include "ControllerText.h"
#include "MailLogScanner.h"

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

/* Convenience function to print an error and exit */
static void
die (const char *prefix, GError *error) 
{
  g_error("%s: %s", prefix, error->message);
  g_error_free (error);
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
  GError *error = NULL;

  context = g_option_context_new (_("- fetchmail monitor daemon"));
  // FIXME PACKAGE->GETTEXT_PACKAGE
  g_option_context_add_main_entries (context, entries, PACKAGE);
  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      gchar *help = g_option_context_get_help (context, TRUE, NULL);
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
  
  if (1 < argc)
    {
      file = argv[1];
    }

  i = 2;
  while (i < argc)
    {
      std::cerr << "Warning: argument ignored: "<<  argv[optind]
           << std::endl;
      i++;
    }
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
  Controller *controller = new ControllerText();
   GIOChannel *io_channel = NULL;

   loop = g_main_loop_new (NULL, FALSE);

  processArgs(argc, argv);

  scanner = new MailLogScanner();
  scanner->setController(controller);
  
  io_channel = create_command_io ( file );
  g_io_add_watch ( io_channel, G_IO_IN, read_info, (gpointer)file );

  g_main_loop_run(loop);
  
  return EXIT_SUCCESS;
}

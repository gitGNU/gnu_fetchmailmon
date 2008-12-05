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

#include <glib.h>
#include <glib/gi18n.h>

const char *file = NULL;

/// Print the usage
/// @param out file the usage must be printed.
/// @param progName the name of the program.
static void
usage(FILE *out, const char *progName)
{
  fprintf(out, "USAGE: %s [-v] [-h]\n", progName);
}


/* Convenience function to print an error and exit */
static void die (const char *prefix, GError *error) 
{
  g_error("%s: %s", prefix, error->message);
  g_error_free (error);
  exit(EXIT_FAILURE);
}

static GMainLoop *loop = NULL;

/* Options */
static gboolean vik_debug = FALSE;
static gboolean vik_verbose = FALSE;
static gboolean vik_version = FALSE;
static GOptionEntry entries[] = 
{
  { "debug", 'd', 0, G_OPTION_ARG_NONE, &vik_debug, N_("Enable debug output"), NULL },
  { "verbose", 'V', 0, G_OPTION_ARG_NONE, &vik_verbose, N_("Enable verbose output"), NULL },
  { "version", 'v', 0, G_OPTION_ARG_NONE, &vik_version, N_("Show version"), NULL },
  { NULL }
};

GIOChannel *
create_tailed_io( const char *filename )
{
	GIOChannel *result = NULL;
	const gchar *argv[] = { "/usr/bin/tail", "-F", filename, NULL };
	GPid child_pid;
	gint tail_stdout;
	GError *error = NULL;
	
	if ( !g_spawn_async_with_pipes ( NULL, argv, NULL, 0, NULL, NULL, &child_pid, NULL, &tail_stdout, NULL,
									 &error ) )
		die("Creating tail sub-process",error);
	
	result = g_io_channel_unix_new ( tail_stdout );
	
	return result;
}

gboolean
read_info (GIOChannel *source,
           GIOCondition condition,
           gpointer data)
{
	GIOStatus status;
	GString *buff = g_string_new ( NULL );
	gsize pos = 0;
	const gchar *filename = (const gchar *)data;
	GError *error = NULL;
	
	do {
		status = g_io_channel_read_line_string (source, buff, &pos, &error );
		if ( G_IO_STATUS_ERROR == status )
			die("Reading line", error);
		if ( G_IO_STATUS_NORMAL == status )
			g_print( "Read line on %s: %s", filename, buff->str );
	} while ( G_IO_CHANNEL_ERROR != status );
}

#define GETTEXT_PACKAGE "fetchmailmon"
int
main(int argc, char *argv[])
{
  GError *error = NULL;
  GOptionContext *context;
  GIOChannel *io_channel = NULL;

  context = g_option_context_new ("- test file reading");
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      g_print ("option parsing failed: %s\n", error->message);
      usage (stderr, argv[0] );
      exit (1);
    }

  loop = g_main_loop_new (NULL, FALSE);

  int i = 0;
  for (i=1 ; i < argc ; i++)
  {
    const gchar *filename = argv[i];
    io_channel = create_tailed_io ( filename );
    g_io_add_watch ( io_channel, G_IO_IN, read_info, filename );
  }

  g_main_loop_run(loop);
  
  return EXIT_SUCCESS;
}

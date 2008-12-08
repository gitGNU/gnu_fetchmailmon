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
static GList *filenames = NULL;
static GList *commands = NULL;

static gboolean
parse_filename (const gchar *option_name,
                const gchar *value,
                gpointer data,
                GError **error);
static gboolean
parse_command (const gchar *option_name,
               const gchar *value,
               gpointer data,
               GError **error);

static GOptionEntry entries[] = 
{
  { "debug", 'd', 0, G_OPTION_ARG_NONE, &vik_debug, N_("Enable debug output"), NULL },
  { "verbose", 'V', 0, G_OPTION_ARG_NONE, &vik_verbose, N_("Enable verbose output"), NULL },
  { "version", 'v', 0, G_OPTION_ARG_NONE, &vik_version, N_("Show version"), NULL },
  { "command", 'c', 0, G_OPTION_ARG_CALLBACK, parse_command, N_("Enable verbose output"), NULL },
  { "file", 'f', 0, G_OPTION_ARG_CALLBACK, parse_filename, N_("Show version"), NULL },
  { NULL }
};

static gboolean
parse_filename (const gchar *option_name,
                const gchar *value,
                gpointer data,
                GError **error)
{
    g_debug ( "parse_filename: %s", value );
	filenames = g_list_append ( filenames, g_strdup ( value ) );
}

static gboolean
parse_command (const gchar *option_name,
               const gchar *value,
               gpointer data,
               GError **error)
{
    g_debug ( "parse_filename: %s", value );
	commands = g_list_append ( commands, g_strdup ( value ) );
}

GIOChannel *
create_command_io( const char *command )
{
	GIOChannel *result = NULL;
	gchar **argv = NULL;
	gint argc = 0;
	GPid child_pid;
	gint tail_stdout;
	GError *error = NULL;
	
	g_debug ( "create_command_io: %s", command );
	if ( !g_shell_parse_argv ( command, &argc, &argv, &error ) )
		die("Parsing command line", error );
	
	if ( !g_spawn_async_with_pipes ( NULL, argv, NULL, 0, NULL, NULL, &child_pid, NULL, &tail_stdout, NULL,
									 &error ) )
		die("Creating tail sub-process",error);
	
	g_strfreev ( argv ); argv = NULL;
	
	result = g_io_channel_unix_new ( tail_stdout );
	
	return result;
}

GIOChannel *
create_file_io( const char *filename )
{
	GIOChannel *result = NULL;
	GError *error = NULL;
	
	g_debug ( "create_file_io: %s", filename );
	result = g_io_channel_new_file ( filename, "r", &error );
	if ( error )
		die("Opening file",error);
	
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
  GList *iter;

  context = g_option_context_new ("- test file reading");
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      g_print ("option parsing failed: %s\n", error->message);
      usage (stderr, argv[0] );
      exit (1);
    }

  loop = g_main_loop_new (NULL, FALSE);

  // Run over commands
  for (iter = commands ; iter != NULL ; iter = iter->next)
  {
    const gchar *input = iter->data;
    io_channel = create_command_io ( input );
    g_io_add_watch ( io_channel, G_IO_IN, read_info, input );
  } 

  // Run over commands
  for (iter = filenames ; iter != NULL ; iter = iter->next)
  {
    const gchar *input = iter->data;
    io_channel = create_file_io ( input );
    g_io_add_watch ( io_channel, G_IO_IN, read_info, input );
  }  	

  g_main_loop_run(loop);
  
  return EXIT_SUCCESS;
}

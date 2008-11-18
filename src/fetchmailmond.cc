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

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include <dbus/dbus.h>

#include <unistd.h>

#include "version.h"
#include "Controller.h"
#include "ControllerDBusSimple.h"
#include "MailLogScanner.h"

#include "fetchmailmon.h"

const char *file = NULL;

/// Print the usage
/// @param out file the usage must be printed.
/// @param progName the name of the program.
static void
usage(FILE *out, const char *progName)
{
  fprintf(out, "USAGE: %s [-v] [-h] [file]\n", progName);
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

  if (optind < argc)
    {
      file = argv[optind];
      optind++;
    }

  while (optind < argc)
    {
      std::cerr << "Warning: argument ignored: "<<  argv[optind]
           << std::endl;
      optind++;
    }
}

int
main(int argc, char *argv[])
{
  Controller *controller = NULL;
  MailLogScanner *scanner = NULL;
     DBusObjectPathVTable fetchmailmon_vtable = {NULL, NULL, NULL, NULL, NULL, NULL };
   DBusError err;
   DBusConnection* conn;
   int ret;
   // initialise the errors
   dbus_error_init(&err);

   // connect to the bus
   conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err)) { 
      fprintf(stderr, "Connection Error (%s)\n", err.message); 
      dbus_error_free(&err); 
   }
   if (NULL == conn) { 
      exit(1); 
   }
   dbus_error_init (&err);
   dbus_bus_request_name (conn, DBUS_FETCHMAILMON_IFACE, 0, &err);
   if (dbus_error_is_set(&err)) { 
      fprintf(stderr, "Connection Error (%s)\n", err.message); 
      dbus_error_free(&err); 
      exit(1); 
   }

   if (!dbus_connection_register_object_path(conn,  DBUS_FETCHMAILMON_PATH,
       					    &fetchmailmon_vtable, NULL))
      exit(1);
sleep(5);
  controller = new ControllerDBusSimple(conn);
  processArgs(argc, argv);

  if (file != NULL)
    scanner = new MailLogScanner(file, SyslogReader::FROM_BEGIN);
  else
    scanner = new MailLogScanner();

  scanner->setController(controller);

  while (1)
  {
    scanner->proceed();
    usleep(10000);
  }
  
  return EXIT_SUCCESS;
}

/*
 *  wmfetchmailmon is part of the FetchmailMon project.
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

#include <unistd.h>

#include "wmapp.h"

#include "version.h"
#include "MailLogScanner.h"
#include "ControllerDockApp.h"

const char *file = NULL;

/**
 * Timer fonction to check the input file.
 */
void
checkInput(const WMApp *a, void *param)
{
  MailLogScanner *scanner = (MailLogScanner *)param;
  scanner->proceed();
}

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
      cerr << "Warning: argument ignored: "<<  argv[optind]
           << endl;
      optind++;
    }
}

/**
 * Main
 */
int
main(int argc, char *argv[])
{
  // This should always be the very first line in the main() function of
  // a WMApplication:
  WMApp::initialize(argc, argv);

  ControllerDockApp dockapp;
  MailLogScanner *scanner;

  processArgs(argc, argv);

  if (file != NULL)
    scanner = new MailLogScanner(file, SyslogReader::FROM_BEGIN);
  else
    scanner = new MailLogScanner();

  scanner->setController(&dockapp);

  dockapp.create();

  dockapp.add_timed_function(10, checkInput, scanner);

  // This makes everything go.
  dockapp.run();

  // Any cleanup code after the window closes should go here.
  // (In this case, none is needed.)
  
  return 0;
}



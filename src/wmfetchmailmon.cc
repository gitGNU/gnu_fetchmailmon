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

#include "wmapp.h"

#include "SyslogReader.h"
#include "ControllerDockApp.h"

FILE *input = NULL;


/**
 * Timer fonction to check the input file.
 */
void
checkInput(const WMApp *a, void *param)
{
  SyslogReader *syslog = (SyslogReader *)param;
  char line[BUFSIZ + 1];	// read line

  if (fgets (line, BUFSIZ, input) != NULL)
    {
      syslog->scanLine(line);
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

  SyslogReader syslog;
  ControllerDockApp dockapp;

  syslog.setController(&dockapp);

  // For now, we simply use the stdin as input
  input = stdin;

  dockapp.create();

  dockapp.add_timed_function(10, checkInput, &syslog);

  // This makes everything go.
  dockapp.run();

  // Any cleanup code after the window closes should go here.
  // (In this case, none is needed.)
  
  return 0;
}



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

#include <stdio.h>

#include "version.h"

/// Print the version message
void version (FILE * out)
{
  // Print some version informations
#if (defined(PACKAGE) && defined(VERSION))
  (void) fprintf (out, "%s %s\n", PACKAGE, VERSION);
#else
  (void) fputs("No package-version available (due to generation).\n", out);
#endif

  // Copyright notice
  (void) fputs("Copyright (C) 2003 Guilhem Bonnefille\n"
               "This program comes with NO WARRANTY.\n"
               "You may redistribute copies of it under the terms of the\n"
               "GNU General Public License.\n"
               "For more information about these matters,\n"
               "see the files named COPYING.\n",
               out);
}

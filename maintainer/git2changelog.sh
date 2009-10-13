#!/bin/sh
#  Copyright (C) 2009 Guilhem Bonnefille <guilhem.bonnefille@free.fr>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Library General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# ChangeLog file generator
#
# This script parse the git history and format it like a ChangeLog file.
# This script is called at distribution tile (see "make dist").
#
git log --date=short "--format=%cd %an <%aE>:%n%x09* %s" "$@" \
| awk '/^[^\t]/{if(previous==$0)next;previous=$0}{print}' \
| sed '/^[^\t]/s/\([^ ]*\) \(.*\)/\n\1\n\2/'

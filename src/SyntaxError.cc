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

#include <cstdlib> 
#include <string>
#include <cstring>

using namespace std;

#include "SyntaxError.h"

const char *SyntaxError::PREFIX = "Syntax error: ";

const char *
SyntaxError::what()
  const throw()
{
  // First call: make the message
  if (_message == NULL)
    {
      int length = ::strlen(PREFIX) + _reason.size() + 1;
      _message = (char*)malloc(sizeof(char)*length);
      ::strcpy(_message, PREFIX);
      ::strcat(_message, _reason.c_str());
    }

  return _message;
}

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
#include <cctype>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

using namespace std;

#include "SyslogReader.h"

SyslogReader::SyslogReader(const char *filename, SyslogReader::StartPosition position)
  : _filename(filename), _file(NULL), _position(position), _lastReadLine("")
{}

SyslogReader::~SyslogReader()
{ closeFile(); }

const char *
SyslogReader::getLine()
{
  bool toRead = false;
  const char *result = NULL;

  _lastReadLine.assign("");

  toRead = toBeRead();
  if (toRead == true)
    {
      char buff[BUFSIZ];
      // Perhaps the line is not complete
      // (larger than the buff size).
      // So we read until a end-of-line char
      do
        {
          if (fgets(buff, BUFSIZ, _file) != NULL)        
            _lastReadLine.append(buff);
          else
            // We enforce an end-of-line to avoid an infinite loop
            _lastReadLine.append("\n");
        }
      while (_lastReadLine[_lastReadLine.size() - 1] != '\n');

      // Remove the end-of-line
      if (_lastReadLine[_lastReadLine.size() - 1] == '\n')
        _lastReadLine.erase(_lastReadLine.begin() + _lastReadLine.size() - 1);

      result = _lastReadLine.c_str();
    }

  return result;
}

bool
SyslogReader::toBeRead()
{
  if (_file == NULL)
    openFile();

  if (_file == NULL)
    // No file to read
    return false;

  struct stat statFile;
  if (fstat(fileno(_file), &statFile) != 0)
    {
      cerr << "Error(toBeRead().fstat()): "
           << strerror(errno) << endl;
      return false;
    }
  
  // Compare the sizes
  long readSize = ftell(_file);
  if (statFile.st_size > readSize)
    return true;
  else
    {
      // Perhaps the file has been rotated
      struct stat statOnDisk;
      if (stat(_filename.c_str(), &statOnDisk) != 0)
        {
          if (errno != ENOENT)
            return false;
          else
            {
              cerr << "Error(toBeRead().stat(" << _filename << ")): "
                   << strerror(errno) << endl;
              return false;
            }
        }

      // Compare if files are differents
      if (statOnDisk.st_ino != statFile.st_ino)
        {
          closeFile();
          openFile();
          return true;
        }
    }

  return false;
}

void
SyslogReader::openFile()
{
  static bool firstTime = true;

  _file = fopen(_filename.c_str(),"r");
  if (_file == NULL && errno != ENOENT)
    {
      cerr << "Error: openFile(): " << strerror(errno);
      exit(EXIT_FAILURE);
    }
  if (_file != NULL && firstTime == true)
    {
      firstTime = false;
      if (_position == SyslogReader::FROM_END)
        {
          fseek(_file, 0, SEEK_END);
        }
    }
}

void
SyslogReader::closeFile()
{
  if (_file != NULL)
    fclose(_file);

  _file = NULL;
}

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

#ifndef SYSLOG_READER_H
#define SYSLOG_READER_H

#include <string>

using namespace std;

/**
 * Reader for syslog file.
 *
 * It handles the possible 'rotation' of the file.
 */
class SyslogReader
{
 public:
  typedef enum {
    FROM_BEGIN, //< The file is read from its first character
    FROM_END    //< The file reading start after the current last character
  } StartPosition;

  SyslogReader(const char *filename, StartPosition position=FROM_END);

  ~SyslogReader();

  /**
   * Get the new line
   */
  const char *getLine();

 private:
  /**
   * Check if something need to be read.
   */
  bool toBeRead();

  /**
   * Open the file.
   */
  void openFile();

  /**
   * Close the file.
   */
  void closeFile();

  /**
   * The current filename to control.
   */
  string _filename;

  /**
   * The current file descriptor.
   */
  FILE* _file;

  /**
   * The required starting position.
   */
  StartPosition _position;

  /**
   * Last line read.
   */
  string _lastReadLine;
};

#endif /*  SYSLOG_READER_H */

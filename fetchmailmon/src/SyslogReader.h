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

#include <cstdlib>
#include <cstring>
#include "Controller.h"
 
/**
 * Generic controller for IHM.
 *
 * It defines the 'events' that can occurs.
 */
class SyslogReader
{
 public:
  SyslogReader()
    : _controller(NULL)
    {}
  ~SyslogReader(){}

  /**
   * Set a controller to inform.
   */
  void setController(Controller *controller)
    { _controller = controller; }

  /**
   * Scan a new line.
   *
   *@param line a complete line
   */
  void scanLine(const char *line);

  bool scanNewFetch(const char *line);
  bool scanNewMessage(const char *line);
  bool scanMessageFlushed(const char *line);

 private:

  /**
   * The current controller
   */
  Controller *_controller;
};

#endif /*  SYSLOG_READER_H */

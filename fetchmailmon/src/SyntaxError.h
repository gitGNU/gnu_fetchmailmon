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

#ifndef SYNTAX_ERROR_H
#define SYNTAX_ERROR_H

#include <string>
#include <stdexcept>

/**
 * Syntax error exception.
 */
class SyntaxError : public exception
{
 public:
  SyntaxError(const char *reason)
    : _reason(reason), _message(NULL)
    {}
  ~SyntaxError(){}

  const char *what() const;

 private:

  /**
   * Prefix of the message
   */
  static const char *const PREFIX = "Syntax error: ";

  /**
   * The reason
   */
  string _reason;

  /**
   * The message
   */
  mutable char *_message;
};

#endif /*  SYSLOG_READER_H */

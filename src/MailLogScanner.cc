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
#include <cctype>
#include <cstring>

using namespace std;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SyntaxError.h"
#include "MailLogScanner.h"

MailLogScanner::MailLogScanner()
  : _controller(NULL)
{}

/**
 * Scan a new line.
 *
 *@param line a complete line
 */
void
MailLogScanner::scanLine(const char *line)
{
  //------------------------------------------------
  // First, we have to find the begining of the message
  //------------------------------------------------
  char *begin = NULL;
  // Jump all the logging informations
  // Such informations are like the following:
  //Sep  8 18:58:12 hercules fetchmail[569]:
  begin = strchr(line, '[');
  if (begin == NULL)
    return;
  begin = strchr(begin, ':');
  if (begin == NULL)
    return;
  begin++;
  
  // Jump spaces
  while (isspace(*begin))
    begin++;

  bool ret;
  ret = scanNewFetch(begin);
  if (ret == false)
    ret = scanNewMessage(begin);
  if (ret == false)
    ret = scanMessageFlushed(begin);
}

bool
MailLogScanner::scanNewFetch(const char *line)
{
  // 21 messages for guilhem.bonnefille at Pop.free.fr (88123 octets).
  // 1 message for guilhem.bonnefille at Pop.free.fr (3242 octets). 

  int ret = 0;
  int number = 0;
  char login[BUFSIZ];
  char server[BUFSIZ];
  int size = 0;

  ret = sscanf(line, "%d %*s for %s at %s (%d octets).",
               &number, login, server, &size);

  if (ret == 4)
    {
      // Line is as expected
      _controller->newFetch(number, login, server, size);
      return true;
    }
  else
    return false;
}

bool
MailLogScanner::scanNewMessage(const char *line)
{
  // reading message guilhem.bonnefille@Pop.free.fr:21 of 32 (3019 octets)
  
  const char *READING_WORD = "reading";

  char login[BUFSIZ];
  char server[BUFSIZ];
  int index = 0; // Index of the current message
  int number = 0; // Number of messages to download
  int size = 0; // Size of the current message

  char *ptr = NULL;
  int ret = 0;

  // Check if the first word is the one expected
  ret = strncmp(line, READING_WORD, strlen(READING_WORD));
  if (ret == 0)
    {
      // The line is as expected
      // Collect all others informations
      // Jump 'reading' word
      line += strlen(READING_WORD);
      // Jump space
      line++;
      // Jump following word
      line = strchr(line, ' ');
      if (line == NULL)
        throw new SyntaxError("'reading message' log not supported.");
      line++;

      // The login
      ptr = strchr(line, '@');
      if (ptr == NULL)
        throw new SyntaxError("'reading message' log not supported.");
      strncpy(login, line, ptr - line);
      // Add the missing nul character
      login[ptr - line] = '\0';

      line = ptr + 1;

      // The server
      ptr = strchr(line, ':');
      if (ptr == NULL)
        throw new SyntaxError("'reading message' log not supported.");
      strncpy(server, line, ptr - line);
      // Add the missing nul character
      server[ptr - line] = '\0';

      line = ptr + 1;      

      // Numbers and sizes
      ret = sscanf(line, "%d of %d (%d octets)",
                   &index, &number, &size);
      if (ret != 3)
        throw new SyntaxError("'reading message' log not supported.");

      // Inform the controller
      _controller->newMessage(login, server, index, number, size);

      return true;
    }
  else
    return false;
}

bool
MailLogScanner::scanMessageFlushed(const char *line)
{  
  const char *FLUSH_WORD = "flushed";
  int ret = 0;

  // Check if the first word is the one expected
  ret = strncmp(line, FLUSH_WORD, strlen(FLUSH_WORD));
  if (ret == 0)
    {
      // The line is as expected

      // Inform the controller
      _controller->messageFlushed();

      return true;
    }
  else
    return false;
}

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

#include "ControllerText.h"

ControllerText::ControllerText()
  : _login(""), _server(""),
    _nbMessages(0), _size(0),
    _nbDownloadedMessages(0), _sizeDownloaded(0),
    _sizeCurrent(0)
{}

/**
 * A new fetch begin.
 *
 *@param number number of messages to retrieve
 *@param login login used
 *@param server server contacted
 *@param size size to download (in bytes)
 */
void
ControllerText::newFetch(int number,
                         const char *login,
                         const char *server,
                         int size)
{
  _login = login;
  _server = server;

  // Set the number of messages to download
  _nbMessages = number;
  _size = size;

  // Reset the number of downloaded messages
  _nbDownloadedMessages = 0;
  _sizeDownloaded = 0;

  printDownloadedMessages();
}

/**
 * A new message is comming.
 *
 *@param login login used
 *@param server server contacted
 *@param index index of the current message
 *@param number number of messages to retrieve on the current server
 *@param size size (in bytes) of the current message
 */
void
ControllerText::newMessage(const char *login,
                           const char *server,
                           int index,
                           int number,
                           int size)
{
  _login = login;
  _server = server;

  // Set the number of messages to download
  // Perhaps it has changed
  _nbMessages = number;

  _sizeCurrent = size;

  // Set the number of downloaded messages
  // NB: the current message is not downloaded
  _nbDownloadedMessages = index - 1;
}

/**
 * Previous message completly downloaded.
 */
void
ControllerText::messageFlushed()
{
  // Increment the number of downloaded messages
  _nbDownloadedMessages++;
  _sizeDownloaded += _sizeCurrent;

  // To avoid eroneous future usage, reset the current size
  _sizeCurrent = 0;

  printDownloadedMessages();
}


/**
 * Print the number of downloaded messages
 *
 */
void
ControllerText::printDownloadedMessages()
{
  int percentMess = 0;
  int percentSize = 0;

  // Compute percentage
  if (_nbMessages > 0)
    percentMess = _nbDownloadedMessages * 100 / _nbMessages;
  else
    percentMess = -1;

  if (_size > 0)
    percentSize = _sizeDownloaded * 100 / _size;
  else
    percentSize = -1;

  printf("%s@%s: %d of %d (%d%%), %d%% of %d octets\n",
         _login.c_str(), _server.c_str(),
         _nbDownloadedMessages, _nbMessages, percentMess,
         percentSize, _size);
}

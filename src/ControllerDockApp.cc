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

#include <string>
#include <sstream>

using namespace std;

#include "wmapp.h"
#include "wmwindow.h"
#include "wmframe.h"
#include "wmtextbar.h"
#include "wmmeterbar.h"

#include "ControllerDockApp.h"

ControllerDockApp::ControllerDockApp()
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
ControllerDockApp::newFetch(int number,
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

  // Refresh display
  refreshEmail();
  refreshNumber();
  refreshNumberBar();
  refreshSize();
  refreshSizeBar();

  // Request the application to repaint
  app.repaint();
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
ControllerDockApp::newMessage(const char *login,
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

  // Refresh display
  refreshEmail();
  refreshNumber();
  refreshNumberBar();
  refreshSize();
  refreshSizeBar();

  // Request the application to repaint
  app.repaint();
}

/**
 * Previous message completly downloaded.
 */
void
ControllerDockApp::messageFlushed()
{
  // Increment the number of downloaded messages
  _nbDownloadedMessages++;
  _sizeDownloaded += _sizeCurrent;

  // To avoid eroneous future usage, reset the current size
  _sizeCurrent = 0;

  // Refresh display
  refreshNumber();
  refreshNumberBar();
  refreshSize();
  refreshSizeBar();

  // Request the application to repaint
  app.repaint();
}


/**
 * refresh the text bar displaying the email processed.
 */
void
ControllerDockApp::refreshEmail()
{
  string emailString;
  emailString.assign(_login);
  emailString.append("@");
  emailString.append(_server);
  this->email.settext(emailString.c_str());
}

/**
 * refresh the text bar displaying the numbers of retrieved messages.
 */
void
ControllerDockApp::refreshNumber()
{
  ostringstream numberString;
  numberString << _nbDownloadedMessages
               << " of "
               << _nbMessages;
  this->number.settext(numberString.str().c_str());
}

/**
 * refresh the progress bar displaying the numbers of retrieved messages.
 */
void
ControllerDockApp::refreshNumberBar()
{
  numberbar.settotal(_nbMessages, false);
  numberbar.setvalue(_nbDownloadedMessages, true);
}

/**
 * refresh the text bar displaying the size retrieved.
 */
void
ControllerDockApp::refreshSize()
{
  ostringstream sizeString;
  sizeString << _sizeDownloaded
             << " of "
             << _size;
  this->size.settext(sizeString.str().c_str());
}

/**
 * refresh the progress bar displaying the size retrieved.
 */
void
ControllerDockApp::refreshSizeBar()
{
  sizebar.settotal(_size, false);
  sizebar.setvalue(_sizeDownloaded, true);
}

/**
 */
void
ControllerDockApp::create()
{
  win.addchild(frame);
  win.setorientation(Orientation::Vertical);
  win.setaspectratios();

  frame.setorientation(Orientation::Vertical);
  frame.setpadding(0);
  frame.setborder(0);

  email.setborder(0);
  frame.addchild(email);

  number.setborder(0);
  frame.addchild(number);

  numberbar.setorientation(Orientation::Horizontal);
  numberbar.setstyle(WMMeterBar::Spectrum);
  numberbar.settotal(50, true);
  numberbar.setborder(0);
  frame.addchild(numberbar);

  size.setborder(0);
  frame.addchild(size);

  sizebar.setorientation(Orientation::Horizontal);
  sizebar.setstyle(WMMeterBar::Spectrum);
  sizebar.settotal(50, true);
  sizebar.setborder(0);
  frame.addchild(sizebar);

  frame.setaspectratios();

  // Attach the window to the application.
  app.addwindow(win);
}

/**
 */
void
ControllerDockApp::run()
{
  app.run();
}

/**
 */
void
ControllerDockApp::add_timed_function(int period,
                                      WMCallback::data_func d,
                                      void *param)
{
  win.add_timed_function(period, d, param);
}



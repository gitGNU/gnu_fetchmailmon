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

#ifndef CONTROLLER_DOCKAPP_H
#define CONTROLLER_DOCKAPP_H

#include <string>

using namespace std;

#include "Controller.h"

#include "wmapp.h"
#include "wmcallback.h"
#include "wmwindow.h"
#include "wmframe.h"
#include "wmtextbar.h"
#include "wmmeterbar.h"
 
/**
 * DockApp controller.
 *
 * Displays the fetchmail monitoring informations on a DockApp (WindowMaker)
 */
class ControllerDockApp : public Controller
{
 public:
  ControllerDockApp();
  virtual ~ControllerDockApp(){}

  //+++++++++++++++++++
  // Controller methods
  //+++++++++++++++++++

  /**
   * A new fetch begin.
   *
   *@param number number of messages to retrieve
   *@param login login used
   *@param server server contacted
   *@param size size to download (in bytes)
   */
  virtual void newFetch(int number,
                        const char *login,
                        const char *server,
                        int size);

  /**
   * A new message is comming.
   *
   *@param login login used
   *@param server server contacted
   *@param index index of the current message
   *@param number number of messages to retrieve on the current server
   *@param size size (in bytes) of the current message
   */
  virtual void newMessage(const char *login,
                          const char *server,
                          int index,
                          int number,
                          int size);
  
  /**
   * Previous message completly downloaded.
   */
  virtual void messageFlushed();

  //++++++++++++++++
  // DockApp methods
  //++++++++++++++++

  /**
   * Create the DockApp.
   *
   * Init the widgets.
   */
  void create();

  /**
   * Run the DockApp.
   */
  void run();

  /**
   * Add a timed function.
   */
  void add_timed_function(int period, WMCallback::data_func d, void *param);

 protected:
  void refreshNumber();
  void refreshNumberBar();
  void refreshSize();
  void refreshSizeBar();

 private:
  /**
   * Number of messages to download.
   */
  int _nbMessages;

  /**
   * Size to download.
   */
  int _size;

  /**
   * Number of downloaded messages.
   */
  int _nbDownloadedMessages;

  /**
   * Size downloaded.
   */
  int _sizeDownloaded;

  /**
   * Size of the current message.
   */
  int _sizeCurrent;


  /**
   * The application.
   */
  WMApp app;

  /**
   * The unique window.
   */
  WMWindow win;

  /**
   * The unique frame.
   */
  WMFrame frame;

  /**
   * The text bar to display server processed.
   */
  WMTextBar server;

  /**
   * The text bar to display login processed.
   */
  WMTextBar login;

  /**
   * The text bar to display the current downloaded emails.
   */
  WMTextBar number;

  /**
   * The text bar to display the current downloaded size.
   */
  WMTextBar size;

  /**
   * The emails number progess bar.
   */
  WMMeterBar numberbar;

  /**
   * The size progress bar.
   */
  WMMeterBar sizebar;
};

#endif /* CONTROLLER_TEXT_H */

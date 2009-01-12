#!/usr/bin/python

def my_func(login, server):
    print login
    print server

import dbus, gobject
from dbus.mainloop.glib import DBusGMainLoop
dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()

bus.add_signal_receiver(my_func,
                        dbus_interface="org.fetchmail.FetchmailMonSimple",
                        signal_name="DownloadedMessages")

loop = gobject.MainLoop()
loop.run()

#include <cstdio>
#include <cstdlib>
#include <dbus/dbus.h>

int main(int argc, char *argv[])
{
   DBusError err;
   DBusConnection* conn;
   int ret;
   // initialise the errors
   dbus_error_init(&err);

   // connect to the bus
   conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err)) { 
      fprintf(stderr, "Connection Error (%s)\n", err.message); 
      dbus_error_free(&err); 
   }
   if (NULL == conn) { 
      exit(1); 
   }
}

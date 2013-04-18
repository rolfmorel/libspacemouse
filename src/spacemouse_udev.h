/*
Copyright (c) 2013 Rolf Morel

spacemouse - a free software driver for 3D/6DoF input devices.

spacemouse is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

spacemouse is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with spacemouse.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SPACEMOUSE_UDEV_H_
#define _SPACEMOUSE_UDEV_H_

#include "spacemouse.h"

enum {
  SPACEMOUSE_ACTION_IGNORE,
  SPACEMOUSE_ACTION_ADD,
  SPACEMOUSE_ACTION_REMOVE,
  SPACEMOUSE_ACTION_CHANGE,
  SPACEMOUSE_ACTION_ONLINE,
  SPACEMOUSE_ACTION_OFFLINE
};

struct spacemouse *spacemouse_devices(void);

struct spacemouse *spacemouse_devices_update(void);

int spacemouse_monitor_open(void);

struct spacemouse *spacemouse_monitor(int *action);

void spacemouse_monitor_close(void);

#endif

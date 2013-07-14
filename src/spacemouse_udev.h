/*
Copyright (c) 2013 Rolf Morel

libspacemouse - a free software driver for 3D/6DoF input devices.

libspacemouse is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libspacemouse is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libspacemouse.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SPACEMOUSE_UDEV_H_
#define _SPACEMOUSE_UDEV_H_

#include "spacemouse.h"

enum {
  SPACEMOUSE_ACTION_IGNORE,
  SPACEMOUSE_ACTION_ADD,
  SPACEMOUSE_ACTION_REMOVE
};

struct spacemouse *spacemouse_device_list(void);

struct spacemouse *spacemouse_device_list_update(void);

int spacemouse_monitor_open(void);

struct spacemouse *spacemouse_monitor(int *action);

void spacemouse_monitor_close(void);

#endif

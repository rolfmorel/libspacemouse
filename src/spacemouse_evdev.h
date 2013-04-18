/*
Copyright (c) 2013 Rolf Morel

libspacemouse - a free software driver for 3D/6DoF input devices.

libspacemouse is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libspacemouse is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libspacemouse.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SPACEMOUSE_EVDEV_H_
#define _SPACEMOUSE_EVDEV_H_

#include "spacemouse.h"

enum {
  SPACEMOUSE_READ_IGNORE,
  SPACEMOUSE_READ_SUCCESS,
  SPACEMOUSE_READ_BUFFERING
};

int spacemouse_device_open(struct spacemouse *mouse);

int spacemouse_device_grab(struct spacemouse *mouse);

int spacemouse_device_ungrab(struct spacemouse *mouse);

int spacemouse_device_read_event(struct spacemouse *mouse,
                                 spacemouse_event *event);

int spacemouse_device_get_led(struct spacemouse *mouse);

int spacemouse_device_set_led(struct spacemouse *mouse, int state);

void spacemouse_device_close(struct spacemouse *mouse);

#endif

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

#ifndef _TYPES_H_
#define _TYPES_H_

#include <sys/time.h>

#include "libspacemouse.h"

struct spacemouse_buf {
  struct spacemouse_event_motion motion;
  struct timeval time;
};

struct spacemouse {
  int id;
  int fd;

  char *devnode;

  char *manufacturer;
  char *product;

  struct spacemouse_buf buf;

  struct spacemouse *next;
};

#endif

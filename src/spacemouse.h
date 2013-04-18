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

#include <sys/time.h>

#ifndef _SPACEMOUSE_H_
#define _SPACEMOUSE_H_

enum {
  SPACEMOUSE_EVENT_ANY,
  SPACEMOUSE_EVENT_MOTION,
  SPACEMOUSE_EVENT_BUTTON
};

struct spacemouse_event_motion {
  int type;
  int x, y, z;
  int rx, ry, rz;
  unsigned int period;
};

struct spacemouse_event_button {
  int type;
  int press;
  int bnum;
};

typedef union spacemouse_event {
  int type;
  struct spacemouse_event_motion motion;
  struct spacemouse_event_button button;
} spacemouse_event;

struct spacemouse_buf {
  spacemouse_event event;
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

extern struct spacemouse *spacemouse_head;

#endif

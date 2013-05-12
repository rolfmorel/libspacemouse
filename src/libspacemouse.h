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

#ifndef _LIBSPACEMOUSE_H_
#define _LIBSPACEMOUSE_H_

#include <sys/time.h>

enum {
  SPACEMOUSE_EVENT_ANY,
  SPACEMOUSE_EVENT_MOTION,
  SPACEMOUSE_EVENT_BUTTON
};

enum {
  SPACEMOUSE_ACTION_IGNORE,
  SPACEMOUSE_ACTION_ADD,
  SPACEMOUSE_ACTION_REMOVE,
  SPACEMOUSE_ACTION_CHANGE,
  SPACEMOUSE_ACTION_ONLINE,
  SPACEMOUSE_ACTION_OFFLINE
};

enum {
  SPACEMOUSE_READ_IGNORE,
  SPACEMOUSE_READ_SUCCESS
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

#ifdef __cplusplus
extern "C" {
#endif

/* Returns the head of the internal spacemouse list, use
 * spacemouse_devices_update() first to get current devices.
 */
struct spacemouse *spacemouse_devices(void);

/* Uses udev to discover devices and update the internal spacemouse list. When
 * the moniter functions are used this should only be needed to collect an
 * initial list. Returns head of internal spacemouse_list.
 */
struct spacemouse *spacemouse_devices_update(void);

/* Opens an udev connection and adds filter to input devices.
 * Returns file descriptor.
 */
int spacemouse_monitor_open(void);

/* Wraps udev's moniter connection. action will be one of SPACEMOUSE_ACTION_*,
 * or -1 in case of error. If action is ADD returned pointer is the new device
 * in the spacemouse list. If action is REMOVE returned pointer is to the
 * cached spacemouse structure which was removed from the spacemouse list. The
 * cached spacemouse structure can be used for comparison, and is only valid
 * until the next device removal. Blocks on read, use select, poll, etc.
 */
struct spacemouse *spacemouse_monitor(int *action);

/* Closes the udev moniter connection.
 */
void spacemouse_monitor_close(void);

/* Opens the device node in /dev for the mouse device, uses mouse->devnode.
 * Sets mouse->fd and returns the new file descriptor or -1 in case of error:
 * not able to open due to permissions, etc.
 */
int spacemouse_device_open(struct spacemouse *mouse);

int spacemouse_device_grab(struct spacemouse *mouse);

int spacemouse_device_ungrab(struct spacemouse *mouse);

/* Wraps evdev's protocol. Reads on mouse->fd (it blocks on read, use select,
 * poll, etc.). Returns one of SPACEMOUSE_READ_*. IGNORE when receiving an
 * unuseful event. event is only valid on SUCCESS. Returns -1 on error: file
 * descriptor closed, read error, etc.
 */
int spacemouse_device_read_event(struct spacemouse *mouse,
                                 spacemouse_event *event);

/* Get state of led of mouse device. 1 if on, 0 if off, or -1 in case of
 * error.
 */
int spacemouse_device_get_led(struct spacemouse *mouse);

/* Set state of led of mouse device. Return -1 in case of error.
 */
int spacemouse_device_set_led(struct spacemouse *mouse, int state);

/* Close the mouse->fd. Closes the file descriptor and sets mouse->fd to -1.
 */
void spacemouse_device_close(struct spacemouse *mouse);

#ifdef __cplusplus
}
#endif

#endif

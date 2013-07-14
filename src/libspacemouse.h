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

#ifndef _LIBSPACEMOUSE_H_
#define _LIBSPACEMOUSE_H_

enum {
  SPACEMOUSE_EVENT_ANY,
  SPACEMOUSE_EVENT_MOTION,
  SPACEMOUSE_EVENT_BUTTON
};

enum {
  SPACEMOUSE_ACTION_IGNORE,
  SPACEMOUSE_ACTION_ADD,
  SPACEMOUSE_ACTION_REMOVE
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

struct spacemouse;

#ifdef __cplusplus
extern "C" {
#endif

/* Returns the head of the internal spacemouse list, use
 * spacemouse_device_list_update() at least once to initialize list.
 */
struct spacemouse *spacemouse_device_list(void);

/* Discover devices and initialize/update the internal spacemouse list. When
 * the monitor functions are used this should only be needed to collect an
 * initial list. Returns head of internal spacemouse list.
 */
struct spacemouse *spacemouse_device_list_update(void);

/* Returns next node in the spacemouse list, this is needed to cycle over all
 * connected devices. Returns NULL when there are no more new nodes.
 */
struct spacemouse *spacemouse_device_list_get_next(struct spacemouse *mouse);

#define spacemouse_device_list_foreach(list_iter, list_head) \
  for (list_iter = list_head; \
       list_iter != NULL; \
       list_iter = spacemouse_device_list_get_next(list_iter))

/* Opens a connection to system device manager (udev with filter to input
 * devices on Linux). Returns file descriptor.
 */
int spacemouse_monitor_open(void);

/* Wraps system device manager connection. action will be one of
 * SPACEMOUSE_ACTION_*, or -1 in case of error. If action is ADD returned
 * pointer is the new device in the spacemouse list. If action is REMOVE
 * returned pointer is to the cached spacemouse structure which was removed
 * from the spacemouse list. The cached spacemouse structure is only valid
 * until the next device removal. Blocks on read, use select, poll, etc.
 */
struct spacemouse *spacemouse_monitor(int *action);

/* Closes the system device manager connection.
 */
void spacemouse_monitor_close(void);

/* Opens the device node in /dev for the mouse device.
 * Sets mouse's file descriptor and returns it or -1 in case of error.
 */
int spacemouse_device_open(struct spacemouse *mouse);

int spacemouse_device_grab(struct spacemouse *mouse);

int spacemouse_device_ungrab(struct spacemouse *mouse);

/* Wraps system HID protocol (evdev on Linux). Reads on mouse's file
 * descriptor (it blocks on read, use select, poll, etc.). Returns one of
 * SPACEMOUSE_READ_*. IGNORE when receiving an unuseful event. event is only
 * valid on SUCCESS. Returns -1 on error.
 */
int spacemouse_device_read_event(struct spacemouse *mouse,
                                 spacemouse_event *event);

/* Returns unique id for mouse device. The id that a device gets assigned is
 * an incremented number relative to previously assigned devices.
 * Ids start at integer value 1.
 */
int spacemouse_device_get_id(struct spacemouse *mouse);

/* Returns file descriptor that was opened by spacemouse_device_open().
 * Unopened/closed devices have their the file descriptor set to -1.
 */
int spacemouse_device_get_fd(struct spacemouse *mouse);

/* Returns character string which is the path to the associated node in /dev.
 * This string is only valid as long as the device is still connected.
 */
char const * const spacemouse_device_get_devnode(struct spacemouse *mouse);

/* Returns character string which is the manufacturer string associated with
 * the device. This string is only valid as long as the device is still
 * connected.
 */
char const * const spacemouse_device_get_manufacturer(
    struct spacemouse *mouse);

/* Returns character string which is the product string associated with the
 * device. This string is only valid as long as the device is still connected.
 */
char const * const spacemouse_device_get_product(struct spacemouse *mouse);

/* Get state of led of mouse device. 1 if on, 0 if off, or -1 in case of
 * error.
 */
int spacemouse_device_get_led(struct spacemouse *mouse);

/* Set state of led of mouse device. Return -1 in case of error.
 */
int spacemouse_device_set_led(struct spacemouse *mouse, int state);

/* Close the mouse's file descriptor and set it internally to -1.
 */
void spacemouse_device_close(struct spacemouse *mouse);

#ifdef __cplusplus
}
#endif

#endif

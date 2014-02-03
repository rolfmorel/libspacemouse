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

/** @file */

#ifndef _LIBSPACEMOUSE_H_
#define _LIBSPACEMOUSE_H_

enum spacemouse_event_type {
  SPACEMOUSE_EVENT_MOTION = 1,
  SPACEMOUSE_EVENT_BUTTON = 2,
  SPACEMOUSE_EVENT_LED = 4
};

enum spacemouse_action {
  SPACEMOUSE_ACTION_IGNORE,
  SPACEMOUSE_ACTION_ADD,
  SPACEMOUSE_ACTION_REMOVE
};

enum spacemouse_read_result {
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

struct spacemouse_event_led {
  int type;
  int state;
};

typedef union spacemouse_event {
  int type;
  struct spacemouse_event_motion motion;
  struct spacemouse_event_button button;
  struct spacemouse_event_led led;
} spacemouse_event_t;

/**
 * Opaque structure representing a spacemouse device
 */
struct spacemouse;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get first device in list and initialize/update the internal device list.
 *
 * When the monitor functions are used this function should only be called once
 * to initialize the internal list.
 *
 * This functions should at least once be called with update argument set to 1.
 *
 * @param mouse_ptr Pointer pointer which will be set to point to the first
 * device in list on successfull return.
 * @param update Set to 0 to only return the current head of the device list,
 * or 1 to first initialize/update the device list.
 *
 * @return 0 on success or negative errno on errro.
 */
int
spacemouse_device_list(struct spacemouse **mouse_ptr, int update);

/**
 * Returns next node in the device list.
 *
 * Call this function successively to iterate over all devices in the internal
 * list.
 *
 * @param mouse The device of which the next device is to be returned.
 *
 * @return Next device in internal list or NULL when there are no further
 * devices.
 */
struct spacemouse *
spacemouse_device_list_get_next(struct spacemouse *mouse);

/**
 * Macro which expands to a convient for loop to iterate over the device list.
 */
#define spacemouse_device_list_foreach(list_iter, list_head) \
  for (list_iter = list_head; \
       list_iter != NULL; \
       list_iter = spacemouse_device_list_get_next(list_iter))

/**
 * Open a connection to system device manager.
 *
 * On Linux open a connection to udev with a filter to input devices.
 *
 * @return File descriptor of the opened connection or -1 in case of error.
 */
int
spacemouse_monitor_open(void);

/**
 * Wrap system device manager connection.
 *
 * Wrap all reads necessary to monitor device connects and disconnects.
 *
 * When a device connects return the new device in the device list.
 * When a device disconnects return a cached device which is now removed from
 * the device list.
 *
 * The cached device is only valid until the next device removal.
 *
 * @param[out] mouse_ptr Pointer pointer which on return code is ADD, is set to
 * point to the new device in the spacemouse list. If return code is REMOVE, is
 * set to point to the cached device. If return code is IGNORE, value is
 * undefined and should be ignored.
 *
 * @return One of SPACEMOUSE_ACTION_*, or -1 in case of error.
 *
 * @note Blocks on read, use select, poll, etc.
 */
enum spacemouse_action
spacemouse_monitor(struct spacemouse **mouse_ptr);

/**
 * Close the system device manager connection.
 *
 * @return 0 on succes or -1 in case of error.
 */
int
spacemouse_monitor_close(void);

/**
 * Open the device node for the device.
 *
 * Set device's file descriptor.
 *
 * @param mouse The device who's device node is to be opened.
 *
 * @return device's opened file descriptor or -1 in case of error.
 */
int
spacemouse_device_open(struct spacemouse *mouse);

/**
 * Wrap system HID protocol.
 *
 * On Linus uses evdev, the kernel's input-subsystem.
 *
 * Wrap all reads on device's file descriptor.
 *
 * @param mouse The device of which the file descriptor is to be read for new
 * events.
 * @param[out] event Event which on SUCCESS is set to it's new value and
 * otherwise is undefined.
 *
 * @return One of SPACEMOUSE_READ_*. IGNORE when receiving an unuseful event,
 * -1 on error.
 *
 * @note Blocks on read, use select, poll, etc.
 */
enum spacemouse_read_result
spacemouse_device_read_event(struct spacemouse *mouse,
                             spacemouse_event_t *event);

/**
 * Return unique id of device.
 *
 * The id that a device gets assigned is an incremented number relative to
 * previously assigned devices.
 * Ids start at integer value 1.
 *
 * @param mouse The device of which the id is to be returned.
 *
 * @return The device's unique id.
 */
int
spacemouse_device_get_id(struct spacemouse *mouse);

/**
 * Return file descriptor that was opened by spacemouse_device_open(mouse).
 *
 * @param mouse The device of which the file descriptor is to be returned.
 *
 * @return The device's file descriptor or -1 in case no valid file descriptor
 * is set.
 *
 * @note Unopened/closed devices have their the file descriptor set to -1.
 */
int
spacemouse_device_get_fd(struct spacemouse *mouse);

/**
 * Return path of the device node of the device.
 *
 * @param mouse The device of which the device node path is to be returned.
 *
 * @return The character string which holds the path of the mouse device or
 * NULL when the path is not available.
 *
 * @note The returned string is only valid as long as the device is still
 * connected.
 */
char const * const
spacemouse_device_get_devnode(struct spacemouse *mouse);

/**
 * Return manufacturer string of the device.
 *
 * @param mouse The device of which the manufacturer string is to be returned.
 *
 * @return The character string which holds manufacturer name of the mouse
 * device or NULL when the name is not available.
 *
 * @note The returned string is only valid as long as the device is still
 * connected.
 */
char const * const
spacemouse_device_get_manufacturer(struct spacemouse *mouse);

/**
 * Return product string of the device.
 *
 * @param mouse The device of which the product string is to be returned.
 *
 * @return The character string which holds product name of the mouse device or
 * NULL when the name is not available.
 *
 * @note The returned string is only valid as long as the device is still
 * connected.
 */
char const * const
spacemouse_device_get_product(struct spacemouse *mouse);

/**
 * Return maximux deviation possible on a axis, valid for all axes of device.
 *
 * @param mouse The device of which the maximum deviation is to be returned.
 *
 * @return The maximum deviation for all axes of device, or -1 in case of
 * error, i.e. device not opened.
 */
int
spacemouse_device_get_max_axis_deviation(struct spacemouse *mouse);

/**
 * Set grab status of device.
 *
 * On Linux requests to be exclusive resipient of events of device on the evdev
 * interface.
 *
 * @param mouse The device of which the grab status is to be set.
 * @param grab The status to which the device it to be set, 1 for grab, 0 for
 * release grab.
 *
 * @return 0 on success, -1 in case of error.
 */
int
spacemouse_device_set_grab(struct spacemouse *mouse, int grab);

/**
 * Get state of led of device.
 *
 * @param mouse The device of which the led state is to be returned.
 *
 * @return 1 if on, 0 if off, or -1 in case of error.
 */
int
spacemouse_device_get_led(struct spacemouse *mouse);

/**
 * Set state of led of mouse device.
 *
 * @param mouse The device of which the led state is to be set.
 * @param state The state to which the led is to be set, 1 for on, 0 for off.
 *
 * @return 0 on success, or -1 in case of error.
 */
int
spacemouse_device_set_led(struct spacemouse *mouse, int state);

/**
 * Close the device's file descriptor.
 *
 * And set it internally to -1.
 *
 * @param mouse The device of which the file descriptor is to be closed.
 *
 * @return 0 on success, or -1 in case of error.
 */
int
spacemouse_device_close(struct spacemouse *mouse);

/**
 * Associate a pointer to data with a device.
 *
 * @param mouse The device with which the data is to be associated.
 * @param data The pointer to be set for later retrieval.
 *
 * @note Data pointers are not "freed" on device removal. In case of a pointer
 * to (unique) heap memory the pointer needs to manually freed by using the
 * monitor functionaillty.
 */
void
spacemouse_device_set_data(struct spacemouse *mouse, void *data);

/**
 * Retrieve a previously asscociated data pointer.
 *
 * @param mouse The device of which the associated data is to be returned.
 *
 * @return The pointer previously associated or when no data has been set NULL.
 */
void *
spacemouse_device_get_data(struct spacemouse *mouse);

#ifdef __cplusplus
}
#endif

#endif

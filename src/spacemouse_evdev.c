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

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "spacemouse_evdev.h"

#ifdef MAP_AXIS_SPACENAVD
/* Map axis the same way spacenavd/libspnav does by default. */
static const int map_axis[] = { 0, 2, 1, 3, 5, 4 };
static const int map_invert[] = { 1, -1, -1, 1, -1, -1 };
#endif

int spacemouse_device_open(struct spacemouse *mouse)
{
  int fd;

  if ((fd = open(mouse->devnode, O_RDWR)) == -1) {
     if ((fd = open(mouse->devnode, O_RDONLY)) == -1) {
      return -1;
    }
  }

  return (mouse->fd = fd);
}

int spacemouse_device_read_event(struct spacemouse *mouse,
                                 spacemouse_event *mouse_event)
{
  struct input_event input_event;
  ssize_t bytes;
  int ret = -1, type = -1, axis_idx, axis_code, invert = 1;

  while (ret == -1) {

    do {
      bytes = read(mouse->fd, &input_event, sizeof input_event);
    } while (bytes == -1 && errno == EINTR);

    if (bytes < sizeof input_event || errno == ENODEV)
      return -1;

    switch (input_event.type) {
      case EV_REL:
      case EV_ABS:
        axis_code = (input_event.type == EV_REL) ? REL_X : ABS_X;

        type = mouse->buf.motion.type = SPACEMOUSE_EVENT_MOTION;
#ifndef MAP_AXIS_SPACENAVD
        axis_idx = input_event.code - axis_code;
#else
        axis_idx = map_axis[input_event.code - axis_code];
        invert = map_invert[input_event.code - axis_code];
#endif

        (&mouse->buf.motion.x)[axis_idx] = invert * input_event.value;
        break;

      case EV_KEY:
        type = mouse_event->type = SPACEMOUSE_EVENT_BUTTON;
        mouse_event->button.bnum = input_event.code - BTN_0;
        mouse_event->button.press = input_event.value;
        break;

      case EV_LED:
        if (input_event.code == LED_MISC) {
          type = mouse_event->type = SPACEMOUSE_EVENT_LED;
          mouse_event->led.state = input_event.value;
        }
        break;

      case EV_SYN:
        if (type == SPACEMOUSE_EVENT_MOTION) {
          memcpy(mouse_event, &mouse->buf.motion, sizeof *mouse_event);
          if (mouse->buf.time.tv_sec != 0)
            mouse_event->motion.period = ((input_event.time.tv_sec * 1000 +
                                           input_event.time.tv_usec / 1000) -
                                          (mouse->buf.time.tv_sec * 1000 +
                                           mouse->buf.time.tv_usec / 1000));

          mouse->buf.time = input_event.time;
          mouse->buf.motion.type = 0;
        } else if (type != SPACEMOUSE_EVENT_BUTTON &&
                   type != SPACEMOUSE_EVENT_LED) {
          ret = SPACEMOUSE_READ_IGNORE;
          break;
        }

        ret = SPACEMOUSE_READ_SUCCESS;
        break;

      default:
        ret = SPACEMOUSE_READ_IGNORE;
        break;
    }

  }

  return ret;
}

int spacemouse_device_set_grab(struct spacemouse *mouse, int grab)
{
  if (grab == 0 || grab == 1)
    return ioctl(mouse->fd, EVIOCGRAB, grab ? (void *)1 : (void *)0);
  return -1;
}

int spacemouse_device_get_led(struct spacemouse *mouse)
{
  unsigned char state[(LED_MAX / 8) + 1];

  memset(state, 0, sizeof state);

  if (ioctl(mouse->fd, EVIOCGLED(LED_MAX), state) == -1)
    return -1;

  return (1UL << LED_MISC & ((unsigned long *) state)[0]) != 0;
}

int spacemouse_device_set_led(struct spacemouse *mouse, int state)
{
  struct input_event input_events[2];

  input_events[0].type = EV_LED;
  input_events[0].code = LED_MISC;
  input_events[0].value = state;

  input_events[1].type = EV_SYN;
  input_events[1].code = SYN_REPORT;

  if (write(mouse->fd, input_events, sizeof input_events) !=
      sizeof input_events)
    return -1;

  return 0;
}

int spacemouse_device_close(struct spacemouse *mouse)
{
  int ret = close(mouse->fd);

  mouse->fd = -1;

  return ret;
}

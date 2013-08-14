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

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "libspacemouse.h"
#include "types.h"

#define LONG_BITS (sizeof(long) * 8)
#define NLONGS(x) (((x) + LONG_BITS - 1) / LONG_BITS)

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
  struct input_event ev;
  ssize_t bytes;
  int ret = -1, type = -1, axis_idx, axis_code, invert = 1;

  while (ret == -1) {

    do {
      bytes = read(mouse->fd, &ev, sizeof ev);
    } while (bytes == -1 && errno == EINTR);

    if (bytes < sizeof ev || errno == ENODEV)
      return -1;

    switch (ev.type) {
      case EV_REL:
      case EV_ABS:
        axis_code = (ev.type == EV_REL) ? REL_X : ABS_X;

        type = mouse->buf.motion.type = SPACEMOUSE_EVENT_MOTION;
#ifndef MAP_AXIS_SPACENAVD
        axis_idx = ev.code - axis_code;
#else
        axis_idx = map_axis[ev.code - axis_code];
        invert = map_invert[ev.code - axis_code];
#endif

        (&mouse->buf.motion.x)[axis_idx] = invert * ev.value;
        break;

      case EV_KEY:
        type = mouse_event->type = SPACEMOUSE_EVENT_BUTTON;
        mouse_event->button.bnum = ev.code - BTN_0;
        mouse_event->button.press = ev.value;
        break;

      case EV_LED:
        if (ev.code == LED_MISC) {
          type = mouse_event->type = SPACEMOUSE_EVENT_LED;
          mouse_event->led.state = ev.value;
        }
        break;

      case EV_SYN:
        if (type == SPACEMOUSE_EVENT_MOTION) {
          memcpy(mouse_event, &mouse->buf.motion, sizeof *mouse_event);
          if (mouse->buf.time.tv_sec != 0)
            mouse_event->motion.period = ((ev.time.tv_sec * 1000 +
                                           ev.time.tv_usec / 1000) -
                                          (mouse->buf.time.tv_sec * 1000 +
                                           mouse->buf.time.tv_usec / 1000));

          mouse->buf.time = ev.time;
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

int spacemouse_device_get_max_axis_deviation(struct spacemouse *mouse)
{
  unsigned long bits[NLONGS(EV_CNT)];

  if (ioctl(mouse->fd, EVIOCGBIT(0, EV_MAX), bits) < 0)
    return -1;

  if (1UL << (EV_ABS % LONG_BITS) & bits[EV_ABS / LONG_BITS]) {
    int axis, val = -1;

    if (ioctl(mouse->fd, EVIOCGBIT(EV_ABS, sizeof(bits)), bits) < 0)
      return -1;

    /* Make sure the max deviation of all axes, both positive and negative,
     * have the same value. */
    for (axis = ABS_X; axis <= ABS_RZ; axis++) {
      if (1UL << (axis % LONG_BITS) & bits[axis / LONG_BITS]) {
        struct input_absinfo absinfo;

        if (ioctl(mouse->fd, EVIOCGABS(axis), &absinfo) < 0)
          return -1;

        if (axis == ABS_X)
          val = absinfo.maximum;

        if (val != absinfo.maximum || -val != absinfo.minimum)
          return -1;
      }
    }
    return val;
  } else if (1UL << (EV_REL % LONG_BITS) & bits[EV_REL / LONG_BITS])
    /* the kernel probably does not "fixing up rel/abs in Logitech report
     * descriptor." The default value for 3Dconnexion devices should be 500. */
     return 500;

  return -1;
}

int spacemouse_device_set_grab(struct spacemouse *mouse, int grab)
{
  if (grab == 0 || grab == 1)
    return ioctl(mouse->fd, EVIOCGRAB, grab ? (void *)1 : (void *)0);
  return -1;
}

int spacemouse_device_get_led(struct spacemouse *mouse)
{
  unsigned long bits[NLONGS(LED_CNT)] = { 0 };

  if (ioctl(mouse->fd, EVIOCGLED(LED_MAX), bits) == -1)
    return -1;

  return (1UL << (LED_MISC % LONG_BITS) & bits[LED_MISC / LONG_BITS]) != 0;
}

int spacemouse_device_set_led(struct spacemouse *mouse, int state)
{
  struct input_event ev[2];

  ev[0].type = EV_LED;
  ev[0].code = LED_MISC;
  ev[0].value = state;

  ev[1].type = EV_SYN;
  ev[1].code = SYN_REPORT;

  if (write(mouse->fd, ev, sizeof ev) != sizeof ev)
    return -1;

  return 0;
}

int spacemouse_device_close(struct spacemouse *mouse)
{
  int ret = close(mouse->fd);

  mouse->fd = -1;

  return ret;
}

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

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "spacemouse_evdev.h"

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

int spacemouse_device_grab(struct spacemouse *mouse)
{
  int grab = 1;

  if (!mouse->fd > -1)
    return -1;

  return ioctl(mouse->fd, EVIOCGRAB, &grab);
}

int spacemouse_device_ungrab(struct spacemouse *mouse)
{
  int grab = 0;

  if (!mouse->fd > -1)
    return -1;

  return ioctl(mouse->fd, EVIOCGRAB, &grab);
}

int spacemouse_device_read_event(struct spacemouse *mouse,
                                 spacemouse_event *mouse_event)
{
  struct input_event input_event;
  ssize_t bytes;
  int ret = SPACEMOUSE_READ_IGNORE;
  int *int_ptr;
  unsigned int period;

  if (!(mouse->fd > -1))
    return -1;

  do {
    bytes = read(mouse->fd, &input_event, sizeof input_event);
  } while (bytes == -1 && errno == EINTR);

  if (bytes < sizeof input_event)
    return -1;

  switch (input_event.type) {
    case EV_REL:
      mouse->buf.event.type = SPACEMOUSE_EVENT_MOTION;
      int_ptr = &(mouse->buf.event.motion.x);
      int_ptr[input_event.code - REL_X] = input_event.value;

      ret = SPACEMOUSE_READ_BUFFERING;
      break;

    case EV_ABS:
      mouse->buf.event.type = SPACEMOUSE_EVENT_MOTION;
      int_ptr = &(mouse->buf.event.motion.x);
      int_ptr[input_event.code - ABS_X] = input_event.value;

      ret = SPACEMOUSE_READ_BUFFERING;
      break;

    case EV_KEY:
      mouse->buf.event.type = SPACEMOUSE_EVENT_BUTTON;
      mouse->buf.event.button.bnum = input_event.code - BTN_0;
      mouse->buf.event.button.press = input_event.value;

      ret = SPACEMOUSE_READ_BUFFERING;
      break;

    case EV_SYN:
        if (mouse->buf.event.type == 0)
          break;
        memcpy(mouse_event, &mouse->buf.event, sizeof *mouse_event);
        mouse->buf.event.type = 0;

        if (mouse_event->type == SPACEMOUSE_EVENT_MOTION) {
          if (mouse->buf.time.tv_sec != 0) {
            period = ((input_event.time.tv_sec * 1000 +
                       input_event.time.tv_usec / 1000) -
                      (mouse->buf.time.tv_sec * 1000 +
                       mouse->buf.time.tv_usec / 1000));
            mouse_event->motion.period = period;
          }

          mouse->buf.time = input_event.time;
        }

        ret = SPACEMOUSE_READ_SUCCESS;
      break;

    default:
        ret = SPACEMOUSE_READ_IGNORE;
      break;
  }
  return ret;
}

int spacemouse_device_get_led(struct spacemouse *mouse)
{
  unsigned char state[(LED_MAX / 8) + 1];

  if (!(mouse->fd > -1))
    return -1;

  memset(state, 0, sizeof state);

  if (ioctl(mouse->fd, EVIOCGLED(LED_MAX), state) == -1)
    return -1;

  return (1UL << LED_MISC & ((unsigned long *) state)[0]) != 0;
}

int spacemouse_device_set_led(struct spacemouse *mouse, int state)
{
  struct input_event input_event;

  if (!(mouse->fd > -1))
    return -1;

  memset(&input_event, 0, sizeof input_event);
  input_event.type = EV_LED;
  input_event.code = LED_MISC;
  input_event.value = state;

  if (write(mouse->fd, &input_event, sizeof input_event) == -1) {
    return -1;
  }

  return 0;
}

void spacemouse_device_close(struct spacemouse *mouse)
{
  if (!(mouse->fd > -1))
    return;

  close(mouse->fd);

  mouse->fd = -1;
}

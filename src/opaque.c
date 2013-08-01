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

#include "libspacemouse.h"
#include "types.h"

struct spacemouse *spacemouse_device_list_get_next(struct spacemouse *mouse) {
  return mouse->next;
}

int spacemouse_device_get_id(struct spacemouse *mouse) {
  return mouse->id;
}

int spacemouse_device_get_fd(struct spacemouse *mouse) {
  return mouse->fd;
}

char const * const spacemouse_device_get_devnode(struct spacemouse *mouse) {
  return mouse->devnode;
}

char const * const spacemouse_device_get_manufacturer(
    struct spacemouse *mouse) {
  return mouse->manufacturer;
}

char const * const spacemouse_device_get_product(struct spacemouse *mouse) {
  return mouse->product;
}

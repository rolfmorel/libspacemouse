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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <libudev.h>

#include "spacemouse_udev.h"

static struct spacemouse *add_device(char const *devnode,
                                     char const *manufacturer,
                                     char const *product);
static void remove_device(struct spacemouse *mouse,
                          struct spacemouse *mouse_buf);

struct spacemouse *spacemouse_head = NULL;

static int new_device_id = 1;

static struct udev *udev = NULL;
static struct udev_monitor *udev_monitor = NULL;

static struct spacemouse *device_in_use(char const *devnode)
{
  struct spacemouse *iter = spacemouse_head;

  while (iter != NULL) {
    if (strcmp(iter->devnode, devnode) == 0)
      return iter;
    iter = iter->next;
  }

  return NULL;
}

static struct spacemouse *add_device(char const *devnode,
                                     char const *manufacturer,
                                     char const *product)
{
  struct spacemouse *mouse, *iter = spacemouse_head;

  if ((mouse = malloc(sizeof *mouse)) == NULL)
    return NULL;

  mouse->id = new_device_id++;
  mouse->fd = -1;

  if ((mouse->devnode = malloc(strlen(devnode) + 1)) == NULL) {
    free(mouse); return NULL;
  }
  strcpy(mouse->devnode, devnode);

  if ((mouse->manufacturer = malloc(strlen(manufacturer) + 1)) == NULL) {
    free(mouse->devnode); free(mouse); return NULL;
  }
  strcpy(mouse->manufacturer, manufacturer);

  if ((mouse->product = malloc(strlen(product) + 1)) == NULL) {
    free(mouse->devnode); free(mouse->manufacturer); free(mouse); return NULL;
  }
  strcpy(mouse->product, product);

  memset(&mouse->buf, 0, sizeof mouse->buf);

  mouse->next = NULL;

  if ((iter = spacemouse_head) == NULL)
    return (spacemouse_head = mouse);
  while (iter->next != NULL)
    iter = iter->next;

  return (iter->next = mouse);
}

struct spacemouse *spacemouse_devices(void)
{
  return spacemouse_head;
}

/* TODO: this function only adds new devices and doesn't remove devices udev
 * no longer lists */
struct spacemouse *spacemouse_devices_update(void)
{
  struct udev *udev;
  struct udev_enumerate *enumerate;
  struct udev_list_entry *devices, *dev_list_entry;
  struct udev_device *dev, *dev_parent;
  char const *syspath, *devnode, *attr_man, *attr_pro;

  /* add error check */
  udev = udev_new();

  enumerate = udev_enumerate_new(udev);
  udev_enumerate_add_match_subsystem(enumerate, "input");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);

  udev_list_entry_foreach(dev_list_entry, devices) {
    syspath = udev_list_entry_get_name(dev_list_entry);
    dev = udev_device_new_from_syspath(udev, syspath);

    devnode = udev_device_get_devnode(dev);

    dev_parent = udev_device_get_parent_with_subsystem_devtype(dev, "usb",
                                                               "usb_device");
    if (dev_parent != NULL &&
        devnode != NULL && strstr(devnode, "event") != NULL) {
      attr_man = udev_device_get_sysattr_value(dev_parent, "manufacturer");
      attr_pro = udev_device_get_sysattr_value(dev_parent, "product");
      if (attr_man != NULL && strcmp(attr_man, "3Dconnexion") == 0 &&
          attr_pro != NULL) {
        if (!device_in_use(devnode))
            add_device(devnode, attr_man, attr_pro);
      }
    }

    /* dev_parent is unreferenced/cleaned with child device */
    udev_device_unref(dev);
  }

  udev_enumerate_unref(enumerate);
  udev_unref(udev);

  return spacemouse_head;
}

static void remove_device(struct spacemouse *mouse,
                          struct spacemouse *mouse_buf)
{
  struct spacemouse *iter = spacemouse_head;

  if (iter == NULL)
    return;

  if (iter == mouse) {
    spacemouse_head = mouse->next;

    if (mouse_buf != NULL)
      memcpy(mouse_buf, mouse, sizeof *mouse_buf);
    else {
      if (mouse->fd > -1) close(mouse->fd);
      free(mouse->devnode); free(mouse->manufacturer);
      free(mouse->product); free(mouse);
    }
    return;
  }

  while(iter->next) {
    if (iter->next == mouse) {
      iter->next = iter->next->next;

      if (mouse_buf != NULL)
        memcpy(mouse_buf, mouse, sizeof *mouse_buf);
      else {
        if (mouse->fd > -1) close(mouse->fd);
        free(mouse->devnode); free(mouse->manufacturer);
        free(mouse->product); free(mouse);
      }
      return;
    }
  }
}

int spacemouse_monitor_open(void)
{
  if (!udev_monitor) {
    /* add error check */
    if (!udev)
      udev = udev_new();

    /* add error check */
    udev_monitor = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "input",
                                                    NULL);
    udev_monitor_enable_receiving(udev_monitor);
  }

  return udev_monitor_get_fd(udev_monitor);
}

struct spacemouse *spacemouse_monitor(int *action)
{
  static struct spacemouse cache_mouse = { -1, -1, NULL, NULL, NULL };
  struct spacemouse *mouse = NULL;
  struct udev_device *dev, *dev_parent;
  char const *devnode, *action_str, *attr_man, *attr_pro;

  *action = -1;

  if (!udev_monitor)
    return NULL;

  dev = udev_monitor_receive_device(udev_monitor);
  if (dev) {
    *action = SPACEMOUSE_ACTION_IGNORE;

    devnode = udev_device_get_devnode(dev);

    dev_parent = udev_device_get_parent_with_subsystem_devtype(dev,
        "usb", "usb_device");

    attr_man = udev_device_get_sysattr_value(dev_parent, "manufacturer");
    attr_pro = udev_device_get_sysattr_value(dev_parent, "product");

    if (devnode != NULL && strstr(devnode, "event") != NULL &&
        dev_parent != NULL) {
      mouse = device_in_use(devnode);

      action_str = udev_device_get_action(dev);
      if (strcmp(action_str, "add") == 0 &&
          mouse == NULL && attr_pro != NULL &&
          attr_man != NULL && strcmp(attr_man, "3Dconnexion") == 0) {
        mouse = add_device(devnode, attr_man, attr_pro);

        *action = SPACEMOUSE_ACTION_ADD;
      } else if (strcmp(action_str, "remove") == 0 && mouse != NULL) {
        free(cache_mouse.devnode); free(cache_mouse.manufacturer);
        free(cache_mouse.product);

        /* Cache the spacemouse structure about to be removed so we can return
         * a reference to the cached structure. */
        remove_device(mouse, &cache_mouse);
        cache_mouse.next = NULL;
        mouse = &cache_mouse;

        *action = SPACEMOUSE_ACTION_REMOVE;
      } else if (strcmp(action_str, "change") == 0)
        *action = SPACEMOUSE_ACTION_CHANGE;
      else if (strcmp(action_str, "online") == 0)
        *action = SPACEMOUSE_ACTION_ONLINE;
      else if (strcmp(action_str, "offline") == 0)
        *action = SPACEMOUSE_ACTION_OFFLINE;
    }

    /* dev_parent is unreferenced/cleaned with child device */
    udev_device_unref(dev);
  }

  return mouse;
}

void spacemouse_monitor_close(void)
{
  if (udev_monitor)
    udev_monitor_unref(udev_monitor);
  if (udev)
    udev_unref(udev);
}
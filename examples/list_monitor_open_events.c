#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <libspacemouse.h>

int main()
{
  struct spacemouse *iter, *mon_mouse;
  spacemouse_event mouse_event;
  fd_set fds;
  int monitor_fd, max_fd;
  int action, read_event;

  monitor_fd = spacemouse_monitor_open();

  iter = spacemouse_devices_update();

  if (iter == NULL)
    printf("No devices found.\n");

  while (iter) {
    printf("device id: %d\n", iter->id);
    printf("  devnode: %s\n", iter->devnode);
    printf("  manufacturer: %s\n", iter->manufacturer);
    printf("  product: %s\n", iter->product);

    if (spacemouse_device_open(iter) == -1)
      printf("Failed to open device: %s\n", iter->devnode);

    spacemouse_device_set_led(iter, 1);

    iter = iter->next;
  }

  printf("Entering monitor loop.\n");
  while(1) {
    FD_ZERO(&fds);
    FD_SET(monitor_fd, &fds);
    max_fd = monitor_fd;

    iter = spacemouse_devices();
    while (iter) {
      if (iter->fd > -1) {
        FD_SET(iter->fd, &fds);
        if (iter->fd > max_fd) max_fd = iter->fd;
      }
      iter = iter->next;
    }

    if (select(max_fd + 1, &fds, NULL, NULL, NULL) == -1) {
      perror("select");
      break;
    }

    if (FD_ISSET(monitor_fd, &fds)) {
      mon_mouse = spacemouse_monitor(&action);

      if (action == SPACEMOUSE_ACTION_ADD) {
        printf("Device added, device id: %d\n", mon_mouse->id);

        printf("  devnode: %s\n", mon_mouse->devnode);
        printf("  manufacturer: %s\n", mon_mouse->manufacturer);
        printf("  product: %s\n", mon_mouse->product);

        spacemouse_device_open(mon_mouse);

        spacemouse_device_set_led(mon_mouse, 1);
      } else if (action == SPACEMOUSE_ACTION_REMOVE) {
        printf("Device removed, device id: %d\n", mon_mouse->id);

        printf("  devnode: %s\n", mon_mouse->devnode);
        printf("  manufacturer: %s\n", mon_mouse->manufacturer);
        printf("  product: %s\n", mon_mouse->product);

        spacemouse_device_close(mon_mouse);
      }
    }

    iter = spacemouse_devices();
    while (iter) {
      if (iter->fd > -1 && FD_ISSET(iter->fd, &fds)) {
        memset(&mouse_event, 0, sizeof mouse_event);

        read_event = spacemouse_device_read_event(iter, &mouse_event);
        if (read_event == -1)
          spacemouse_device_close(iter);
        else if (read_event == SPACEMOUSE_READ_SUCCESS) {
          printf("device id %d: ", iter->id);

          if(mouse_event.type == SPACEMOUSE_EVENT_MOTION) {
            printf("got motion event: t(%d, %d, %d) ",
                   mouse_event.motion.x, mouse_event.motion.y,
                   mouse_event.motion.z);
            printf("r(%d, %d, %d) period(%d)\n",
                   mouse_event.motion.rx, mouse_event.motion.ry,
                   mouse_event.motion.rz, mouse_event.motion.period);
          } else if (mouse_event.type == SPACEMOUSE_EVENT_BUTTON) {
            printf("got button %s event b(%d)\n",
                   mouse_event.button.press ? "press" : "release",
                   mouse_event.button.bnum);
          }
        }
      }

      iter = iter->next;
    }
  }

  return 0;
}
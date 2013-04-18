#include <stdio.h>
#include <stdlib.h>

#include <libspacemouse.h>

int main()
{
  struct spacemouse *iter, *mon_mouse;
  int monitor_fd;
  fd_set fds;
  int action;

  monitor_fd = spacemouse_monitor_open();

  iter = spacemouse_devices_update();

  if (iter == NULL)
    printf("No devices found.\n");

  while (iter) {
    printf("device id: %d\n", iter->id);
    printf("  devnode: %s\n", iter->devnode);
    printf("  manufacturer: %s\n", iter->manufacturer);
    printf("  product: %s\n", iter->product);

    iter = iter->next;
  }

  printf("Entering monitor loop.\n");
  while(1) {
    FD_ZERO(&fds);
    FD_SET(monitor_fd, &fds);

    if (select(monitor_fd + 1, &fds, NULL, NULL, NULL) == -1) {
      perror("select");
      break;
    }

    mon_mouse = spacemouse_monitor(&action);

    if (action == -1)
      break;
    else if (action == SPACEMOUSE_ACTION_ADD) {
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

  /* infinite loop breaks only on recieved error */
  fprintf(stderr, "Recieved error from spacemouse_monitor\n");

  return 0;
}

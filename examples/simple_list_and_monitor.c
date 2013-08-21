#include <stdio.h>
#include <stdlib.h>

#include <libspacemouse.h>

int main()
{
  struct spacemouse *iter, *mon_mouse;
  int action;

  spacemouse_monitor_open();

  spacemouse_device_list(&iter, 1);
  if (iter == NULL)
    printf("No devices found.\n");

  spacemouse_device_list_foreach(iter, iter) {
    printf("device id: %d\n", spacemouse_device_get_id(iter));
    printf("  devnode: %s\n", spacemouse_device_get_devnode(iter));
    printf("  manufacturer: %s\n", spacemouse_device_get_manufacturer(iter));
    printf("  product: %s\n", spacemouse_device_get_product(iter));
  }

  printf("Entering monitor loop.\n");
  while(1) {
    action = spacemouse_monitor(&mon_mouse);

    if (action < 0)
      break;
    else if (action == SPACEMOUSE_ACTION_ADD) {
      printf("Device added, ");

      spacemouse_device_open(mon_mouse);

      spacemouse_device_set_led(mon_mouse, 1);
    } else if (action == SPACEMOUSE_ACTION_REMOVE) {
      printf("Device removed, ");

      spacemouse_device_close(mon_mouse);
    }

    if (action == SPACEMOUSE_ACTION_ADD ||
        action == SPACEMOUSE_ACTION_REMOVE) {
      printf("device id: %d\n", spacemouse_device_get_id(mon_mouse));
      printf("  devnode: %s\n", spacemouse_device_get_devnode(mon_mouse));
      printf("  manufacturer: %s\n",
             spacemouse_device_get_manufacturer(mon_mouse));
      printf("  product: %s\n", spacemouse_device_get_product(mon_mouse));
    }
  }

  /* infinite loop breaks only on recieved error */
  fprintf(stderr, "Recieved error from spacemouse_monitor\n");

  return 0;
}

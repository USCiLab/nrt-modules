#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define JOY_DEV "/dev/input/js1"

int main()
{
  printf("Joystick test...\n");

  int joy_fd, *axis=NULL, num_axes=0, num_buttons=0, x;
  char *button=NULL, joy_name[80];
  struct js_event js;

  if ((joy_fd = open(JOY_DEV, O_RDONLY)) == -1)
  {
    printf("Couldn't open joystick\n");
    return -1;
  }

  ioctl(joy_fd, JSIOCGAXES, &num_axes);
  ioctl(joy_fd, JSIOCGBUTTONS, &num_buttons);
  ioctl(joy_fd, JSIOCGNAME(80), &joy_name);

  axis = (int*) calloc(num_axes, sizeof(int));
  button = (char*) calloc(num_buttons, sizeof(char));

  printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n", joy_name, num_axes, num_buttons);

  fcntl(joy_fd, F_SETFL, O_NONBLOCK);

  while (1)
  {
    read(joy_fd, &js, sizeof(struct js_event));

    switch (js.type & ~JS_EVENT_INIT)
    {
      case JS_EVENT_AXIS:
        axis[js.number] = js.value;
        break;

      case JS_EVENT_BUTTON:
        button[js.number] = js.value;
        break;
    }

    printf("X: %6d  Y: %6d  ", axis[0], axis[1]);

    if (num_axes > 2)
      printf("Z: %6d  ", axis[2]);

    if (num_axes > 3)
      printf("R: %6d  ", axis[3]);

    for (x = 0; x < num_buttons; x++)
      printf("B%d: %d  ", x, button[x]);

    printf("\n");
    fflush(stdout);
  }
  close(joy_fd);
  return 0;
}

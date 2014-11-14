#include "rpc+.h"
#include <dirent.h>

bool gpio_get_gpio_count(int *gpio_count)
{
  /* get the number of all gpios by enumerating all gpio chips */
  DIR *dir = opendir("/sys/class/gpio");

}
//bool gpio_get_gpios(int* gpios, int max_count);
//bool gpio_export(int gpio);
//bool gpio_unexport(int gpio);
//bool gpio_set_value(int gpio, bool value);
//bool gpio_get_value(int gpio, bool *value);
//typedef enum { in, out } gpio_direction_t;
//bool gpio_set_direction(int gpio, gpio_direction_t direction);
//bool gpio_get_direction(int gpio, gpio_direction_t *direction);


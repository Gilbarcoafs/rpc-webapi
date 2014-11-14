#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "rpc+.h"

#define dir_gpio "/sys/class/gpio"
#define dir_name_gpiochip "gpiochip"
#define dir_name_ngpio "ngpio"

bool gpio_get_gpio_count(int *gpio_count)
{
  /* get the number of all gpios by enumerating all gpio chips */
  DIR *dir;
  struct dirent *entry;
  FILE* file;
  int bytes_read_count, i, count = 0;
  char filepath[255], buffer[255];

  memset(buffer, 0, sizeof(buffer));

  /* open the gpio directory */
  dir = opendir(dir_gpio);

  if (dir != NULL)
  {
    while((entry = readdir(dir)) != NULL)
    {
      /* look for gpiochip directories... */
      if (strncmp(entry->d_name, dir_name_gpiochip, strlen(dir_name_gpiochip)) == 0)
      {
        /* try to read their ngpio file */
        snprintf(filepath, sizeof(filepath), "%s/%s/%s", dir_gpio, entry->d_name, dir_name_ngpio);
        //printf("ngpio: %s\n", filepath);
        file = fopen(filepath, "r");
        if (file != NULL)
        {
          /* read the file */
          bytes_read_count = fread(buffer, sizeof(char), sizeof(buffer)-1, file);

          if (bytes_read_count > 0)
            count += atoi(buffer);

          /* close the file */
          fclose(file);
        }
      }
    }

    /* close the dir again */
    closedir(dir);
  }

  *gpio_count = count;
  return count != 0;
}

//bool gpio_get_gpios(int* gpios, int max_count);
//bool gpio_export(int gpio);
//bool gpio_unexport(int gpio);
//bool gpio_set_value(int gpio, bool value);
//bool gpio_get_value(int gpio, bool *value);
//typedef enum { in, out } gpio_direction_t;
//bool gpio_set_direction(int gpio, gpio_direction_t direction);
//bool gpio_get_direction(int gpio, gpio_direction_t *direction);


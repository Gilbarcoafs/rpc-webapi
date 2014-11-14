#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "rpc+.h"

#define dir_gpio "/sys/class/gpio"
#define dir_name_gpiochip "gpiochip"
#define dir_name_ngpio "ngpio"
#define dir_name_base "base"
#define file_export "/sys/class/gpio/export"
#define file_unexport "/sys/class/gpio/unexport"
#define str_in "in"
#define str_out "out"

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

int gpio_get_gpios(int* gpios, int max_count)
{
   /* get the number of all gpios by enumerating all gpio chips */
  DIR *dir;
  struct dirent *entry;
  FILE* file;
  int bytes_read_count, i, ngpio = 0, base = 0, index = 0;
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
        /* try to read their file 'ngpio' */
        snprintf(filepath, sizeof(filepath), "%s/%s/%s", dir_gpio, entry->d_name, dir_name_ngpio);
        if ((file = fopen(filepath, "r")) != NULL)
        {
          if ((bytes_read_count = fread(buffer, sizeof(char), sizeof(buffer)-1, file)) > 0) /* read the ngpio file */
            ngpio = atoi(buffer);
          fclose(file);
        }
        else
          ngpio = -1;

        /* try to read the file 'base' */
        snprintf(filepath, sizeof(filepath), "%s/%s/%s", dir_gpio, entry->d_name, dir_name_base); /*  */
        if ((file = fopen(filepath, "r")) != NULL)
        {
          if (fread(buffer, sizeof(char), sizeof(buffer)-1, file) > 0)
            base = atoi(buffer);
          fclose(file);
        }
        else
          base = -1;

        if (ngpio != -1 && base != -1)
        {
          for (i= 0; i < max_count && i < ngpio; i++)
            gpios[index++] = base + i;
        }
      }
    }

    /* close the dir again */
    closedir(dir);
  }

  return index;
}

static bool write_data_to_file(const char *filepath, const char *data, int count)
{
  bool ret = false;
  FILE* file = fopen(filepath, "r+w");

  if (file != NULL)
  {
    fwrite(data, sizeof(char), count, file);
    fclose(file);
    ret = true;
  }
  return ret;
}

static bool write_string_to_file(const char *filepath, const char *string)
{
  write_data_to_file(filepath, string, strlen(string));
}

static bool write_int_to_file(const char *filepath, int value)
{
  int count;
  char buffer[255];
  memset(buffer, 0, sizeof(buffer));
  count = snprintf(buffer, sizeof(buffer)-1, "%i", value);
  return write_data_to_file(filepath, buffer, count);
}

bool gpio_export(int gpio)
{
  return write_int_to_file(file_export, gpio);
}

bool gpio_unexport(int gpio)
{
  return write_int_to_file(file_unexport, gpio);
}

bool gpio_set_direction(int gpio, gpio_direction_t direction)
{
  char file[255];
  memset(file, 0, sizeof(file));
  snprintf(file, sizeof(file)-1, dir_gpio "/gpio%i/direction", gpio);
  return write_string_to_file(file, (direction == gpio_direction_in) ? str_in : str_out);
}

bool gpio_get_direction(int gpio, gpio_direction_t *direction)
{
  char buffer[255];
  char filepath[255];
  FILE *file;
  bool ret = false;

  memset(filepath, 0, sizeof(filepath));
  snprintf(filepath, sizeof(filepath)-1, dir_gpio "/gpio%i/direction", gpio);

  if ((file = fopen(filepath, "r")) != NULL)
  {
    if (fread(buffer, sizeof(char), sizeof(buffer), file) > 0)
    {
      if (strncmp(str_in, buffer, strlen(str_in)) == 0)
      {
        *direction = gpio_direction_in;
        ret = true;
      }
      else if (strncmp(str_out, buffer, strlen(str_out)) == 0)
      {
        *direction = gpio_direction_out;
        ret = true;
      }
    }
  }
  return ret;
}

bool gpio_set_value(int gpio, bool value)
{
  char file[255];
  memset(file, 0, sizeof(file));
  snprintf(file, sizeof(file)-1, dir_gpio "/gpio%i/value", gpio);
  return write_int_to_file(file, value ? 1 : 0);
}

bool gpio_get_value(int gpio, bool *value)
{
  char buffer[1];
  char filepath[255];
  bool ret = false;
  FILE *file;

  memset(filepath, 0, sizeof(filepath));
  snprintf(filepath, sizeof(filepath)-1, dir_gpio "/gpio%i/value", gpio);

  if ((file = fopen(filepath, "r")) != NULL)
  {
    if (fread(buffer, sizeof(char), sizeof(buffer), file) > 0)
    {
      if (strncmp("1", buffer, 1) == 0)
      {
        *value = true;
        ret = true;
      }
      else if (strncmp("0", buffer, 1) == 0)
      {
        *value= false;
        ret = true;
      }
    }
  }

  return ret;
}


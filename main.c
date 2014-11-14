#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mongoose.h"
#include "rpc+.h"

typedef struct {
  bool is_debug;
} config_t;

static config_t m_config = { true };

#define uri_gpio_get           "/gpio/get"
#define uri_gpio_export        "/gpio/export"
#define uri_gpio_unexport      "/gpio/unexport"
#define uri_gpio_value_get     "/gpio/value/get"
#define uri_gpio_value_set     "/gpio/value/set"
#define uri_gpio_direction_get "/gpio/direction/get"
#define uri_gpio_direction_set "/gpio/direction/set"

#define is_request(req) (strncmp(conn->uri, req, sizeof(req)) == 0)

static int get_query_param(const char *param, const char *query, char *value, int max_count)
{
  int i=0, value_index = 0;
  int param_length = strlen(param);
  /* is it a valid query string? */

  if (query != NULL)
  {
    /* search within the query string */
    while (query[i] != '\0')
    {
      /* if the query starts with the parameter */
      if (i == 0 || query[i-1] == '&')
        if(strncmp(query+i, param, param_length) == 0)
        {
          /* if the param is followed by a '=' */
          if (query[i+param_length] == '=')
          {
            /* copy the value */
            for (value_index = 0; 
              value_index <= max_count && /* as long as the user has space */
              query[i+param_length+1+value_index] != '&' && /* until the next parameter starts */
              query[i+param_length+1+value_index] != '\0'; /* or query's end is reached */
              value_index++)
            {
              value[value_index] = query[i+param_length+1+value_index];
            }
          }
        }

      i++;
    }
  }
  return value_index;
}

static int ev_handler(struct mg_connection *conn, enum mg_event ev)
{
  char param_gpio[10], param_value[10], param_direction[10];
  int gpio; int value = -1; bool input_value; gpio_direction_t direction;

  
  switch (ev)
  {
    case MG_REQUEST:
      if (get_query_param("gpio", conn->query_string, param_gpio, sizeof(param_gpio)) > 0)
        gpio = atoi(param_gpio);
      if (get_query_param("value", conn->query_string, param_value, sizeof(param_value)) > 0)
        value = atoi(param_value);
      get_query_param("direction", conn->query_string, param_direction, sizeof(param_direction));

      printf("parameters: gpio=%i, value=%i, direction=%s\n", gpio, value, param_direction);

      if (is_request(uri_gpio_get))
        printf("gpio get\n");
      else if (is_request(uri_gpio_export))
      {
        gpio_export(gpio);
        mg_send_data(conn, NULL, 0);
      }
      else if (is_request(uri_gpio_unexport))
      {
        gpio_unexport(gpio);
        mg_send_data(conn, NULL, 0);
      }
      else if (is_request(uri_gpio_value_get))
      {
        gpio_get_value(gpio, &input_value);
        mg_send_data(conn, input_value ? "1" : "0", 1);
      }
      else if (is_request(uri_gpio_value_set))
      {
        if (value == 0)
          gpio_set_value(gpio, false);
        else if (value == 1)
          gpio_set_value(gpio, true);

        mg_send_data(conn, NULL, 0);
      }
      else if (is_request(uri_gpio_direction_get))
      {
        if (gpio_get_direction(gpio, &direction))
        {
          switch (direction)
          {
            case gpio_direction_in:
              mg_send_data(conn, "in", 2);
              break;
            case gpio_direction_out:
              mg_send_data(conn, "out", 3);
              break;
          }
        }
        else
          mg_send_data(conn, NULL, 0);

      }
      else if (is_request(uri_gpio_direction_set))
      {
        //printf("gpio_direction_set gpio=%i, direction=%s\n", gpio, param_direction);
        if (strncmp("in", param_direction, 2) == 0)
          gpio_set_direction(gpio, gpio_direction_in);
        else if (strncmp("out", param_direction, 3) == 0)
          gpio_set_direction(gpio, gpio_direction_out);

        mg_send_data(conn, NULL, 0);
      }
      else
        mg_send_data(conn, NULL, 0);
      return MG_TRUE;
      //return MG_MORE;
      break;

    case MG_AUTH: return MG_TRUE;
    default: return MG_FALSE;
  }
}

    //case MG_REQUEST:
    //  //mg_send_file(conn, "/home/pi/test.txt", NULL);  // Also could be a dir, or CGI
    //  //mg_send_file(conn, "/sys/class/gpio/gpio55/value", NULL);  // Also could be a dir, or CGI
    //  fp = fopen("/sys/class/gpio/gpio55/value", "r");
    //  fread(buffer, 1, 1, fp);
    //  fclose(fp);

    //  printf("serving file with content: %c\n", buffer[0]);

    //  mg_send_data(conn, buffer, 1);

    //  //return MG_MORE; // It is important to return MG_MORE after mg_send_file!
    //  return MG_TRUE;

#define MAX_GPIOS 255

void rpcp_test(void)
{
  int gpio_count;
  gpio_get_gpio_count(&gpio_count);
  printf("gpio_count: %i\n", gpio_count);

  int gpios[MAX_GPIOS];
  int i;
  gpio_count = gpio_get_gpios(gpios, MAX_GPIOS);
  printf("gpio_count: %i\n", gpio_count);
  for (i = 0; i < MAX_GPIOS && i < gpio_count; i++)
    printf("gpio %i: %i\n", i, gpios[i]);

  printf("gpio_export(55): %i\n", gpio_export(55));

  gpio_direction_t direction;
  if (gpio_get_direction(55, &direction))
    printf("gpio_get_direction(55) was successful: %i\n", direction);
  else
    printf("gpio_get_direction(55) failed\n");

  bool value;
  if (gpio_get_value(55, &value))
    printf("gpio_get_value(55) was successful: %i\n", value);
  else
    printf("gpio_get_value(55) failed\n");

  printf("gpio_unexport(55): %i\n", gpio_unexport(55));

}

int main(void)
{
  struct mg_server *server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8080");

  //rpcp_test();

  printf("Starting rpc+webapi on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) mg_poll_server(server, 1000);
  mg_destroy_server(&server);

  return 0;
}
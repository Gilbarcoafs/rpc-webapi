/* written by mdk, at amescon.com*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mongoose.h"
#include "rpc+.h"

typedef struct {
  bool is_debug;
} config_t;

static config_t m_config = { true };
static const char *m_content_files[] = { "index.html", "index_gpio.html", "knockout-3.2.0.js", "jquery-2.1.1.min.js", "bootstrap.min.js", "bootstrap.min.css", "glyphicons-halflings-regular.eot", "glyphicons-halflings-regular.svg", "glyphicons-halflings-regular.ttf", "glyphicons-halflings-regular.woff" };
#define content_file_count (sizeof(m_content_files)/sizeof(char*))

#define uri_index              "/"
#define uri_content            "/content"
#define uri_gpio_get           "/gpio/get"
#define uri_gpio_export        "/gpio/export"
#define uri_gpio_unexport      "/gpio/unexport"
#define uri_gpio_value_get     "/gpio/value/get"
#define uri_gpio_value_set     "/gpio/value/set"
#define uri_gpio_direction_get "/gpio/direction/get"
#define uri_gpio_direction_set "/gpio/direction/set"

#define is_request(uri,req) (strncmp(uri, req, sizeof(req)) == 0)
#define starts_with_request(uri,req) (strncmp(uri, req, sizeof(req)-1) == 0)

#define DEBUG 1
#if DEBUG
#define ON_DEBUG(a) do { a; } while (false);
#else
#define ON_DEBUG(a)
#endif

typedef enum {
  request_unknown,
  request_index,
  request_content,
  request_gpio_get,
  request_gpio_export,
  request_gpio_unexport,
  request_gpio_value_get,
  request_gpio_value_set,
  request_gpio_direction_get,
  request_gpio_direction_set
} request_t;

static request_t get_request_type(const char *uri)
{
  request_t ret;
  ON_DEBUG(printf("uri: %s -> ", uri));
  if (is_request(uri, uri_index))
  {
    ret = request_index;
    ON_DEBUG(printf("request_index"))
  }
  else if (starts_with_request(uri, uri_content))
  {
    ret = request_content;
    ON_DEBUG(printf("request_content"));
  }
  else if (is_request(uri, uri_gpio_get))
  {
    ret = request_gpio_get;
    ON_DEBUG(printf("request_gpio_get"));
  }
  else if (is_request(uri, uri_gpio_export))
  {
    ret = request_gpio_export;
    ON_DEBUG(printf("request_gpio_export"));
  }
  else if (is_request(uri, uri_gpio_unexport))
  {
    ret = request_gpio_unexport;
    ON_DEBUG(printf("request_gpio_unexport"));
  }
  else if (is_request(uri, uri_gpio_value_get))
  {
    ret = request_gpio_value_get;
    ON_DEBUG(printf("request_gpio_value_get"));
  }
  else if (is_request(uri, uri_gpio_value_set))
  {
    ret = request_gpio_value_set;
    ON_DEBUG(printf("request_gpio_value_set"));
  }
  else if (is_request(uri, uri_gpio_direction_get))
  {
    ret = request_gpio_direction_get;
    ON_DEBUG(printf("request_gpio_direction_get"));
  }
  else if (is_request(uri, uri_gpio_direction_set))
  {
    ret = request_gpio_direction_set;
    ON_DEBUG(printf("request_gpio_direction_set"));
  }
  else
  {
    ret = request_unknown;
    ON_DEBUG(printf("request_unknown"));
  }
  ON_DEBUG(printf("\n"));
  return ret;
}

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
              value_index < max_count && /* as long as the user has space */
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

  /* add the string termination */
  value[value_index] = '\0';

  return value_index;
}

static int get_index_of(const char *string, const char c)
{
  int i;
  if (string != NULL)
    for (i = 0; string[i] != '\0'; i++)
      if (string[i] == c)
        return i;
  return -1;
}

static int get_last_index_of(const char *string, const char c)
{
  int i, last_index = -1;
  if (string != NULL)
    for (i = 0; string[i] != '\0'; i++)
      if (string[i] == c)
        last_index = i;

  return last_index;
}

static int ev_handler(struct mg_connection *conn, enum mg_event ev)
{
  static int request_num = 0;
  char param_gpio[10], param_value[10], param_direction[10], param_content[20];
  int gpio, last_slash; int value = -1; bool input_value; gpio_direction_t direction;
  request_t req;

  switch (ev)
  {
    case MG_REQUEST:
      ON_DEBUG(printf("req# (%i)", request_num++));
      if (get_query_param("gpio", conn->query_string, param_gpio, sizeof(param_gpio)) > 0)
      {
        gpio = atoi(param_gpio);
        ON_DEBUG(printf(" param gpio: %i", gpio));
      }
      if (get_query_param("value", conn->query_string, param_value, sizeof(param_value)) > 0)
      {
        value = atoi(param_value);
        ON_DEBUG(printf(" param value: %i", value));
      }
      if (get_query_param("direction", conn->query_string, param_direction, sizeof(param_direction)) > 0)
        ON_DEBUG(printf(" param direction: %s", param_direction));
      if (get_query_param("content", conn->query_string, param_content, sizeof(param_content)) > 0)
        ON_DEBUG(printf(" param content: %s", param_content));
      ON_DEBUG(printf("\n"));

      /* get the type of the request */
      req = get_request_type(conn->uri);

      switch (req)
      {
      case request_index:
        mg_send_file(conn, "content/index.html", NULL);
        return MG_MORE;
        break;
      case request_content:
        int i;
        char filename[255];

        last_slash = get_last_index_of(conn->uri, '/');
        //ON_DEBUG(printf("last_slash: %i\n", last_slash));

        if (last_slash >= 0)
          for (i = 0; i < content_file_count; i++)
            if (strcmp(conn->uri + last_slash + 1, m_content_files[i]) == 0)
            //if (strcmp(param_content, m_content_files[i]) == 0)
            {
              snprintf(filename, sizeof(filename)-1, "content/%s", m_content_files[i]);
              ON_DEBUG(printf("serving content file: %s\n", filename))
              mg_send_file(conn, filename, NULL);
              return MG_MORE;
            }

        mg_send_data(conn, NULL, 0);
        break;

      case request_gpio_get:
        mg_send_data(conn, NULL, 0);
        break;
      case request_gpio_export:
        gpio_export(gpio);
        mg_send_data(conn, NULL, 0);
        break;
      case request_gpio_unexport:
        gpio_unexport(gpio);
        mg_send_data(conn, NULL, 0);
        break;
      case request_gpio_value_get:
        if (gpio_get_value(gpio, &input_value))
          mg_printf_data(conn, input_value ? "1" : "0" );
        else {
          ON_DEBUG(printf("can't get value of gpio: %i\n", gpio));
          mg_send_data(conn, NULL, 0);
        }
        break;
      case request_gpio_value_set:
        if (value == 0)
          gpio_set_value(gpio, false);
        else if (value == 1)
          gpio_set_value(gpio, true);
        mg_send_data(conn, NULL, 0);
        break;
      case request_gpio_direction_get:
        if (gpio_get_direction(gpio, &direction))
        {
          switch (direction)
          {
            case gpio_direction_in:
              mg_printf_data(conn, "in");
              break;
            case gpio_direction_out:
              mg_printf_data(conn, "out");
              break;
          }
        }
        else
        {
          ON_DEBUG(printf("can't get direction of gpio %i\n", gpio));
          mg_send_data(conn, NULL, 0);
        }
        break;

      case request_gpio_direction_set:
        if (strncmp("in", param_direction, 2) == 0)
          gpio_set_direction(gpio, gpio_direction_in);
        else if (strncmp("out", param_direction, 3) == 0)
          gpio_set_direction(gpio, gpio_direction_out);

        mg_send_data(conn, NULL, 0);
       break;

      case request_unknown:
        mg_send_data(conn, NULL, 0);
        break;
      }

      return MG_TRUE;
      //return MG_MORE; /* return MG_MORE when using mg_send_file() */
      
      break;

    case MG_AUTH: return MG_TRUE;
    default: return MG_FALSE;
  }
}

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
  ON_DEBUG(printf("content file count: %i\n", content_file_count));

  struct mg_server *server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8080");

  //rpcp_test();

  printf("Starting rpc+webapi on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) mg_poll_server(server, 1000);
  mg_destroy_server(&server);

  return 0;
}
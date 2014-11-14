#include <stdio.h>
#include <string.h>
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

  printf("query: %s\n", query);

  if (query != NULL)
  {
    /* search within the query string */
    while (query[i] != '\0')
    {
      printf("i: %i\n",i);
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
  char param1[255], param2[255];
  switch (ev) {
    case MG_REQUEST:
      if (is_request(uri_gpio_get))
        printf("gpio get\n");
      else if (is_request(uri_gpio_export))
      {
        printf("gpio export\n");
        if (get_query_param("gpio", conn->query_string, param1, sizeof(param1)) > 0)
          printf("get_query_param('gpio') succeeded: %s\n", param1);
        else
          printf("get_query_param('gpio') failed.\n");
      }
      else if (is_request(uri_gpio_unexport))
        printf("gpio unexport\n");
      else if (is_request(uri_gpio_value_get))
        printf("gpio value get\n");
      else if (is_request(uri_gpio_value_set))
        printf("gpio value set\n");
      else if (is_request(uri_gpio_direction_get))
        printf("gpio direction get\n");
      else if (is_request(uri_gpio_direction_set))
        printf("gpio direction set\n");

      /* */
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


int main(void) {
  struct mg_server *server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8080");

  printf("Starting rpc+webapi on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) mg_poll_server(server, 1000);
  mg_destroy_server(&server);

  return 0;
}
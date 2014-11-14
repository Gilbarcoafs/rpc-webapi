#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include "rpc+.h"

typedef struct {
  bool is_debug;
} config_t;

static config_t m_config = { true };

//static const char uri_gpio[] = "/gpio";
//static const char uri_gpio_export[] = "/gpio/export";
//static const char uri_gpio_unexport[] = "/gpio/unexport";
//static const char uri_gpio_value_set[] = "/gpio/value/set";
//static const char uri_gpio_value_get[] = "/gpio/value/get";

#define uri_gpio "/gpio"
#define uri_gpio_export uri_gpio "/export"
#define uri_gpio_unexport uri_gpio "/unexport"
#define uri_gpio_value uri_gpio "/value"
#define uri_gpio_value_get uri_gpio_value "/get"
#define uri_gpio_value_set uri_gpio_value "/set"
#define uri_gpio_direction uri_gpio "/direction"
#define uri_gpio_direcetion_get uri_gpio_direction "/get"
#define uri_gpio_direction_set uri_gpio_direction "/set"

#define is_request(req) (strncmp(conn->uri, req, sizeof(req)) == 0)

static int ev_handler(struct mg_connection *conn, enum mg_event ev)
{
  int len;
  char *req;

  switch (ev) {
    case MG_REQUEST:
      if (is_request(uri_gpio_export))
        printf("gpio export\n");
      else if (is_request(uri_gpio_unexport))
        printf("gpio unexport\n");
      else if (is_request(uri_gpio_value_get))
        printf("gpio value get\n");
      else if (is_request(uri_gpio_value_set))
        printf("gpio value set\n");

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
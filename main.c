#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include "rpc+.h"

typedef struct {

} config_t;

static config_t m_config = {};

static const char uri_gpio[] = "gpio";

static int ev_handler(struct mg_connection *conn, enum mg_event ev)
{
  FILE *fp = NULL;
  char buffer[1];

  switch (ev) {
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

    case MG_REQUEST:
      printf("requested uri: %s", conn->uri);
      if (strncmp(conn->uri, uri_gpio, sizeof(uri_gpio)) == 0)
      {
        printf("gpio request\n");
      }
      return MG_TRUE;
      break;

    case MG_AUTH: return MG_TRUE;
    default: return MG_FALSE;
  }
}

int main(void) {
  struct mg_server *server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8080");

  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) mg_poll_server(server, 1000);
  mg_destroy_server(&server);

  return 0;
}
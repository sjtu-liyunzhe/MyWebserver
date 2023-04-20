#include <unistd.h>
#include "./server/webserver.h"

int main()
{
    WebServer server(1314, 3, 60000, false, 8);
    server.start();
}
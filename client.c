#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "threadpool.h"
#include "http.h"

int main(int argc, char* argv[]) {
    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(8080);
    web_addr.sin_addr.s_addr = INADDR_ANY;
    char buf[256];
    for (int i = 0; i < 100; i++) {
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == 0)
            ERR("Couldn't allocate a socket")

        if (connect(client_socket, (struct sockaddr*)&web_addr, sizeof(struct sockaddr_in)))
            ERR("connect failed")

        snprintf(buf, 255, "\r\n\r\nGET /%i HTTP/1.1\r\n\r\nhi, what's up ?", i);
        send(client_socket, buf, sizeof(buf), 0);

        close(client_socket);
    }

    return EXIT_SUCCESS;
}

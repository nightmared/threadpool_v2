#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"
#include <netinet/in.h>

int main(int argc, char* argv[]) {
    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(8080);
    web_addr.sin_addr.s_addr = INADDR_ANY;
    for (int i = 0; i < 10000; i++) {
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == 0)
            ERR("Couldn't allocate a socket")

        if (connect(client_socket, (struct sockaddr*)&web_addr, sizeof(struct sockaddr_in)))
            ERR("connect failed")

        close(client_socket);
    }

    return EXIT_SUCCESS;
}

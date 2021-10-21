#include "thread.h"

int main(){
    int server_fd, new_socket, valread, opt = 1;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    pthread_t serverThread;

    init_server_fd(&server_fd, opt);

    init_addr(&addr);

    bind_fd(&server_fd, addr);

    listen_server(&server_fd);
    int cont = 1;
    while(cont <= 10){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cont++;
        pthread_create(&serverThread, NULL, parse_socket, (void *)new_socket);
    }

    return 0;
}
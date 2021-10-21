#include "socketPool.h"

int main(){
    int server_fd, new_socket, valread, thr_id[NUM_HANDLER_THREADS], opt = 1;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    pthread_t  p_threads[NUM_HANDLER_THREADS];
    struct timespec delay;

    init_server_fd(&server_fd, opt);

    init_addr(&addr);

    bind_fd(&server_fd, addr);

    listen_server(&server_fd);

    init_threads(thr_id, p_threads);

    sleep(3);
    int cont = 0;
    while(1){
        system("echo boleta=2020630002 puerto=8888 aplicacion=clientes3.sh > prueba100.txt");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        add_request(cont, new_socket, &request_mutex, &got_request);
        cont++;
        if (rand() > 3*(RAND_MAX/4)) {
	        delay.tv_sec = 0;
	        delay.tv_nsec = 10;
	        nanosleep(&delay, NULL);
	    }
    }

    return 0;
}
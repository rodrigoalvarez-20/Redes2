#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define PORT 8080

void init_server_fd(int *, int);
void init_addr(struct sockaddr_in *);
void bind_fd(int *, struct sockaddr_in);
void listen_server(int *);
void *parse_socket(void *);

void init_server_fd(int *fd, int opt){
    if(( *fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Ha ocurrido un error al crear el socket");
        exit(-1);
    }

    if(setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("No se ha podido ligar el puerto al socket");
        exit(-1);
    }

}

void init_addr(struct sockaddr_in *addr){
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);
}

void bind_fd(int *fd, struct sockaddr_in addr){
    if(bind(*fd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
        perror("No se ha podido hacer el bind");
        exit(-1);
    }
}

void listen_server(int *fd){
    if(listen(*fd, 3) < 0){
        perror("Fallo al iniciar el Listen");
        exit(-1);
    }
}

void *parse_socket(void *d){
    int socket = (int)d;
    pthread_detach(pthread_self());

    //while(1){
        char buffer[1024] = {0};
        int valread = read(socket, buffer, 1024);
        printf("Mensaje recibido: %s\n",buffer );
        char *greet = "Hola desde el thread";
        send(socket, greet, strlen(greet) , 0 );
        printf("Mensaje enviado\n");
    //}

    close(socket);

}
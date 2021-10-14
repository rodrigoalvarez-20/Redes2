#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>

#define PORT 8888

void init_server_fd(int *, int);
void init_addr(struct sockaddr_in *);
void bind_fd(int *, struct sockaddr_in);
void listen_server(int *);
void *parse_socket(void *);

void init_server_fd(int *fd, int opt)
{
    if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Ha ocurrido un error al crear el socket");
        exit(-1);
    }

    if (setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("No se ha podido ligar el puerto al socket");
        exit(-1);
    }
}

void init_addr(struct sockaddr_in *addr)
{
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("127.0.0.1");
    addr->sin_port = htons(PORT);
}

void bind_fd(int *fd, struct sockaddr_in addr)
{
    if (bind(*fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("No se ha podido hacer el bind");
        exit(-1);
    }
}

void listen_server(int *fd)
{
    if (listen(*fd, 3) < 0)
    {
        perror("Fallo al iniciar el Listen");
        exit(-1);
    }
}

void *parse_socket(void *d)
{
    int socket = *((int *) d);
    pthread_detach(pthread_self());

    char buffer[1024] = {0};
    int valread = read(socket, buffer, 1024);
    printf("Mensaje recibido: %s\n", buffer);
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = toupper(buffer[i]);
    }

    printf("Cadena a enviar: %s\n", buffer);

    send(socket, buffer, strlen(buffer), 0);

    close(socket);
}

int main()
{
    int server_fd, new_socket, valread, opt = 1;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    pthread_t serverThread;

    init_server_fd(&server_fd, opt);

    init_addr(&addr);

    bind_fd(&server_fd, addr);

    listen_server(&server_fd);
    int cont = 1;
    
    while (cont <= 10)
    {
        system("echo boleta=2020630002 puerto=8888 aplicacion=clientes3.sh > prueba100.txt");
        if ((new_socket = accept(server_fd, 
            (struct sockaddr *)&addr, 
            (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        char *bienvenida = "Bienvenido\n";
        send(new_socket, bienvenida, strlen(bienvenida), 0);
        cont++;
        pthread_create(&serverThread, 
            NULL, 
            parse_socket, 
            (void *)&new_socket);
    }

    return 0;
}

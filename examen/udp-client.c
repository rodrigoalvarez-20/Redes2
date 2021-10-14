#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define SERVER_PORT 8000

typedef struct sock_params
{
    int server_socket;
    struct sockaddr_in address;
} socket_params_t;

void verifySocketConnection(int socket)
{
    if (socket == -1)
    {
        perror("Ha ocurrido un error al crear el socket\n");
        exit(-1);
    }
    printf("Socket creado con exito\n");
}

void *sendMessage(void *data)
{
    socket_params_t *params = (socket_params_t *)data;
    char msj[512];

    printf("Introduce el mensaje: \n");
    fgets(msj, sizeof(msj), stdin);
    fflush(stdin);

    int tam = sendto(params->server_socket, msj, 
        strlen(msj) + 1, 0, 
        (struct sockaddr *)&(params->address), 
        sizeof(params->address));

    if (tam == -1)
    {
        //Error al enviar los datos
        perror("Ha ocurrido un error al enviar los datos al servidor\n");
        exit(-1);
    }
}

void *receiveMessage(void *data)
{
    socket_params_t *params = (socket_params_t *)data;
    char msjRecv[512];

    int lrecv = sizeof(params->address);
    int tam = recvfrom(params->server_socket, 
        msjRecv, 512, 0, 
        (struct sockaddr *)&(params->address), &lrecv);
    if (tam == -1)
    {
        perror("Mensaje no procesable\n");
        exit(-1);
    }

    printf("Mensaje nuevo: %s\n", msjRecv);
}

int main()
{
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servidor;
    pthread_t send, receive;
    char msj[512] = {0};

    verifySocketConnection(server_socket);

    memset(&servidor, 0, sizeof(servidor));

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(SERVER_PORT);
    servidor.sin_addr.s_addr = INADDR_ANY;

    socket_params_t toSend;

    toSend.server_socket = server_socket;
    toSend.address = servidor;

    while (1)
    {
        pthread_create(&send, NULL, &sendMessage, &toSend);
        pthread_create(&receive, NULL, &receiveMessage, &toSend);

        pthread_join(send, NULL);
        pthread_join(receive, NULL);
    }

    close(server_socket);
}
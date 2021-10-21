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

typedef struct s_chat
{
    int socket;
    char msj[512];
    struct sockaddr_in remota;
} chat;

typedef struct node
{
    struct sockaddr_in cliente;
    in_port_t port;
    struct node *next;
} Node;

typedef struct list
{
    Node *root;
    int size;
} List;

typedef struct threadParams
{
    int srv_socket;
    List l;
} thread_params_t;

void initList(List *l)
{
    if (l == NULL)
    {
        printf("La lista es nula\n");
        exit(-1);
    }
    l->root = NULL;
    l->size = 0;
}

Node *createNode(struct sockaddr_in c, in_port_t p)
{
    Node *toAdd = (Node *)malloc(sizeof(Node));

    if (toAdd == NULL)
    {
        printf("Ha ocurrido un error al crear el nodo\n");
    }

    toAdd->cliente = c;
    toAdd->port = p;
    toAdd->next = NULL;

    return toAdd;
}

void addToList(List *l, Node *nToAdd)
{
    if (l == NULL)
    {
        printf("La lista es nula\n");
        exit(-1);
    }
    nToAdd->next = l->root;
    l->root = nToAdd;
    (l->size)++;
}

int checkIfAddressIsAdded(List *l, in_port_t p)
{
    if (l == NULL)
    {
        printf("La lista es nula\n");
        exit(-1);
    }
    int isAdded = 0;
    if (l->root != NULL)
    {
        for (Node *n = l->root;; n = n->next)
        {
            if (n == NULL)
                break;

            if (n->port == p)
            {
                isAdded = 1;
                break;
            }
        }
    }
    return isAdded; //0 si no se ha agregado, 1 si ya existe en la lista
}

void displayList(List *l)
{
    if (l == NULL)
    {
        printf("La lista es nula\n");
        exit(-1);
    }
    if (l->root != NULL)
    {
        for (Node *n = l->root;; n = n->next)
        {
            if (n == NULL)
                break;
            printf("Port: %d\n", n->port);
        }
    }
}

void *acceptMessages(void *data)
{
    thread_params_t *params = (thread_params_t *)data;
    struct sockaddr_in clientTemp;
    int len = sizeof(clientTemp);
    char buffer[512];
    int n = recvfrom((*params).srv_socket, 
        (char *)buffer, 512, 0, 
        (struct sockaddr *)&clientTemp, &len);

    if (!checkIfAddressIsAdded(&(*params).l, clientTemp.sin_port))
    {
        //Añadir a la lista y modificar el contador
        printf("Nuevo cliente conectado: %d\n", clientTemp.sin_port);
        addToList(&(*params).l, createNode(clientTemp, clientTemp.sin_port));
    }

    //Hacer la recepcion de datos

    if ((*params).l.size >= 2)
    {
        //Hay 2 o mas clientes conectados
        //Mostrar en todos los clientes el mensaje, excepto en el remitente
        printf("Mensaje de cliente %d: %s", clientTemp.sin_port, buffer);
        for (Node *n = (*params).l.root;; n = n->next)
        {
            if (n == NULL)
                break;
            if (n->port != clientTemp.sin_port)
            {
                sendto((*params).srv_socket, buffer, 
                    strlen(buffer), 0, (struct sockaddr *)&n->cliente, 
                    sizeof(n->cliente));
            }
        }
    }
}

void verifySocketConnection(int socket)
{
    if (socket == -1)
    {
        perror("Ha ocurrido un error al crear el socket\n");
        exit(-1);
    }
    printf("Socket creado con exito\n");
}

void verifyBind(int lbdind)
{
    if (lbdind == -1)
    {
        perror("Ha ocurrido un error al hacer el bind\n");
        exit(-1);
    }

    printf("Bind hecho con exito\n");
}

int main()
{
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servidor;
    pthread_t messages;
    List l;

    initList(&l);

    verifySocketConnection(server_socket);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(SERVER_PORT);
    servidor.sin_addr.s_addr = INADDR_ANY;

    int lbind = bind(server_socket, 
        (struct sockaddr *)&servidor, 
        sizeof(servidor));

    verifyBind(lbind);

    thread_params_t params;
    params.l = l;
    params.srv_socket = server_socket;

    while (1)
    {
        pthread_create(&messages, NULL, &acceptMessages, &params);
        pthread_join(messages, NULL);
    }

    close(server_socket);

    return 0;
}
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define __USE_GNU
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#define NUM_HANDLER_THREADS 10
#define PORT 8888

pthread_mutex_t request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

pthread_cond_t got_request = PTHREAD_COND_INITIALIZER;

int num_requests = 0;

typedef struct request
{
    int number, socket;
    struct request *next;
} Request;

Request *requests = NULL;
Request *last_request = NULL;

void init_server_fd(int *, int);
void init_addr(struct sockaddr_in *);
void bind_fd(int *, struct sockaddr_in);
void listen_server(int *);
void init_threads(int *, pthread_t *);

void *handle_requests_loop(void *);

void handle_request(Request *, int);

Request *get_request(pthread_mutex_t *);

void add_request(int, int, pthread_mutex_t *, pthread_cond_t *);

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

void init_threads(int *thr_id, pthread_t *p_threads)
{
    for (int i = 0; i < NUM_HANDLER_THREADS; i++)
    {
        thr_id[i] = i;
        pthread_create(&p_threads[i], NULL, handle_requests_loop, (void *)&thr_id[i]);
    }
}

void handle_request(struct request *a_request, int thread_id)
{
    if (a_request)
    {
        char buffer[1024] = {0};
        int valread = read(a_request->socket, buffer, 1024);
        printf("%s\n", buffer);
        for (int i = 0; i < strlen(buffer); i++)
        {
            buffer[i] = toupper(buffer[i]);
        }

        printf("%s\n", buffer);

        send(a_request->socket, buffer, strlen(buffer), 0);

        close(a_request->socket);
    }
}

void *handle_requests_loop(void *data)
{
    int rc;
    struct request *a_request;
    int thread_id = *((int *)data);
    rc = pthread_mutex_lock(&request_mutex);
    while (1)
    {
        if (num_requests > 0)
        {
            a_request = get_request(&request_mutex);
            if (a_request)
            {
                handle_request(a_request, thread_id);
                free(a_request);
            }
        }
        else
        {
            rc = pthread_cond_wait(&got_request, &request_mutex);
        }
    }
}

Request *get_request(pthread_mutex_t *p_mutex)
{
    int rc;
    Request *a_request;
    rc = pthread_mutex_lock(p_mutex);

    if (num_requests > 0)
    {
        a_request = requests;
        requests = a_request->next;
        if (requests == NULL)
        {
            last_request = NULL;
        }
        num_requests--;
    }
    else
    {
        a_request = NULL;
    }
    rc = pthread_mutex_unlock(p_mutex);

    return a_request;
}

void add_request(int n, int socket, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond_var)
{
    int rc;
    Request *a_request;

    a_request = (Request *)malloc(sizeof(Request));
    if (!a_request)
    { /* malloc failed?? */
        fprintf(stderr, "add_request: out of memory\n");
        exit(1);
    }
    a_request->number = n;
    a_request->socket = socket;
    a_request->next = NULL;

    rc = pthread_mutex_lock(p_mutex);

    if (num_requests == 0)
    {
        requests = a_request;
        last_request = a_request;
    }
    else
    {
        last_request->next = a_request;
        last_request = a_request;
    }

    num_requests++;

    rc = pthread_mutex_unlock(p_mutex);

    rc = pthread_cond_signal(p_cond_var);
}

int main()
{
    int server_fd, new_socket, valread, thr_id[NUM_HANDLER_THREADS], opt = 1;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    pthread_t p_threads[NUM_HANDLER_THREADS];
    struct timespec delay;

    init_server_fd(&server_fd, opt);

    init_addr(&addr);

    bind_fd(&server_fd, addr);

    listen_server(&server_fd);

    init_threads(thr_id, p_threads);

    sleep(3);
    int cont = 0;
    while (cont <= 10)
    {
        //printf("%d\n", cont);
        system("echo boleta=2020630002 puerto=8888 aplicacion=clientes3.sh > prueba100.txt");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        char *bienvenida = "Bienvenido\n";
        send(new_socket, bienvenida, strlen(bienvenida), 0);
        add_request(cont, new_socket, &request_mutex, &got_request);
        cont++;
        if (rand() > 3 * (RAND_MAX / 4))
        {
            delay.tv_sec = 0;
            delay.tv_nsec = 10;
            nanosleep(&delay, NULL);
        }
    }

    return 0;
}
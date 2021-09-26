#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define __USE_GNU 
#include <pthread.h>
#include <unistd.h>

#define NUM_HANDLER_THREADS 3
#define PORT 8080

pthread_mutex_t request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

pthread_cond_t  got_request   = PTHREAD_COND_INITIALIZER;

int num_requests = 0;

typedef struct request {
    int number, socket;
    struct request* next;
} Request;

Request *requests = NULL; 
Request *last_request = NULL;

void init_server_fd(int *, int);
void init_addr(struct sockaddr_in *);
void bind_fd(int *, struct sockaddr_in);
void listen_server(int *);
void init_threads(int *, pthread_t *);

void *handle_requests_loop(void *);

void handle_request(Request*, int);

Request *get_request(pthread_mutex_t *);

void add_request(int, int, pthread_mutex_t*, pthread_cond_t*);


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

void init_threads( int *thr_id, pthread_t *p_threads){
     for (int i=0; i<NUM_HANDLER_THREADS; i++) {
	    thr_id[i] = i;
	    pthread_create(&p_threads[i], NULL, handle_requests_loop, (void*)&thr_id[i]);
    }
}

void handle_request(struct request* a_request, int thread_id) { //Aqui quiza deba de meter el handle de mi socket
    if (a_request) {
        char buffer[1024] = {0};
	    printf("Thread '%d' handled request '%d'\n", thread_id, a_request->number);
        int valread = read( a_request->socket , buffer, 1024);
        printf("Mensaje recibido: %s\n",buffer );
        char *greet = "Hola desde el pool";
        send(a_request->socket, greet, strlen(greet) , 0 );
        printf("Mensaje enviado\n");
        fflush(stdout);
    }
}

void *handle_requests_loop(void* data) {
    int rc;	                    /* return code of pthreads functions.  */
    struct request* a_request;      /* pointer to a request.               */
    int thread_id = *((int*)data);  /* thread identifying number           */

    //printf("Starting thread '%d'\n", thread_id);
    //fflush(stdout);

    /* lock the mutex, to access the requests list exclusively. */
    rc = pthread_mutex_lock(&request_mutex);

    //printf("thread '%d' after pthread_mutex_lock\n", thread_id);
    //fflush(stdout);

    /* do forever.... */
    while (1) {
    	//printf("thread '%d', num_requests =  %d\n", thread_id, num_requests);
    	//fflush(stdout);
	    if (num_requests > 0) { /* a request is pending */
	        a_request = get_request(&request_mutex);
	        if (a_request) { /* got a request - handle it and free it */
		        handle_request(a_request, thread_id);
		        free(a_request);
	        }
	    } else {
	        /* wait for a request to arrive. note the mutex will be */
	        /* unlocked here, thus allowing other threads access to */
	        /* requests list.                                       */
    	    //printf("thread '%d' before pthread_cond_wait\n", thread_id);
    	    //fflush(stdout);
	        rc = pthread_cond_wait(&got_request, &request_mutex);
	        /* and after we return from pthread_cond_wait, the mutex  */
	        /* is locked again, so we don't need to lock it ourselves */
    	    //printf("thread '%d' after pthread_cond_wait\n", thread_id);
    	    //fflush(stdout);
	    }
    }
}

Request *get_request(pthread_mutex_t* p_mutex) {
    int rc;	                    /* return code of pthreads functions.  */
    Request* a_request;      /* pointer to request.                 */

    /* lock the mutex, to assure exclusive access to the list */
    rc = pthread_mutex_lock(p_mutex);

    if (num_requests > 0) {
	    a_request = requests;
	    requests = a_request->next;
	    if (requests == NULL) { /* this was the last request on the list */
	        last_request = NULL;
	    }
	    /* decrease the total number of pending requests */
	    num_requests--;
    } else { /* requests list is empty */
	    a_request = NULL;
    }

    /* unlock mutex */
    rc = pthread_mutex_unlock(p_mutex);

    /* return the request to the caller. */
    return a_request;
}

void add_request(int n, int socket, pthread_mutex_t* p_mutex, pthread_cond_t*  p_cond_var) {
    int rc;
    Request *a_request;

    /* create structure with new request */
    a_request = (Request*)malloc(sizeof(Request));
    if (!a_request) { /* malloc failed?? */
	    fprintf(stderr, "add_request: out of memory\n");
	    exit(1);
    }
    a_request->number = n;
    a_request->socket = socket;
    a_request->next = NULL;

    /* lock the mutex, to assure exclusive access to the list */
    rc = pthread_mutex_lock(p_mutex);

    /* add new request to the end of the list, updating list */
    /* pointers as required */
    if (num_requests == 0) { /* special case - list is empty */
	    requests = a_request;
	    last_request = a_request;
    } else {
	    last_request->next = a_request;
	    last_request = a_request;
    }

    num_requests++;

    /* unlock mutex */
    rc = pthread_mutex_unlock(p_mutex);

    /* signal the condition variable - there's a new request to handle */
    rc = pthread_cond_signal(p_cond_var);
}

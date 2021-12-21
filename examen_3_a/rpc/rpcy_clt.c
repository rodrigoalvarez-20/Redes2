#include <stdio.h>
#include "rpcy.h"

int main(int argc, char *argv[])
{
    CLIENT *clnt;
    char *server;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s {servidor_remoto}\n", argv[0]);
        exit(1);
    }

    server = argv[1];
    clnt = clnt_create(server, RPCE, RPC_1, "tcp");
    printf("Se ha creado el cliente\n");
    if (clnt == NULL)
    {
        clnt_pcreateerror(server);
        exit(1);
    }

    int *r = test_1(NULL, clnt);
    if(r == NULL){
        printf("Ha ocurrido un error al llamar la funcion RPC\n");
        exit(-1);
    }

    printf("Status: %d\n", *r);

    clnt_destroy(clnt);
    exit(0);
}
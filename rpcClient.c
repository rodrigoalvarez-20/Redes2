#include <stdio.h>
#include "Validator.h"

int main(int argc, char *argv[])
{
    CLIENT *clnt;
    int *result;
    char *server;
    //char *message;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s {servidor_remoto}\n", argv[0]);
        exit(1);
    }

    server = argv[1];
    clnt = clnt_create(server, VALIDATOR, VALIDATOR_1, "visible");
    if (clnt == NULL)
    {
        clnt_pcreateerror(server);
        exit(1);
    }

    data pointCloud;

    pointCloud.x = 0.82;
    pointCloud.y = -0.82;
    pointCloud.z = 0.5678;
    pointCloud.r = 0;
    pointCloud.g = 254;
    pointCloud.b = 0;

    result = validate_point_1(&pointCloud, clnt);
    if (result == NULL)
    {
        clnt_perror(clnt, server);
        exit(1);
    }

    print("%d\n", *result);

    clnt_destroy(clnt);
    exit(0);
}
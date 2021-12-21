#include <stdio.h>
#include "rpcy.h"

int *test_1_svc(void *args, struct svc_req *rqstp){
    int res = 0;
    int *resp;
    resp = &res;
    printf("Llamada desde el servidor remoto\n");
    return(resp);
}
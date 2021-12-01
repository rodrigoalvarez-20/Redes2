#include <stdio.h>
#include "validator.h"

int *validate_point_1(point *dt, struct svc_req *req)
{
    int res = 1;

    printf("%d\n", dt->x);

    return (&res);
}
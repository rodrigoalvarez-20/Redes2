#include <stdio.h>
#include "validator.h"

/**
 * @brief Funcion que permite evaluar un punto de la nube
 * 
 * @param dt: Puntero de la estructura que contiene los valores (x,y,z,r,g,b)
 * @param req: Estructura del mensaje del servidor, no la ocupamos
 * @return int* : Regresamos un puntero de entero. 1 si es un punto valido, 0 si no lo es.
 */

int *validate_point_1_svc(point *dt, struct svc_req *req)
{
    int res = 0; // No sabia como hacerlo de una forma mas bonita, ya que siempre me marcaba error
    int *ret = &res;

    res = dt->g == 254; //Solamente evaluo el valor verde para separarlo

    return ret;
}
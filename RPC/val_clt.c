
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "validator.h"

int main(int argc, char *argv[])
{
    CLIENT *clnt;        //Autogenerado, es el cliente
    FILE *fpCSV, *fpOUT; //Archivos a leer y escribir
    char *server, *csvFilePath, *outPath, *line = NULL, *token, delim[] = ",";
    // Nombre del servidor, path del CSV, path de salida (con nombre de archivo), linea actual que estamos leyendo, token de separacion, delimitador
    // CSV --> Comma Separated Values (valores separados por coma)

    size_t len = 0;
    ssize_t read;

    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s {servidor_remoto} {ruta_csv_analizar} {ruta_salida_csv_filtrado} \n", argv[0]); //Se ve bien mona la advertencia
        exit(1);
    }

    server = argv[1];      //Jalo el nombre del server, no te olvides de pasarlo entre comillas
    csvFilePath = argv[2]; //Igual el path del archivo a analizar "/var/www/html/pc/data/pointcloud_4/pc.csv"
    outPath = argv[3];     // Path del archivo de salida "/var/www/html/pc/data/pointcloud_5/pc.csv"

    clnt = clnt_create(server, PVAL, VALIDATOR, "udp"); //Creamos el cliente, fijate bien que el segundo y tercer valor correspondan con los valores del *.x y debe de ser "udp"
    printf("Se ha creado el cliente\n");
    if (clnt == NULL)
    {
        clnt_pcreateerror(server);
        exit(1);
    }

    point pointCloud; //temporal para guardar los datos

    fpCSV = fopen(csvFilePath, "r"); //Abrimos el archivo con los puntos
    fpOUT = fopen(outPath, "w");     //Abrimos el archivo de salida

    if (fpCSV == NULL || fpOUT == NULL)
    {
        printf("El puntero de archivo es invalido\n");
        exit(-1);
    }

    while ((read = getline(&line, &len, fpCSV)) != -1) //Vamos leyendo linea por linea
    {
        //printf("%s", line); //Imprimimos la linea (Esto es solo de DEBUG, ya al momento de hacer el deploy quitar, ya que casi vale madres la compu del labo)

        token = strtok(line, delim); // STRTOK --> X
        pointCloud.x = atof(token);  //Asignamos
        token = strtok(NULL, delim); // STRTOK --> Y
        pointCloud.y = atof(token);
        token = strtok(NULL, delim); // STRTOK --> Z
        pointCloud.z = atof(token);
        token = strtok(NULL, delim); // STRTOK --> R
        pointCloud.r = atoi(token);
        token = strtok(NULL, delim); // STRTOK --> G
        pointCloud.g = atoi(token);
        token = strtok(NULL, delim); // STRTOK --> B
        pointCloud.b = atoi(token);

        //Mandamos a evaluar
        int *res;
        res = validate_point_1(&pointCloud, clnt); //Mandamos a llamar a la funcion remota
        //printf("Res: %d\n", *res);
        if (*res > 1) //Tengo un pequenio bug que siempre la primer llamada devuelve un valor incorrecto
        {
            //Vuelvo a solicitar la llamada (BUG)
            res = validate_point_1(&pointCloud, clnt);
        }

        ///printf("Second Res: %d\n", *res);

        if (*res) //Si es valida la respuesta (1)
        {
            fprintf(fpOUT, "%f,%f,%f,%d,%d,%d\n", pointCloud.x, pointCloud.y, pointCloud.z, pointCloud.r, pointCloud.g, pointCloud.b); //Grabo en el archivo de salia
        }
    }

    fclose(fpCSV); //Cerramos archivos
    fclose(fpOUT);
    if (line)
        free(line); //Limpiamos buffer de linea

    clnt_destroy(clnt);                       //Destruimos bien salvajemente el cliente
    printf("Se ha terminado la ejecucion\n"); //Mostramos un aviso
    return 0;
}
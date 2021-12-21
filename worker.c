#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct point
{
    float x;
    float y;
    float z;
    int r;
    int g;
    int b;
} point;

int main(int argc, char *argv[])
{
    //CLIENT *clnt;
    //int *result;
    FILE *fpCSV, *fpOUT;
    char *server, *csvFilePath, *outPath, *line = NULL, *token, delim[] = ",";;
    size_t len = 0;
    ssize_t read;

    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s {servidor_remoto} {ruta_csv_analizar} {ruta_salida_csv_filtrado} \n", argv[0]);
        exit(1);
    }

    server = argv[1];
    csvFilePath = argv[2];
    outPath = argv[3];

    //clnt = clnt_create(server, PVAL, VALIDATOR, "tcp");
    /* printf("Se ha creado el cliente\n");
    if (clnt == NULL)
    {
        clnt_pcreateerror(server);
        exit(1);
    } */

    point pointCloud;

    fpCSV = fopen(csvFilePath, "r");
    fpOUT = fopen(outPath, "w");

    if (fpCSV == NULL || fpOUT == NULL)
    {
        printf("El puntero de archivo es invalido\n");
        exit(-1);
    }

    while ((read = getline(&line, &len, fpCSV)) != -1)
    {
        //printf("%s", line);

        token = strtok(line, delim);
        pointCloud.x = atof(token);
        token = strtok(NULL, delim);
        pointCloud.y = atof(token);
        token = strtok(NULL, delim);
        pointCloud.z = atof(token);
        token = strtok(NULL, delim);
        pointCloud.r = atoi(token);
        token = strtok(NULL, delim);
        pointCloud.g = atoi(token);
        token = strtok(NULL, delim);
        pointCloud.b = atoi(token);

        //Mandamos a evaluar
        int res;
        res = pointCloud.g == 254;
        if(res > 1){
            //Vuelvo a solicitar la llamada (BUG)
        }

        if(res){
            fprintf(fpOUT, "%f,%f,%f,%d,%d,%d\n",pointCloud.x, pointCloud.y, pointCloud.z, pointCloud.r, pointCloud.g, pointCloud.b);
        }
    }

    fclose(fpCSV);
    fclose(fpOUT);
    if(line)
        free(line);
    

    //result = validate_point_1(&pointCloud, clnt);
    /* if (result == NULL)
    {
        clnt_perror(clnt, server);
        exit(1);
    }

    print("%d\n", *result);

    clnt_destroy(clnt); */
    return 0;
}
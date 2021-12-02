int tftp(char mensaje_recibido[], int longitud, char resultado[])
{
    long tipoPaquete = 0;
    int idxType = 0;
    int typeLen = 2;
    unsigned char auxArray[typeLen];
    for (int aux = 0; aux < typeLen; aux++)
        auxArray[aux] = mensaje_recibido[idxType + typeLen - 1 - aux];
    memcpy(&tipoPaquete, auxArray, typeLen);
    if (tipoPaquete == 1)
    {
        int index = idxType + typeLen;
        int indexName = 0;
        while (mensaje_recibido[index] != 0)
        {
            resultado[indexName++] = mensaje_recibido[index++];
        }
        resultado[indexName] = '\0';
        return 1;
    }
    else if (tipoPaquete == 4)
    {
        //ack
        long nb = 0;
        int bl = 2;
        int index = idxType + typeLen;
        for (int aux = 0; aux < bl; aux++)
            auxArray[aux] = mensaje_recibido[index + bl - 1 - aux];
        memcpy(&nb, auxArray, bl);
        sprintf(resultado, "%lld", nb);
        int len = strlen(resultado);
        resultado[len] = '\0';
        return 1;
    }
    return 0;
}
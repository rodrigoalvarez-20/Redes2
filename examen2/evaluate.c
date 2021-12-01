int dhcp(char mensaje_recibido[], int longitud, char resultado[])
{
    int pos = 0;
    long long int type = 0;
    unsigned char magicCookie[4] = {0x63, 0x82, 0x53, 0x63};
    for (pos = 0;; pos++)
        if ((unsigned char)mensaje_recibido[pos] == magicCookie[0] &&
            (unsigned char)mensaje_recibido[pos + 1] == magicCookie[1] &&
            (unsigned char)mensaje_recibido[pos + 2] == magicCookie[2] &&
            (unsigned char)mensaje_recibido[pos + 3] == magicCookie[3])
            break;
    unsigned char opType = 0x35;
    unsigned char option = 0;
    int typeIdx = pos + 4;
    int index = pos + 4;
    while (1)
    {
        if ((unsigned char)mensaje_recibido[typeIdx] == opType)
        {
            int typeLength = mensaje_recibido[typeIdx + 1];
            type = 0;
            unsigned char auxArray[typeLength];
            for (int aux = 0; aux < typeLength; aux++)
                auxArray[aux] = mensaje_recibido[typeIdx + 2 + typeLength - 1 - aux];
            memcpy(&type, auxArray, typeLength);
            break;
        }
        else
        {
            typeIdx = (unsigned int)mensaje_recibido[typeIdx + 1] + 2 + typeIdx;
        }
    }
    if (type == 1)
    {
        option = 0x3d;
        while (1)
        {
            if ((unsigned char)mensaje_recibido[index] == option)
            {
                int hwt = mensaje_recibido[index + 2];
                sprintf(resultado, "%d %02x:%02x:%02x:%02x:%02x:%02x", hwt, (unsigned char)mensaje_recibido[index + 3], (unsigned char)mensaje_recibido[index + 4], (unsigned char)mensaje_recibido[index + 5], (unsigned char)mensaje_recibido[index + 6], (unsigned char)mensaje_recibido[index + 7], (unsigned char)mensaje_recibido[index + 8]);
                int len = strlen(resultado);
                resultado[len] = '\0';
                break;
            }
            else
            {
                index = (unsigned int)mensaje_recibido[index + 1] + 2 + index;
            }
        }
        return 1;
    }
    else if (type == 2)
    {
        option = 0x33;
        long long int time = 0;
        while (1)
        {
            if ((unsigned char)mensaje_recibido[index] == option)
            {
                int timeLength = mensaje_recibido[index + 1];
                time = 0;
                unsigned char auxArray[timeLength];
                for (int aux = 0; aux < timeLength; aux++)
                    auxArray[aux] = mensaje_recibido[index + 2 + timeLength - 1 - aux];
                memcpy(&time, auxArray, timeLength);
                break;
            }
            else
            {
                index = (unsigned int)mensaje_recibido[index + 1] + 2 + index;
            }
        }
        sprintf(resultado, "%lld", time);
        int len = strlen(resultado);
        resultado[len] = '\0';
        return 2;
    }
    else if (type == 3)
    {
        option = 0x36;
        int index = pos + 4;
        while (1)
        {
            if ((unsigned char)mensaje_recibido[index] == option)
            {
                sprintf(resultado, "%d.%d.%d.%d", (unsigned char)mensaje_recibido[index + 2], (unsigned char)mensaje_recibido[index + 3], (unsigned char)mensaje_recibido[index + 4], (unsigned char)mensaje_recibido[index + 5]);
                int len = strlen(resultado);
                resultado[len] = '\0';
                break;
            }
            else
            {
                index = (unsigned int)mensaje_recibido[index + 1] + 2 + index;
            }
        }
        return 3;
    }
    else if (type == 5)
    {
        option = 0x01;
        int index = pos + 4;
        while (1)
        {
            if ((unsigned char)mensaje_recibido[index] == option)
            {
                sprintf(resultado, "%d.%d.%d.%d", (unsigned char)mensaje_recibido[index + 2], (unsigned char)mensaje_recibido[index + 3], (unsigned char)mensaje_recibido[index + 4], (unsigned char)mensaje_recibido[index + 5]);
                int len = strlen(resultado);
                resultado[len] = '\0';
                break;
            }
            else
            {
                index = (unsigned int)mensaje_recibido[index + 1] + 2 + index;
            }
        }
        return 4;
    }
    return 0;
}
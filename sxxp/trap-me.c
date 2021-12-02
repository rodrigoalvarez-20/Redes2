#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define HELO "EHLO 10.0.1.2\r\n"
#define FROM "gestorsnmp@gns3.com"
#define TO "mantenimiento@laboratorio.com"
#define DTA "DATA\r\n"
#define QUIT "QUIT\r\n"

void notify(int *mSock, char *message)
{
    char msgTo[120], msgFrom[120], buffer[1024];

    sprintf(msgFrom, "MAIL FROM:<%s>\r\n", FROM);
    sprintf(msgTo, "RCPT TO:<%s>\r\n", TO);

    send(*mSock, HELO, strlen(HELO), 0);
    //read(mSock, buffer, 1024);
    //printf("%s\n", buffer);
    //memset(buffer, '\0', 1024);

    send(*mSock, msgFrom, strlen(msgFrom), 0);
    //read(mSock, buffer, 1024);
    //printf("%s\n", buffer);
    //memset(buffer, '\0', 1024);

    send(*mSock, msgTo, strlen(msgTo), 0);
    //read(mSock, buffer, 1024);
    //printf("%s\n", buffer);
    //memset(buffer, '\0', 1024);

    char *dta = "DATA\r\n";
    send(*mSock, dta, strlen(dta), 0);
    //read(mSock, buffer, 1024);
    //printf("%s\n", buffer);
    //memset(buffer, '\0', 1024);

    char msg[1024];
    sprintf(msg, "From:<%s>\r\nTo:<%s>\r\nSubject:Notificacion SNMP\r\n%s\r\n.\r\n", FROM, TO, message);
    send(*mSock, msg, strlen(msg), 0);
    //read(mSock, buffer, 1024);
    //printf("%s\n", buffer);
    //memset(buffer, '\0', 1024);

    //char *QUIT = "QUIT\r\n";
    send(*mSock, QUIT, strlen(QUIT), 0);
    printf("Se ha enviado el correo de notificacion\n");
}

int main(int argc, char *argv[])
{
    char trappy[2048];
    int tSock, mSock;
    struct sockaddr_in trpSock, mailSock;

    tSock = socket(AF_INET, SOCK_DGRAM, 0);
    mSock = socket(AF_INET, SOCK_STREAM, 0);

    if (tSock < 0)
    {
        perror("Ha ocurrido un error al crear el socket para los Traps.\n");
        exit(1);
    }

    if (mSock < 0)
    {
        perror("Ha ocurrido un error al crear el socket de correo\n");
        exit(1);
    }

    memset((char *)&trpSock, 0, sizeof(trpSock));
    memset((char *)&mailSock, 0, sizeof(mailSock));

    trpSock.sin_family = AF_INET;
    trpSock.sin_addr.s_addr = inet_addr("10.0.1.1");
    trpSock.sin_port = htons(162); //162 es para las Traps
    int trpSize = sizeof(trpSock);

    mailSock.sin_family = AF_INET;
    mailSock.sin_addr.s_addr = inet_addr("10.0.1.2");
    mailSock.sin_port = htons(25);
    int mailSize = sizeof(mailSock);

    if (bind(tSock, (struct sockaddr *)&trpSock, trpSize))
    {
        perror("Ha ocurrido un error en el Bind del TRP.\n");
        exit(1);
    }

    if (connect(mSock, (struct sockaddr *)&mailSock, mailSize) < 0)
    {
        perror("Ha ocurrido un error al conectar con el servidor de correo\n");
        exit(1);
    }

    //int dLength = recvfrom(tSock, trappy, sizeof(trappy), 0, (struct sockaddr *)&tSock, &trpSize);

    //printf("%d\n", dLength);

    /* for (int i = 0; i < dLength; i += 2)
    {
        if (i != 0 && i % 32 == 0)
        {
            printf("\n");
        }
        printf("%.2x ", trappy[i]);
    } */

    notify(&mSock, "Prueba de correo");

    return 0;
}
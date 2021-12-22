#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
int datalen;
char databuf[1024];
struct pollfd pollfds[1];

int main(int argc, char *argv[])
{

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0)
  {
    perror("Error al crear el socket.");
    exit(1);
  }

  /*
   * Se inicializa la estructura sockaddr con una direccion en grupo de 224.1.1.1 y * en el puerto 5555
   */
  memset((char *)&groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  //groupSock.sin_addr.s_addr = inet_addr("224.1.1.1");
  groupSock.sin_addr.s_addr = inet_addr("224.0.0.1");
  groupSock.sin_port = htons(5555);

  /**
   * Para no escucharme a mi mismo
   */
  {
    char loopch = 0;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
    {
      perror("Error al asignar IP_MULTICAST_LOOP:");
      close(sd);
      exit(1);
    }
  }

  if (argv[1] != NULL) //Si le paso una direccion via parametro, la utilizo
    localInterface.s_addr = inet_addr(argv[1]);
  else
    localInterface.s_addr = INADDR_ANY; //Si no, ocupo la 0.0.0.0

  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
  {
    perror("Error al crear la interfaz local");
    exit(1);
  }

  strcpy(databuf, "Hola multicast");
  datalen = strlen(databuf);
  if (sendto(sd, databuf, datalen, 0, (struct sockaddr *)&groupSock, sizeof(groupSock)) < 0)
  {
    perror("Error al enviar el mensaje");
  }
/* 
  memset(databuf, '\0', datalen);

  pollfds[0].fd = sd;
  pollfds[0].events = POLLIN;

  int pollRes = poll(pollfds, 1, 15000);

  if (pollRes > 0)
  {
    if (pollfds[0].events & POLLIN)
    {
      if (read(sd, databuf, datalen) < 0)
      {
        perror("Error al recibir el mensaje del servidor");
        close(sd);
        exit(-1);
      }

      // Voy a recibir IP:PUERTO
      //Ocupar STRTOK

      printf("Data Received:%s\n", databuf);
    }
  }
  else
  {
    printf("Tiempo de espera agotado\n");
    exit(-1);
  } */

  return 0;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024], ipDir[INET_ADDRSTRLEN];

int main(int argc, char *argv[])
{

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0)
  {
    perror("Error al crear el socket");
    exit(-1);
  }

  {
    int reuse = 1;

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
    {
      perror("Error al ocupar SO_REUSEADDR");
      close(sd);
      exit(-1);
    }
  }

  memset((char *)&localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(5555);

  if (argv[1] != NULL) //Si le paso una direccion via parametro, la utilizo
    localSock.sin_addr.s_addr = inet_addr(argv[1]);
  else
    localSock.sin_addr.s_addr = INADDR_ANY; //Si no, ocupo la 0.0.0.0

  inet_ntop(AF_INET, &localSock.sin_addr.s_addr, ipDir, sizeof(ipDir));

  printf("IP: %s\n", ipDir);

  if (bind(sd, (struct sockaddr *)&localSock, sizeof(localSock)))
  {
    perror("binding datagram socket");
    close(sd);
    exit(1);
  }

  //La interfaz debe de ser aquella donde se vayan a recivir los paquetes
  //group.imr_multiaddr.s_addr = inet_addr("224.1.1.1");
  group.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
  //group.imr_interface.s_addr = inet_addr("8.40.1.213");
  group.imr_interface.s_addr = inet_addr("192.168.100.21");
  if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
  {
    perror("Error al asignar al grupo multicast");
    close(sd);
    exit(-1);
  }

  datalen = sizeof(databuf);
  if (read(sd, databuf, datalen) < 0)
  {
    perror("Error al leer el mensaje");
    close(sd);
    exit(-1);
  }

  printf("Data: %s\n", databuf);
  if (!strcmp(databuf, "GET_RPC"))
  {
    FILE *rcpFp;
    rcpFp = popen("find /bin -name rpcgen", "r");
    if (rcpFp == NULL)
    {
      printf("Ha ocurrido un error al buscar el servidor RCP");
      exit(-1);
    }

    char rcpPath[1024];

    while (fgets(rcpPath, sizeof(rcpPath), rcpFp) != NULL)
    {
      printf("%s", rcpPath);
    }

    fclose(rcpFp);

  }

  return 0;
}
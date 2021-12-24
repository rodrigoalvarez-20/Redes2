#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct sockaddr_in serverSocket; // Socket del servidor, por el cual vamos a enviar la respuesta
struct ip_mreq mGroup;           // Grupo de multicast
int sd;                          // Socket descriptor
int datalen;
char databuf[1024], ipDir[INET_ADDRSTRLEN];

/**
 * Inicializamos el descriptor de socket
 */

void initSD()
{
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0)
  {
    perror("Error al crear el socket");
    exit(-1);
  }

  int reuse = 1;

  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
  {
    perror("Error al ocupar SO_REUSEADDR");
    close(sd);
    exit(-1);
  }
}

/**
 * Inicializamos el Socket del servidor
 */

void initServerSocket(char *ip)
{
  memset(&serverSocket, 0, sizeof(serverSocket));
  serverSocket.sin_family = AF_INET;
  serverSocket.sin_port = htons(4321); // Es el mismo puerto que el del grupo multicast
  if (ip != NULL)                      // La direccion puede ser cualquiera, asi que se la podemos pasar por parametro
    serverSocket.sin_addr.s_addr = inet_addr(ip);
  else
    serverSocket.sin_addr.s_addr = INADDR_ANY;
}

/**
 * Hacemos el bind del socket para poder escuchar y enviar datos
 */

void bindSocket()
{
  if (bind(sd, (struct sockaddr *)&serverSocket, sizeof(serverSocket)))
  {
    perror("Error al hacer el bind");
    close(sd);
    exit(1);
  }
}

/**
 * Ajustamos las opciones del grupo de multicast
 */

void setMCOpts()
{
  mGroup.imr_multiaddr.s_addr = inet_addr("226.1.1.1");      // La direccion del grupo multicast
  mGroup.imr_interface.s_addr = inet_addr("192.168.100.21"); //INADDR_ANY; // La direccion de la tarjeta por la cual va a recibir los datos. Ocupamos cualquier direccion
  if (
      setsockopt(
          sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mGroup, sizeof(mGroup)) < 0)
  {
    perror("Error en las opciones del grupo multicast");
    exit(-1);
  }
}

int main(int argc, char *argv[])
{

  initSD();
  initServerSocket(NULL);
  bindSocket();
  setMCOpts();

  int srvLen = sizeof(serverSocket);

  datalen = sizeof(databuf);

  if (read(sd, databuf, datalen) < 0)
  {

    perror("Error al obtener el mensaje");
    exit(1);
  }

  printf("%s\n", databuf);

  if (!strcmp(databuf, "GET_RPC")) // Validammos si es el comando esperado
  {
    FILE *rpcFp;
    char response[1024] = "";
    rpcFp = popen("rpcinfo -p", "r"); // Consultamos a rpcinfo para obtener la lista de aplicaciones RPC registradas en el servidor o maquina
    if (rpcFp == NULL)
    {
      printf("Ha ocurrido un error al buscar el servidor RCP");
      exit(-1);
    }

    char rpcPath[1024];
    char *delimData = " "; 

    while (!feof(rpcFp))
    {
      if (fgets(rpcPath, 1024, rpcFp) != NULL)
      {
        //printf("%s", rpcPath); // Vean la salida o ejecuten el comando para saber porque hago esto
        char *prog, *proto, *serv;
        int vers, port;
        prog = strtok(rpcPath, delimData); // Obtengo el numero de programa
        vers = atoi(strtok(NULL, delimData)); // Version
        proto = strtok(NULL, delimData); // Protocolo
        port = atoi(strtok(NULL, delimData)); // Puerto en el que corre
        serv = strtok(NULL, delimData); // Servicio (Nombre)
        //printf("%s - %d - %s - %d - %s \n", prog, vers, proto, port, serv);
        if (strcmp(prog, "100000") != 0 && port != 0) // El programa 100000 es el por defecto, asi que lo evitamos
        {
          strcat(response, ipDir);
          strcat(response, ":");
          char p[10];
          sprintf(p, "%d", port);
          strcat(response, p);
          strcat(response, ";");
          //Formateamos el mensaje
        }
      }
    }
    fclose(rpcFp); //Cerramos el buffer

    strcpy(response, "Hola cliente");
    //response = "Hola Client";

    initSD();
    initServerSocket(argv[1]);
    bindSocket();

    inet_ntop(AF_INET, &serverSocket.sin_addr.s_addr, ipDir, sizeof(ipDir)); // Truco para obtener la direccion IP de una interfaz ya creada

    printf("%s\n", ipDir);

    printf("Sending %s ...\n", response);

    if (sendto(sd, response, sizeof(response), 0, (struct sockaddr *)&serverSocket, sizeof(serverSocket)) < 0) // Enviamos la respuesta
    {
      perror("Error al enviar el mensaje");
    }
  }

  return 0;
}
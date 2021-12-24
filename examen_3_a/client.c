#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

//Sockets a ocupar
struct sockaddr_in multicastGroupSocket, localSocket; // Multicast para enviar el paquete de multidifusion y el local para poder recibir una respuesta
struct in_addr localInterface;
int sd;             // Socket descriptor
int datalen;        // Tamaño de los datos, tanto enviados como recibidos (no cambia)
char databuf[1024]; // Buffer de datos (enviar y recibir)
struct pollfd pollfds[1];

/**
 * Funcion que permite inicializar el descriptor de sockets
 */

void initSD()
{
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (!sd)
  {
    perror("Ha ocurrido un error al crear el socket");
  }
}

/**
 * Funcion que rellena los valores del socket de multidifusion
 * Es importante que el puerto y la direccion IP sean iguales aqui y en el server
 * Nota: La direccion Multicast a veces varia dependiendo las redes
 */

void initMCS()
{
  memset(&multicastGroupSocket, '\0', sizeof(multicastGroupSocket));

  multicastGroupSocket.sin_family = AF_INET;
  multicastGroupSocket.sin_port = htons(4321);
  multicastGroupSocket.sin_addr.s_addr = inet_addr("226.1.1.1");
}

/**
 * Funcion que omite el reenvio de paquetes a la direccion inicial
 */

void setSockOpts()
{
  localInterface.s_addr = inet_addr("192.168.100.21");
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
  {
    perror("Error al configurar la interfaz local");
    exit(1);
  }
}

/**
 * Funcion que permite mandar un mensaje de multidifusion
 */

void sendMCMSG()
{
  strcpy(databuf, "GET_RPC"); //TODO cambiar este valor aqui y en el servidor
  datalen = strlen(databuf);
  if (sendto(sd, databuf, datalen, 0, (struct sockaddr *)&multicastGroupSocket, sizeof(multicastGroupSocket)) < 0)
  {
    perror("Error al enviar el mensaje");
  }
  //printf("%d\n", datalen);
  memset(databuf, '\0', datalen);
}

/**
 * Funcion que inicializa el socket local
 * @param ip Valor que se le pasa al programa por parametro. Esta va a ser la direccion IP por donde recibamos los datos de respuesta
 */

void initLocalSocket(char *ip)
{
  memset((char *)&localSocket, 0, sizeof(localSocket));
  localSocket.sin_family = AF_INET;
  localSocket.sin_port = htons(4321); // El puerto puede ser cualquiera, pero, #Vue+Python Dev
  if (ip != NULL)
    localSocket.sin_addr.s_addr = inet_addr(ip);
  else
    localSocket.sin_addr.s_addr = INADDR_ANY;
}

/**
 * Funcion que permite hacer el bind del socket local. Esto nos permite escuchar a las conexiones
 */

void bindLocalSocket()
{
  if (!bind(sd, (struct sockaddr *)&localSocket, sizeof(localSocket)))
  {
    perror("Ha ocurrido un error al hacer el bind local");
    exit(-1);
  }
}

int main(int argc, char *argv[])
{

  initSD();
  initMCS();
  setSockOpts();

  sendMCMSG();

  initLocalSocket(argv[1]);
  bindLocalSocket();
  //memset(&databuf, '\0', sizeof(databuf));

  int srvLen = sizeof(localSocket);

  // Una vez enviado el mensaje o comando, obtenemos los datos
  // El servidor nos regresa una cadena en formato 'DIRECCION_IP:PUERTO;' la cual contiene todos los datos de las aplicaciones RPC en el servidor

  pollfds[0].fd = sd;
  pollfds[0].events = POLLIN;

  int pollres = poll(pollfds, 1, 15000);

  if (pollres > 0)
  {
    if (pollfds[0].revents & POLLIN)
    {

      if (read(sd, databuf, datalen) < 0)
      {

        perror("Error al obtener el mensaje");
        exit(1);
      }

      printf("%s\n", databuf);

      /* if (!recvfrom(sd, databuf, sizeof(databuf), 0, (struct sockaddr *)&localSocket, &srvLen))
      {
        perror("Error al recibir los datos");
        exit(-1);
      }

      char delimIPs[] = ";"; // Delimitador para parsear el string

      char *tk; //Token

      printf("%s\n", databuf);  // Descomentar por si quieren ver los datos que reciben

      tk = strtok(databuf, delimIPs); // Iniciamos con el primer token

      while (tk != NULL)
      {                                                  // Mientras la cadena aun tenga un valor
        printf("Aplicacion RPC encontrada en %s\n", tk); // Mostramos la informacion
        tk = strtok(NULL, delimIPs);                     //Siguiente token
      } */
    }
  }
  else
  {
    printf("Tiempo de espera agotado\n");
  }

  printf("Hasta luego\n"); // Fin

  return 0;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


struct in_addr        localInterface;
struct sockaddr_in    sck, sckR;
int                   sd;
int                   datalen;
char                  message[1024], messageRecv[1024];
struct ip_mreq        group;

int main(){
    sleep(1);
    scanf("%[^\n]s", message);
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("opening datagram socket");
        exit(1);
    }
   
    memset((char*)&sck, 0, sizeof(sck));
    sck.sin_family = AF_INET;
    sck.sin_addr.s_addr = inet_addr("224.0.0.30");
    sck.sin_port = htons(8000);

    /*
    * Disable loopback so you do not receive your own datagrams.
    */
    {
    char loopch = 0;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loopch, sizeof(loopch)) < 0) {
        perror("setting IP_MULTICAST_LOOP:");
        close(sd);
        exit(1);
        }
    }

    //Direccion hacia donde se quiere enrutar los paquetes Multicast
    localInterface.s_addr = inet_addr("192.168.100.100");
    //localInterface.s_addr = inet_addr("192.168.100.72");
    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localInterface, sizeof(localInterface)) < 0) {
        perror("setting local interface");
        exit(1);
    }

    //Enviar el mensaje
    datalen = strlen(message);
    message[datalen] = '\0';
    if (sendto(sd, message, datalen, 0, (struct sockaddr*)&sck, sizeof(sck)) < 0) {
        perror("sending datagram message");
    }

    //Ahora toca escuchar

    {
        int reuse = 1;

        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
            perror("setting SO_REUSEADDR");
            close(sd);
            exit(1);
        }
    }

    memset(&sckR, 0, sizeof(sckR));
    sckR.sin_family = AF_INET;
    sckR.sin_addr.s_addr = inet_addr("224.0.0.31");
    sckR.sin_port = htons(8500);

    int sdR = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sdR, (struct sockaddr*)&sckR, sizeof(sckR))) {
        perror("binding datagram socket");
        close(sdR);
        exit(1);
    }

    group.imr_multiaddr.s_addr = inet_addr("224.0.0.31");
    group.imr_interface.s_addr = inet_addr("192.168.200.200");
    //group.imr_interface.s_addr = inet_addr("192.168.100.72");
    if (setsockopt(sdR, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group, sizeof(group)) < 0) {
        perror("adding multicast group");
        close(sdR);
        exit(1);
    }

    while (1) {
        int addrlen = sizeof(sckR);
        int nbytes = recvfrom(sdR, messageRecv, 1024, 0, (struct sockaddr *) &sckR, &addrlen);
        if (nbytes < 0) {
            perror("recvfrom");
            return 1;
        }
        messageRecv[nbytes] = '\0';
        printf("%s %s", messageRecv, inet_ntoa(sckR.sin_addr));
        break;
    }

    return 0;
}
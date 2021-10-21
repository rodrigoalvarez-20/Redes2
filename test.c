#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
//#include <winsock2.h>

typedef struct RQ
{
	//Hay que hacerlo portable, no siempre short int es de 2 bytes
	short int opCode;
	char * filename;
	char * mode;
} request;

typedef struct DataPacket
{
	short int opCode;
	short int blockNumber;
	char * data;
}packet;

typedef struct ACKPacket
{
	short int opCode;
	short int blockNumber; 
} ack;

typedef struct ERRORPacket
{
	short int opCode;
} ErrorPack;
int main(int argc, char const *argv[])
{
	char buffer [512];
    const char archivo[100] = "archivo1.txt";
    const char octetMode[] = "octet";
	int16_t * opCode = (int16_t *)buffer;
	char *filename = buffer + 2;
    char *modeOp, *endOfHeader;
	int i=2, dataLength, length;

	*opCode = htons(1);

    sprintf(filename, "%s", archivo);
    //modeOp = buffer + strlen(archivo) + 2;
    modeOp = filename + strlen(filename) + 1;
    sprintf(modeOp, "%s", octetMode);

    printf("%s\n", filename);

	endOfHeader = modeOp + strlen(modeOp) + 1;

	length = endOfHeader - buffer; //Modo 1

	length = 2 + strlen(filename) + strlen(modeOp) + 2; //Modo 2

	//dataLength = strlen(endOfHeader) - strlen(buffer);

	/* printf("Total: %d\n", length);

    for(int i = 0; i < 24; i++)
        printf("%02X ", buffer[i]);

    printf("\n"); */

	//Envio el paquete

	//request * r;
	//sendto(fd, r, sizeof(*r), 0, (sockaddr_in *) socket, sizeof(socket));


	//Recibo y mando el acuse
	char data[550]; // 2 + 2 + 512 
	int blockNumber = 1, reciveBlockNumber = 0;
	while(1){
		//Si es menor a 512 los datos, hago un break

		//Funcion para recibir datos

		//Aqui va el rcvfrom hacia el buffer
		//Poner un timer para esperar un rato
		//Si falla, reenviar solicitud

		//El numero de bloque esta en data + 2 y son 2 bytes
		reciveBlockNumber = htons(*((int16_t *) data + 2));

		char ack[4];

		opCode = (int16_t *)ack;

		*opCode = htons(0x04);

			//

		memcpy(ack + 2, data + 2, 2);

		//Solo guardar cuando sea diferente el bloques

		if(reciveBlockNumber != blockNumber){
			// No es el que estoy esperando
		}else {
			// Es el que estoy esperando, guardar a archivo y mandar acuse y esperar
			
		}
		//Mandar el acuse


		//for(int i = 0; i < 4; i++)
        //	printf("%02X ", ack[i]);

	}


	return 0;
}
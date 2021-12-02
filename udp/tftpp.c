#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>

#define OP_CODE_ASCII "octet"

int dSock, dLength, bLength;
char buff[512], *filename, *modeOp, *endOfHeader;
int16_t *operationCode = (int16_t *)buff;
struct sockaddr_in sck;
struct pollfd pollfds[1];

void configDGramToSend(char const *argv[]);

int main(int argc, char const *argv[])
{

	if (argc < 4)
	{
		printf("Parametros invalidos.\nUso adecuado: ./tftpp host metodo archivo\n");
		exit(-1);
	}

	//printf("%s - %s - %s\n", argv[1], argv[2], argv[3]);

	dSock = socket(AF_INET, SOCK_DGRAM, 0); //Creacion del socket

	if (dSock < 0)
	{
		perror("Ha ocurrido un error al crear el socket.\n");
		exit(-1);
	}

	memset((char *)&sck, 0, sizeof(sck));
	sck.sin_family = AF_INET;
	sck.sin_addr.s_addr = inet_addr(argv[1]);
	sck.sin_port = htons(69);

	configDGramToSend(argv);

	dLength = sendto(dSock, buff, bLength, 0, (struct sockaddr *)&sck, sizeof sck);

	if (dLength < 0)
	{
		printf("Ha ocurrido un error al mandar los datos\n");
		exit(-1);
	}

	//printf("Count: %d\n", dLength);

	FILE *fp;

	fp = fopen(argv[3], "w");

	if (fp == NULL)
	{
		printf("Ha ocurrido un error al crear el archivo\n");
		exit(-1);
	}

	char dataRcv[550];
	int16_t expectedBlock = 1, actualBlock = 0;

	pollfds[0].fd = dSock;
	pollfds[0].events = POLLIN;

	do
	{
		int srvLen = sizeof sck;
		//Iniciar el contador
		printf("Esperando datos...\n");

		int pollRes = poll(pollfds, 1, 10000);

		if (pollRes > 0)
		{
			if (pollfds[0].revents & POLLIN)
			{
				dLength = recvfrom(dSock, buff, 600, 0,
								   (struct sockaddr *)&sck, &srvLen);
				//Si no he recibido una respuesta, crear una interrupcion
				int rcvOpCode = (int)buff[1];
				actualBlock = (int)buff[3];

				if (rcvOpCode == 5)
				{
					printf("Ha ocurrido un error al obtener los datos\n");
					fclose(fp);
					remove(argv[3]);
					exit(-1);
				}

				if (actualBlock == expectedBlock)
				{
					expectedBlock = actualBlock + 1;
					//Significa que si me envia una nueva parte del archivo
					//Guardar a archivo
					//const char data[512];
					void *data = malloc(512);
					memcpy(data, buff + 4, 512);
					//printf("%s", data);
					fputs(data, fp);
				}

				char ack[4];
				operationCode = (int16_t *)ack;
				*operationCode = htons(0x04);
				memcpy(ack + 2, buff + 2, 2);

				memset(buff, 0, 512);

				sendto(dSock, ack, 4, 0, (struct sockaddr *)&sck, sizeof sck); //Envio el acuse
			}
		}
		else
		{
			printf("Tiempo de espera agotado\n");
			fclose(fp);
			//close(dSock);
			remove(argv[3]);
			exit(-1);
		}

	} while (dLength == 516);

	fclose(fp);
	//close(dSock);

	printf("El archivo se ha obtenido con exito!!\n");

	return 0;
}

void configDGramToSend(char const *argv[])
{
	filename = buff + 2;
	*operationCode = htons(1);

	sprintf(filename, "%s", argv[3]);
	modeOp = filename + strlen(filename) + 1;
	sprintf(modeOp, "%s", OP_CODE_ASCII);
	endOfHeader = modeOp + strlen(modeOp) + 1;

	bLength = endOfHeader - buff; //Modo 1
}
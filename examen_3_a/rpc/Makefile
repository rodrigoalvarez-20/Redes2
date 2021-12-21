.SUFFIXES:
.SUFFIXES: .c .o
CLNT = out/rpcy_clt # Ruta/Nombre del archivo de salida del cliente
SRVR = out/rpcy_srv # Ruta/Nombre del archivo de salida del servidor
CFLAGS = -g -Wall -lnsl #Banderas de compilacion

SRVR_OBJ = rpcy_srv.o rpcy_svc.o  # Instruccion de compilacion de los archivos del servidor (Generamos los .o)
CLNT_OBJ = rpcy_clt.o rpcy_clnt.o # Igual que arriba pero del cliente

.c.o:; gcc -c -o $@ $(CFLAGS) $<  ## Meta instruccion de lo que va a hacer el compilador, configuramos las flags

default: $(CLNT) $(SRVR) ## Nombre de instruccion y lo que se va a ejecutar, en este caso ejecuta la instruccion de CLNT y SRVR

$(CLNT): $(CLNT_OBJ) ## Comando Cliente y lo que va a realizar (al final limpiamos los archivos .o)
	gcc -o $(CLNT) $(CLNT_OBJ) && rm $(CLNT_OBJ)

$(SRVR): $(SRVR_OBJ) ## Comando Servudir y lo que va a realizar (al final limpiamos los archivos .o)
	gcc -o $(SRVR) $(SRVR_OBJ) && rm $(SRVR_OBJ)
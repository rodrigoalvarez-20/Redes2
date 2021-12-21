#### No entiendo mucho de Makefiles, pero, aqui va la explicacion
## Esto es lo equivalente a compilarlos a mano

.SUFFIXES:
.SUFFIXES: .c .o
<<<<<<< HEAD
CLNT = val_clt
SRVR = val_srv
CFLAGS = -g -Wall -lnsl
=======
>>>>>>> 11810996ad2e8bad0acf087df0e98d13da00cefc

CLNT = val_clt  ## Archivo ejecutable de salida (cliente - Debian-1)
SRVR = val_srv  ## Archivo ejecutable de salida (servidor - Debian-2)
CFLAGS = -g -Wall -lnsl ### Flags para compilar el archivo, en realidad solo necesitamos -lnsl, pero ps entre mas haya, mas cabrones nos vemos

## Asignamos a una variable los distintos archivos

# Objeto del servidor: 
# Este objeto y la compilacion de abajo la pueden comentar si estan en Debian-1, ya que es el cliente
# val_srv_proc.o --> Archivo de funcionamiento del servidor (programado por nosostros)
# validator_xdr.o --> Archivo que genera el rpcgen cuando tenemos un tipo de dato complejo
# validator_svc.o --> Archivo generado por RPCGEN, no tocar
SRVR_OBJ = val_srv_proc.o validator_xdr.o validator_svc.o

# Objeto del cliente: 
# Este objeto y la compilacion de abajo la pueden comentar si estan en Debian-2, ya que es el servidor
# val_clt.o --> Archivo que contiene todo el custom code de lado del cliente, como abrir archivos, declarar funciones
# validator_xdr.o --> Archivo que genera el rpcgen cuando tenemos un tipo de dato complejo
# validator_clnt.o --> Archivo generado por RPCGEN, no tocar
CLNT_OBJ = val_clt.o validator_xdr.o validator_clnt.o


## Aqui creo que dice que compilemos a archivos .o
.c.o:; gcc -c -o $@ $(CFLAGS) $<

# "Nombre de la configuracion": "rutina 1" "rutina 2" "rutina n"
default: $(CLNT) $(SRVR)

## Revisen arriba la variable
## Compilamos los archivos .o a un ejecutable
$(CLNT): $(CLNT_OBJ)
	gcc -o $(CLNT) $(CLNT_OBJ)


##Igual
$(SRVR): $(SRVR_OBJ)
	gcc -o $(SRVR) $(SRVR_OBJ)
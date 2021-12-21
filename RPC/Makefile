.SUFFIXES:
.SUFFIXES: .c .o
CLNT = val_clt
SRVR = val_srv
CFLAGS = -g -Wall -lnsl

SRVR_OBJ = val_srv_proc.o validator_xdr.o validator_svc.o
CLNT_OBJ = val_clt.o validator_xdr.o validator_clnt.o

.c.o:; gcc -c -o $@ $(CFLAGS) $<

default: $(CLNT) $(SRVR)

$(CLNT): $(CLNT_OBJ)
	gcc -o $(CLNT) $(CLNT_OBJ)

$(SRVR): $(SRVR_OBJ)
	gcc -o $(SRVR) $(SRVR_OBJ)
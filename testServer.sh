#!/bin/bash
#Ciclo for como en C
#$1 representa el numero de veces que se desea ejecutar
if [ $# -lt 2 ]
then
    echo "Por favor introduce los 2 parametros necesarios: sh testServer.sh numero_vueltas archivo_ejecutar"
else
    for (( c=1; c<=$1; c++ ))
    do #Respeta la identacion, no se puede subir xD
        ./$2 & #Ejecuto el archivo "$2", que es pasado por argumento
        echo "" #Salto de linea simple
    done
fi
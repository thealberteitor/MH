#!/bin/bash

#Comentar uno de los dos y ejecutar ./execute.sh para ejecutar todos los ficheros a la vez
#ejecutar=ES
#ejecutar=BMB
#ejecutar=ILS
ejecutar=ILS-ES

#Semilla usada
seed=54142189


#Directorios
dir_cpp="fuentes/$ejecutar.cpp"
dir_bin="bin/$ejecutar"
dir_instancias="instancias"
GCC="g++ -O2"

$GCC $dir_cpp -o $dir_bin

echo "Ejecutanto: $ejecutar"

for i in $(ls ./$dir_instancias -C1)
do
	./$dir_bin  $seed   $dir_instancias/$i 
done


#Si se desea borrar el ejecutable descomentar la siguiente linea
#rm $dir_bin

#!/bin/bash

#Comentar uno de los dos y ejecutar ./execute.sh para ejecutar todos los ficheros a la vez
ejecutar=greedy
#ejecutar=BL


#Semilla usada
seed=54142189


#Directorios
dir_cpp="FUENTES/$ejecutar.cpp"
dir_bin="BIN/$ejecutar"
dir_instancias="instancias"
GCC="g++ -O2"

$GCC $dir_cpp -o $dir_bin

for i in $(ls ./$dir_instancias -C1)
do
	if [ $ejecutar == "BL" ]
	then
		./$dir_bin  $seed   $dir_instancias/$i 
	else
		./$dir_bin   $dir_instancias/$i 
	fi
done


#Si se desea borrar el ejecutable descomentar la siguiente linea
#rm $dir_bin

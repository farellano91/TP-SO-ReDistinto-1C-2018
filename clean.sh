#!/bin/bash
echo ""
echo ...::: Iniciando limpieza de carpetas de montaje :::...
echo ""

echo Borramos las carpetas de montaje de las instancias
rm -rf /home/utnso/inst*

echo Borramos el log del coordinador
cd coordinador/src
rm -rf log\ de\ operaciones.log 
cd ..
cd ..

echo Borramos el log del planificador
cd planificador/src
rm -rf log\ de\ operaciones.log 
cd ..
cd ..

echo ""
echo ...::: Proceso de limpieza de carpetas terminado correctamente :::...
echo ""

/*
 ============================================================================
 Name        : Prueba_arg.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>

int main(int argc, char *argv[])
{
	//argc: cantidad de parametros + el de la funcion
	//listado de parametros + la funcion
	int i;
	for(i = 0 ; i < argc ; i++){
	   printf("Parametro[%d]: %s\n", i , argv[i]);
   }

   return 0;
}
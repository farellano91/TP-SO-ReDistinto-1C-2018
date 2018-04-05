/*
 ============================================================================
 Name        : guia_ejercicio_3|.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

//Ejercicio 3 (Uso de Commons-Library)
//
//Dado un archivo de texto que contiene un conjunto de personas,
//leerlo, procesarlo y generar la salida especificada. Recomendación:
//Revisar las funciones provistas en la biblioteca commons-library de la cátedra.
//
//Archivo de entrada:
//●	Formato => Región; Nombre y Apellido; Edad; Número Telefónico; DNI; Saldo
//●	Cada renglón representa una persona, cada campo de la persona se encuentra
//separado por un ‘;’ como delimitador (por lo tanto tienen campos de longitud variable).
//●	El archivo no sigue ningún orden.
//
//Archivo de salida:
//●	Formato => Región | Edad | DNI | Nombre y Apellido (30 chars máximo) | Número telefónico
//●	Cada renglón representa una persona.
//●	El archivo debe estar ordenado por Región y Edad.
//●	Filtrar los menores de edad (< 18 años).
//●	Loggear las personas cuyo saldo sea menor a 100$.


#include <stdio.h>
#include <stdlib.h>
#include "archivo.h"

int main(void) {
	//Creo el archivo
	FILE * archivo = txt_open_file("prueba_archivo.txt","w");
	//controlo que este bien
	if (!archivo){
		puts("Error al crear el archivo");
		return EXIT_SUCCESS;
	}
	//primera carga
	txt_carga_inicial(archivo);
	txt_close_file(archivo);

	//Leo el archivo
	archivo = txt_open_file("prueba_archivo.txt","r");
	//controlo que este bien
	if (!archivo){
		puts("Error al ver el archivo");
		return EXIT_SUCCESS;
	}
	txt_read_all(archivo);
	txt_close_file(archivo);


	//BUsco el de posicion cualquiera
	archivo = txt_open_file("prueba_archivo.txt","r");
	//controlo que este bien
	if (!archivo){
		puts("Error al buscar poscion en archivo");
		return EXIT_SUCCESS;
	}
	txt_read_especific_line(archivo, 3);
	txt_close_file(archivo);


	/*----------------------------------3---------------------------------------------------*/

	/*Almacenarlo en una lista*/
	archivo = txt_open_file("prueba_archivo.txt","r");
	if (!archivo){
		puts("Error al Almacenarlo");
		return EXIT_SUCCESS;
	}
	t_list * lista_personas = txt_save_in_list(archivo);


	/*Ordenar la lista*/
	txt_order_list(lista_personas);

	/*Guardar la lista en una nueva tabla*/
	txt_save_in_file(lista_personas);

	txt_clear_list(lista_personas);
	txt_close_file(archivo);

	return EXIT_SUCCESS;
}
/*
 * archivo.c
 *
 *  Created on: 31/3/2018
 *      Author: utnso
 */
#include "archivo.h"


FILE* txt_open_file(char* path, char* condicion) {
	return fopen(path,condicion);
}

//caso donde cada linea tiene como maximo 30 caracteres
void txt_carga_inicial(FILE* file){
	int i;
	for(i = 0; i < 10; i++){
		char* fila = malloc(sizeof(char) * 8);
		strcpy(fila,"nota1\n");
		fwrite(fila,sizeof(char),strlen(fila),file);
		free(fila);
	}


}

void txt_close_file(FILE* file) {
	fclose(file);
}

void txt_read_all(FILE* file){
	char* line = malloc(sizeof(char)*8);
	size_t maxl = 8;

	while (fgets(line, maxl, file)) {
		while(line[strlen(line) - 1] != '\n' && line[strlen(line) - 1] != '\r'){
			char *tmp = realloc (line, 2 * maxl * sizeof(char));

			fseek(file,0,SEEK_SET);          //or wherever you want to seek to
			if (tmp) {
				line = tmp;
				maxl *= 2;
				fgets(line, maxl, file);
			}
			else{
				printf("Not enough memory for this line!!\n");
			}
		}
		printf("%s",line);
	}

//	char* line = malloc(sizeof(char)*21);
//	fread(line,sizeof(char),21,file);
//	printf("%s",strsep(&line,"t"));
//	while(!feof(file)){
//		printf("%s",strsep(&line,"\0"));
//
//		printf("%s\n",line);
//
//		fread(line,sizeof(char),21,file);
//	}
}

void txt_read_especific_line(FILE* file, int line_number){
	//Posiciona al puntero en el registro pedido
	//1 archivo, 2 posicion x tamaño de cada linea, 3 SEEK_SET
	fseek(file,line_number*31,SEEK_SET);
	char * fila = malloc(sizeof(char)*31);
	fgets(fila, 31, file);
	printf("Fila: %s\n",fila);


}
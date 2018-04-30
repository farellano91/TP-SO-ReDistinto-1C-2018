#ifndef FUNCIONALIDAD_INSTANCIA_H_
#define FUNCIONALIDAD_INSTANCIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <commons/collections/list.h>

char* IP_CONFIG_COORDINADOR;
int PUERTO_CONFIG_COORDINADOR;
char* ALGORITMO_REEMPLAZO;
char* PUNTO_MONTAJE;
char* NOMBRE_INSTANCIA;
int INTERVALO_DUMP;


//Cargo los parametros desde el archivo config y los libero conforme deje de usarlos
void get_parametros_config();

void free_parametros_config();

//Recibo los datos para mis entradas (para armar el storage)
void recibo_datos_entrada(int sockfd);

//Envio mis datos al coordinador
void envio_datos(int sockfd);


int TAMANIO_ENTRADA;

int CANT_ENTRADA;

//Para tener la tabla de entradas
t_list* TABLA_ENTRADA;

typedef struct {
	int clave;
	int numero_entrada;
	int tamanio_variable;
} t_fila_tabla_entrada;

int recibo_sentencia(int fd_coordinador);

void envio_resultado_al_coordinador(int sockfd,int resultado);

#endif /* FUNCIONALIDAD_INSTANCIA_H_ */

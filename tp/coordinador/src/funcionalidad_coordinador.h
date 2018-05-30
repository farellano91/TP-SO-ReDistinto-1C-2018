/*
 * funcionalidades_coordinador.h
 *
 *  Created on: 18 abr. 2018
 *      Author: utnso
 */

#ifndef FUNCIONALIDAD_COORDINADOR_H_
#define FUNCIONALIDAD_COORDINADOR_H_

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
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include <math.h>

pthread_mutex_t MUTEX;

pthread_cond_t CONDICION_LIBERO_PLANIFICADOR;

pthread_cond_t CONDICION_RECV_INSTANCIA;

pthread_mutex_t MUTEX_RECV_INSTANCIA;

pthread_cond_t CONDICION_INSTANCIA;

pthread_mutex_t MUTEX_INSTANCIA;

//Para planificador
int FD_PLANIFICADOR;

t_log * LOGGER;

int PUERTO_ESCUCHA_CONEXION;
char* ALGORITMO_DISTRIBUCION;
int32_t CANTIDAD_ENTRADAS;
int32_t TAMANIO_ENTRADA;
int RETARDO;

typedef struct {
	int fd;
	char* nombre_instancia;
	int tamanio_libre;
} t_Instancia;

//Variables del COODINADOR
int INDEX; /* esta variable no se debe tocar en otro lado q no sea el algoritomo de distribucion  (podriamos por un mutex)*/
t_Instancia* equitativeLoad(char** resultado);
t_Instancia*  LeastSpaceUsed(char** resultado);
t_Instancia*  keyExplicit(char** resultado);
//en variables del COORINADOR



//esto me sirve para armar mi tabla <nombre instancia,clave> par guardar las claves que ya se crearon
typedef struct {
	char* nombre_instancia;
	char* clave;
} t_registro_instancia;

typedef struct {
	char* nombre_instancia;
	int respuesta;
	int tamanio_libre; //solo si la operacion es OK_SET_INSTANCIA se recibe un espacio libre actualizado
} t_instancia_respuesta;

int envio_tarea_instancia(int32_t id_operacion, t_Instancia * instancia,char** clave_valor);

t_Instancia* get_instancia_by_name(char* name);

void remove_registro_instancia( char * clave);

int envio_recibo_tarea_store_instancia(int32_t id_operacion, char* clave,t_Instancia *instancia);

//Cargo los parametros desde el archivo config y los libero conforme deje de usarlos
void get_parametros_config();

//libera todos los parametros que tenga
void free_parametros_config();

void configure_logger();

t_list* LIST_INSTANCIA_RESPUESTA;

t_list* LIST_INSTANCIAS;

t_list* LIST_REGISTRO_INSTANCIAS;

t_list* create_list();

void envio_datos_entrada(int fd_instancia);

t_Instancia* creo_instancia(int fd_instancia);

t_registro_instancia* creo_registro_instancia(char* nombre_instancia, char* clave);

t_instancia_respuesta* creo_instancia_respuesta(char* nombre_instancia,int respuesta,int tamanio_libre);

void agrego_instancia_lista(t_list* list,t_Instancia* instancia_nueva);

void inicializo_semaforos();

int aplicarAlgoritmoDisctribucion(char * algoritmo,char** resultado);

void envio_tarea_planificador(int32_t id_operacion,char* clave_recibida,int32_t id_esi);

void loggeo_info(int32_t id_operacion,int32_t id_esi,char* clave_recibida,char* valor_recibida);

void loggeo_respuesta(char* operacion, int32_t id_esi,int32_t resultado_linea);

int reciboRespuestaInstancia(int fd_instancia);

int reciboTamanioLibre(int fd_instancia);

void free_instancia(t_Instancia * instancia);

void free_registro_instancia(t_registro_instancia* registro_instancia);

void free_instancia_respuesta(t_instancia_respuesta* instancia_respuesta);

void remove_instancia(int fd_instancia);

void cargo_instancia_respuesta(char * instancia_nueva,int respuesta,int tamanio_libre);

bool exist_clave_registro_instancias(char * clave);

bool controlo_existencia(t_Instancia * instanciaNueva);

void send_mensaje_rechazo(t_Instancia * instancia_nueva);

int aplicar_filtro_respuestas(int resultado_linea);

void limpia_destruye_elemt_lista_respuesta_instancia();

enum t_tipo_cliente {
	ESI = 1, PLANIFICADOR = 2, INSTANCIA = 3,
};

enum t_respuesta_de_instancia {
	FALLO_INSTANCIA_CLAVE_SOBREESCRITA = 5, //
	OK_SET_INSTANCIA = 6,         //AMBOS OK DERIVAN EN SER 2 PARA AVISAR OK AL ESI
	OK_STORE_INSTANCIA = 7,
	ABORTA_ESI_CLAVE_INNACCESIBLE = 8, //Por instancia desconectada
	TODOS_COMPACTEN = 13,
};

enum t_respuesta_de_planificador {
	FALLO_PLANIFICADOR = 1, //planificador desconectado
	OK_PLANIFICADOR = 2,
	OK_BLOQUEADO_PLANIFICADOR = 3,
	ABORTA_ESI_CLAVE_NO_BLOQUEADA = 4
};

enum t_respuesta_de_coordinador {
	ABORTA_ESI_CLAVE_NO_IDENTIFICADA = 9,
	FALLA_ELEGIR_INSTANCIA = 10,
	FALLA_PLANIFICADOR_DESCONECTADO = 11,
	FALLA_SIN_INSTANCIA_CLAVE_STORE = 12,

};
enum t_respuesta_al_esi {
	ABORTA_ESI = 1,
	OK_ESI = 2, //los casos de error que no deriven el aborto segun el enunciado, los filtramos por aca, pero lo registramos en el log
	BLOQUEADO_ESI = 3,

};

enum t_operacion {
	GET = 1, //GET CLAVE
	SET = 2, //SET CLAVE VALOR
	STORE = 3, //STORE CLAVE
};

#endif /* FUNCIONALIDAD_COORDINADOR_H_ */

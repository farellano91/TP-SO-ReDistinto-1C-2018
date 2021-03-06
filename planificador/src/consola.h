/*
 * funciones_planificador.h
 *
 *  Created on: 15/4/2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "funcionalidad_planificador.h"


int control_parametros(char* arg,int cant_parametros);

void destroySplit(char** split);

void levantar_consola();

t_list* buscar_deadlock(t_list* bloqueadores,t_list* bloqueados);

char* recibo_instancia();

char* recibo_valor();

#endif /* CONSOLA_H_ */

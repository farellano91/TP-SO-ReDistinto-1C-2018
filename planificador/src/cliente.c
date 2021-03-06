#include "cliente.h"

//PD:Si ejecuto esto la ip del coordinador ya lo libere ;)
void saludo_inicial_coordinador(int sockfd) {

	//Recibo saludo
	int numbytes = 0;
	int32_t longitud = 0;
	if ((numbytes = recv(sockfd, &longitud, sizeof(int32_t), 0)) == -1) {

		char* aux = string_from_format("No se pudo recibir la longitud del saludo");
		logger_mensaje_error(aux);
		free(aux);

		//MUERO
		exit(1);
	}
	char* mensajeSaludoRecibido = malloc(sizeof(char) * longitud);
	if ((numbytes = recv(sockfd, mensajeSaludoRecibido, longitud, 0)) == -1) {

		char* aux = string_from_format("No se pudo recibir saludo");
		logger_mensaje_error(aux);
		free(aux);

		//MUERO
		exit(1);
	}

	char* aux = string_from_format("Saludo recibido: %s", mensajeSaludoRecibido);
	logger_mensaje(aux);
	free(aux);

	//Envio saludo
	char * mensajeSaludoEnviado = malloc(sizeof(char) * 100);
	strcpy(mensajeSaludoEnviado, "Hola, soy el PLANIFICADOREITOR");
	mensajeSaludoEnviado[strlen(mensajeSaludoEnviado)] = '\0';

	int32_t longitud_mensaje = strlen(mensajeSaludoEnviado) + 1;

	void* bufferEnvio = malloc(sizeof(int32_t)+ sizeof(char)*longitud_mensaje);
	memcpy(bufferEnvio, &longitud_mensaje,sizeof(int32_t));
	memcpy(bufferEnvio + sizeof(int32_t),mensajeSaludoEnviado,longitud_mensaje);

	if (send(sockfd, bufferEnvio,sizeof(int32_t)+ sizeof(char)*longitud_mensaje, 0) == -1) {

		char* aux = string_from_format("No se pudo enviar saludo");
		logger_mensaje_error(aux);
		free(aux);

		exit(1);
	}
	char* aux1 = string_from_format("Saludo enviado correctamente");
	logger_mensaje(aux1);
	free(aux1);


	free(bufferEnvio);
	free(mensajeSaludoEnviado);
	free(mensajeSaludoRecibido);

}

int conectar_coodinador() {

	//Creamos un socket
	int sockfd;
	struct sockaddr_in their_addr; // información de la dirección de destino

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		free(IP_CONFIG_COORDINADOR);
		pthread_exit(NULL);
	}

	their_addr.sin_family = AF_INET;    // Ordenación de bytes de la máquina
	their_addr.sin_port = htons(PUERTO_CONFIG_COORDINADOR);  // short, Ordenación de bytes de la red
	their_addr.sin_addr.s_addr = inet_addr(IP_CONFIG_COORDINADOR);  //toma la ip directo

	memset(&(their_addr.sin_zero), 0, 8); // poner a cero el resto de la estructura

	if (connect(sockfd, (struct sockaddr *) &their_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("No se pudo conectar al coordinador");
		free(IP_CONFIG_COORDINADOR);
		pthread_exit(NULL);
	}

	return (sockfd);
}

int conectar_coordinar_status(){
	FD_COORDINADOR_STATUS = -1;
	//Creamos un socket
	int sockfd;
	struct sockaddr_in their_addr; // información de la dirección de destino

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		free(IP_CONFIG_COORDINADOR);
		pthread_exit(NULL);
	}

	their_addr.sin_family = AF_INET;    // Ordenación de bytes de la máquina
	their_addr.sin_port = htons(PUERTO_CONFIG_COORDINADOR_STATUS);  // short, Ordenación de bytes de la red
	their_addr.sin_addr.s_addr = inet_addr(IP_CONFIG_COORDINADOR);  //toma la ip directo

	memset(&(their_addr.sin_zero), 0, 8); // poner a cero el resto de la estructura

	if (connect(sockfd, (struct sockaddr *) &their_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("No se pudo conectar al coordinador status");
		exit(1);
	}

	//conectado al coordinador status
	FD_COORDINADOR_STATUS = sockfd;

	return (sockfd);
}

void recibirInfoCoordinador() {

	//SALUDOS DE CONEXION CON EL COORDINADOR
	int fdCoordinador = conectar_coodinador();
	saludo_inicial_coordinador(fdCoordinador);

	int numbytes = 0;


	//me conecto al servidorpara estatus
	pthread_t hiloCordinadorStatus;
	pthread_create(&hiloCordinadorStatus, NULL, (void*) conectar_coordinar_status, NULL);

	while (1) {
		//recibo: ID_OPERACION,ID_ESI,LENG_CLAVE,CLAVE
		int32_t id_operacion = 0;
		int32_t id_esi = 0;
		int32_t leng_clave = 0;
		if ((numbytes = recv(fdCoordinador, &id_operacion,sizeof(int32_t), 0)) <= 0) {
			if(numbytes < 0 ){
				puts("Error al recibir tarea desde el coordinador");
			}else{
				puts("Se fue el coordinador");
			}
			break;
		} else {

			//recibo el id_esi y len_clave
			if ((numbytes = recv(fdCoordinador, &id_esi, sizeof(int32_t), 0)) <= 0) {
				if (numbytes < 0) {
					puts("Error al recibir el id del esi de la tarea que mando el coordinador");
				} else {
					puts("Se fue el coordinador");
				}

			} else {
				if ((numbytes = recv(fdCoordinador, &leng_clave, sizeof(int32_t), 0)) <= 0) {
					if (numbytes < 0) {
						puts("Error al recibir la longitud de la clave de la tarea que mando el coordinador");
					} else {
						puts("Se fue el coordinador");
					}
				}
			}

			char* clave = malloc(sizeof(char)*leng_clave);

			//recibo ahora si la clave
			if ((numbytes = recv(fdCoordinador,clave,sizeof(char)*leng_clave, 0)) <= 0) {
				if (numbytes < 0) {
					puts("Error al recibir la clave de la tarea que mando el coordinador");
				} else {
					puts("Se fue el coordinador");
				}

			}
			char* aux;
			switch (id_operacion) {
			case GET:

				aux = string_from_format("Coordinador envio GET clave: %s del ESI ID: %d",clave,id_esi);
				logger_mensaje(aux);
				free(aux);

				if(!esta_conectado(id_esi)){
					send_mensaje(fdCoordinador,17);
					break;
				}

				//Controlo si get es sobre un recurso tomado (osea dentro de LIST_ESI_BLOQUEADOR para un unico ESI PAG.10)
				if(find_recurso_by_clave(clave)){

					//muevo de execute a block al ESI
					bool _esElid(t_Esi* un_esi) { return un_esi->id == id_esi;}
					pthread_mutex_lock(&EXECUTE);
					t_Esi* esi_buscado = list_find(LIST_EXECUTE,(void*) _esElid);
					esi_buscado->status=1;
					pthread_mutex_unlock(&EXECUTE);

					char* aux = string_from_format("Marco como bloqueado al ESI ID:%d",id_esi);
					logger_mensaje(aux);
					free(aux);

					//envio mensaje de ejecutado 1:falle , 2:ok , 3: ok pero te bloqueaste
					send_mensaje(fdCoordinador,3);
				}else{
					//registro la clave y continua (cargo en lis_esi_bloqueador)
					bool _esElid(t_Esi* un_esi) { return un_esi->id == id_esi;}
					pthread_mutex_lock(&EXECUTE);
					pthread_mutex_lock(&ESISBLOQUEADOR);
					t_Esi* esi_buscado = list_find(LIST_EXECUTE,(void*) _esElid);
					t_esiBloqueador* esi_bloqueador = get_esi_bloqueador(esi_buscado,clave);
					list_add(LIST_ESI_BLOQUEADOR,esi_bloqueador);
					pthread_mutex_unlock(&ESISBLOQUEADOR);
					pthread_mutex_unlock(&EXECUTE);

					char* aux = string_from_format("Registro que ahora la clave:%s lo tomo el ESI ID:%d",clave,id_esi);
					logger_mensaje(aux);
					free(aux);

					//envio mensaje de ejecutado 1:falle , 2:ok , 3: ok pero te bloqueaste
					send_mensaje(fdCoordinador,2);
				}
				break;

			case SET://con la clave me basta

				aux = string_from_format("Coordinador envio pedido para controlar si la clave: %s esta bloqueada por el esi id: %d",clave,id_esi);
				logger_mensaje(aux);
				free(aux);

				if(!esta_conectado(id_esi)){
					send_mensaje(fdCoordinador,17);
					break;
				}

				if(!clave_tomada_esi_ejecutando(clave)){
					char* aux = string_from_format("ESI ID: %d no tiene tomada la clave %s -> ERROR CLAVE NO BLOQUEADA",id_esi,clave);
					logger_mensaje(aux);
					free(aux);

					//envio mensaje de ejecutado 1:falle , 2:ok , 3: ok pero te bloqueaste, 4:ABORTA_ESI_CLAVE_NO_BLOQUEADA
					send_mensaje(fdCoordinador,4);
				}else{
					send_mensaje(fdCoordinador,2);
				}

				break;
			case STORE:
				aux = string_from_format("Coordinador envio STORE clave: %s del ESI ID: %d",clave,id_esi);
				logger_mensaje(aux);
				free(aux);

				//pregunto si se desconecto, pork al desconectarse ya libero todo antes
				if(!esta_conectado(id_esi)){
					send_mensaje(fdCoordinador,17);
					break;
				}
				//verifico si el esi que pidio el store lo puede hacer -> ABORTA_ESI_CLAVE_NO_BLOQUEADA
				if(!clave_tomada_esi_ejecutando(clave)){
					char* aux = string_from_format("ESI ID: %d no tiene tomada la clave %s -> ERROR CLAVE NO BLOQUEADA",id_esi,clave);
					logger_mensaje_error(aux);
					free(aux);

					//envio mensaje de ejecutado 1:falle , 2:ok , 3: ok pero te bloqueaste, 4:ABORTA_ESI_CLAVE_NO_BLOQUEADA
					send_mensaje(fdCoordinador,4);
				}else{
					//libero el recurso (borro de lis_esi_bloqueador el esi q corresponda)
					libero_recurso_by_clave_id(clave,id_esi);

					//paso de bloqueado a listo (EL PRIMER) ESIs que querian esa clave
					move_esi_from_bloqueado_to_listo(clave);

					//envio mensaje de ejecutado 1:falle , 2:ok , 3: ok pero te bloqueaste, 4:ABORTA_ESI_CLAVE_NO_BLOQUEADA
					send_mensaje(fdCoordinador,2);
				}

				break;

			}
			free(clave);
		}

	}
}

//busca si esta en finalizado al esi
bool esta_conectado(int id_esi){
	bool resultado;
	bool _esElidClave(t_Esi* esi) { return (esi->id == id_esi);}
	pthread_mutex_lock(&FINISHED);
	if(!list_is_empty(LIST_FINISHED) &&
			list_find(LIST_FINISHED, (void*)_esElidClave) != NULL){
		//Ya esta tomado ese recurso
		resultado = false;
	}else{
		resultado = true;
	}
	pthread_mutex_unlock(&FINISHED);
	return resultado;
}

bool clave_tomada_esi_ejecutando(char* clave){
	pthread_mutex_lock(&ESISBLOQUEADOR);
	bool _esElidClave(t_esiBloqueador* esi_bloqueador) { return (strcmp(esi_bloqueador->clave,clave)==0);}
	t_esiBloqueador * esi_tomo_recurso = list_find(LIST_ESI_BLOQUEADOR, (void*)_esElidClave);
	pthread_mutex_unlock(&ESISBLOQUEADOR);
	pthread_mutex_lock(&EXECUTE);
	t_Esi * esi_ejecutando = list_get(LIST_EXECUTE,0);
	if(esi_tomo_recurso!=NULL && esi_ejecutando != NULL ){
		pthread_mutex_unlock(&EXECUTE);
		return esi_tomo_recurso->esi->id == esi_ejecutando->id;
	}
	pthread_mutex_unlock(&EXECUTE);
	return false;
}
void send_mensaje(int fdCoordinador,int tipo_respuesta){
	if (send(fdCoordinador, &tipo_respuesta,sizeof(int32_t), 0) == -1) {

		char* aux = string_from_format("No se pudo enviar el resultado del GET o STORE");
		logger_mensaje_error(aux);
		free(aux);

		exit(1);
	}
	char* aux = string_from_format("Envié respuesta de la ejecucion de la operacion al coordinador");
	logger_mensaje(aux);
	free(aux);

}


//busco si el recurso se encuentra tomado
bool find_recurso_by_clave(char* clave){

	bool resultado = false;
	bool _esElidClave(t_esiBloqueador* esi_bloqueador) { return (strcmp(esi_bloqueador->clave,clave)==0);}
	pthread_mutex_lock(&ESISBLOQUEADOR);
	if(!list_is_empty(LIST_ESI_BLOQUEADOR) &&
			list_find(LIST_ESI_BLOQUEADOR, (void*)_esElidClave) != NULL){
		//Ya esta tomado ese recurso
		resultado = true;
	}
	pthread_mutex_unlock(&ESISBLOQUEADOR);
	return resultado;
}

void libero_recurso_by_clave_id(char* clave,int id_esi){
	bool _esElidClave(t_esiBloqueador* esi_bloqueador) { return (esi_bloqueador->esi->id == id_esi) && (strcmp(esi_bloqueador->clave,clave)==0);}
	pthread_mutex_lock(&ESISBLOQUEADOR);
	if(!list_is_empty(LIST_ESI_BLOQUEADOR) &&
			list_find(LIST_ESI_BLOQUEADOR, (void*)_esElidClave) != NULL){

		//Solo lo saco de la lista
		list_remove_by_condition(LIST_ESI_BLOQUEADOR,(void*)_esElidClave);
		char* aux = string_from_format("Libero la clave:%s que tenia tomado el ESI ID:%d",clave,id_esi);
		logger_mensaje(aux);
		free(aux);
	}else{
		char* aux = string_from_format("No hay clave para liberar del ESI ID:%d",id_esi);
		logger_mensaje(aux);
		free(aux);
	}
	pthread_mutex_unlock(&ESISBLOQUEADOR);
}



#include <stdio.h>
#include <stdlib.h>
#include "funcionalidad_esi.h"
#include "archivo.h"
#include "cliente.h"

void exit_gracefully(int return_nr) {
	exit(return_nr);
}

int main(int argc, char** argv) {

	if(argc < 2){
		puts("Falta el script");
		return EXIT_FAILURE;
	}
	//En caso de una interrupcion va por aca
	signal(SIGINT, intHandler);

	get_parametros_config();

	//Conecta como cliente al coordinador
	int fd_coordinador = conectar_servidor(PUERTO_CONFIG_COORDINADOR,
			IP_CONFIG_COORDINADOR, "COORDINADOR");
	saludo_inicial_servidor(fd_coordinador, "COORDINADOR");

	//Conecta como cliente al planificador
	int fd_planificador = conectar_servidor(PUERTO_CONFIG_PLANIFICADOR,
			IP_CONFIG_PLANIFICADOR, "PLANIFICADOR");
	saludo_inicial_servidor(fd_planificador, "PLANIFICADOR");

	free_parametros_config();

	configure_logger();

	//2.- lee el archivo linea por linea
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;


	int numbytes = 0;
	int32_t numero_linea_leer = 0; //Numero de linea a leer
	int32_t resultado_coordinador = 0; //1:falle , 2:ok , 3: ok pero te bloqueaste

	int32_t longitud_valor = 0;
	int32_t longitud_clave = 0;
	char* aux = string_from_format("Espero numero_linea_leer para empezar...");
	logger_mensaje(aux);
	free(aux);


	//Loop para recibir los numeros de las lineas que tengo que ejecutar
	while (1) {
		//recv de numero_linea_leer(numero de linea a leer) por parte del planificador (sabemos que los numero_linea_leers son solo int32_t)
		if ((numbytes = recv(fd_planificador, &numero_linea_leer, sizeof(int32_t), 0))
				<= 0) {

			close(fd_planificador); // si ya no conversare mas con el cliente, lo cierro
			close(fd_coordinador);

			if (line)
				free(line);
			exit(1); //muero
		} else {

			fp = fopen(argv[1], "r");
			//fp = fopen("script_store.esi", "r");
			if (fp == NULL) {
				perror("Error al abrir el archivo: ");
				close(fd_planificador); // si ya no conversare mas con el cliente, lo cierro
				close(fd_coordinador);
				if (line)
					free(line);
				fclose(fp);
				exit(EXIT_FAILURE);
			}
			if (numero_linea_leer > 0) {
				int count = 0;
				while ((read = getline(&line, &len, fp)) != -1) {
					count++;
					if (count == numero_linea_leer)
						break;
				}
				if (read != -1) { //Encontre la linea que me piden
					char* aux = string_from_format("Recibi numero de linea: %d para leer",numero_linea_leer);
					logger_mensaje(aux);
					free(aux);

					//Envio ESI + DATOS DE LA SENTENCIA QUE LEO al coordinador
					t_esi_operacion parsed = parse(line);
					int32_t operacion = 0; //1:GET 2:SET 3:STORE
					if (parsed.valido) {
						char* aux;
						switch (parsed.keyword) {
						case GET:

							operacion = 1;
							longitud_clave = strlen(parsed.argumentos.GET.clave)
									+ 1;

							//envio al coordinador ID_ESI+OPERACION+CLAVE
							void* bufferEnvio_get = malloc(
									sizeof(int32_t) * 3 + longitud_clave);
							memcpy(bufferEnvio_get, &ID_ESI_OBTENIDO,
									sizeof(int32_t));
							memcpy(bufferEnvio_get + sizeof(int32_t),
									&operacion, sizeof(int32_t));
							memcpy(bufferEnvio_get + sizeof(int32_t) * 2,
									&longitud_clave, sizeof(int32_t));
							memcpy(bufferEnvio_get + sizeof(int32_t) * 3,
									parsed.argumentos.GET.clave,
									longitud_clave);
							if (send(fd_coordinador, bufferEnvio_get,
									(sizeof(int32_t) * 3) + longitud_clave, 0)
									== -1) {
								perror("send");
								exit(1);
							}
							aux = string_from_format("Envie correctamente: GET clave: <%s>",parsed.argumentos.GET.clave);
							logger_mensaje(aux);
							free(aux);

							free(bufferEnvio_get);
							break;
						case SET:

							operacion = 2;
							longitud_valor = strlen(parsed.argumentos.SET.valor)
									+ 1;
							longitud_clave = strlen(parsed.argumentos.SET.clave)
									+ 1;

							//envio al coordinador ID_ESI + OPERACION + CLAVE + VALOR
							void* bufferEnvio_set = malloc(
									sizeof(int32_t) * 4 + longitud_valor
											+ longitud_clave);

							memcpy(bufferEnvio_set, &ID_ESI_OBTENIDO,
									sizeof(int32_t));
							memcpy(bufferEnvio_set + sizeof(int32_t),
									&operacion, sizeof(int32_t));
							memcpy(bufferEnvio_set + sizeof(int32_t) * 2,
									&longitud_clave, sizeof(int32_t));
							memcpy(bufferEnvio_set + sizeof(int32_t) * 3,
									parsed.argumentos.SET.clave,
									longitud_clave);
							memcpy(
									bufferEnvio_set + sizeof(int32_t) * 3
											+ longitud_clave, &longitud_valor,
									sizeof(int32_t));
							memcpy(
									bufferEnvio_set + sizeof(int32_t) * 4
											+ longitud_clave,
									parsed.argumentos.SET.valor,
									longitud_valor);

							if (send(fd_coordinador, bufferEnvio_set,
									sizeof(int32_t) * 4 + longitud_valor
											+ longitud_clave, 0) == -1) {
								perror("send");
								exit(1);
							}
							aux = string_from_format("Envie correctamente: SET clave: <%s> valor: <%s>",
									parsed.argumentos.SET.clave,
									parsed.argumentos.SET.valor);
							logger_mensaje(aux);
							free(aux);

							free(bufferEnvio_set);
							break;
						case STORE:

							operacion = 3;
							longitud_clave = strlen(
									parsed.argumentos.STORE.clave) + 1;

							//envio al coordinador ID_ESI + OPERACION+CLAVE
							void* bufferEnvio_store = malloc(
									sizeof(int32_t) * 3 + longitud_clave);

							memcpy(bufferEnvio_store, &ID_ESI_OBTENIDO,
									sizeof(int32_t));
							memcpy(bufferEnvio_store + sizeof(int32_t),
									&operacion, sizeof(int32_t));
							memcpy(bufferEnvio_store + sizeof(int32_t) * 2,
									&longitud_clave, sizeof(int32_t));
							memcpy(bufferEnvio_store + sizeof(int32_t) * 3,
									parsed.argumentos.STORE.clave,
									longitud_clave);

							if (send(fd_coordinador, bufferEnvio_store,
									sizeof(int32_t) * 3 + longitud_clave, 0)
									== -1) {
								perror("recv");
								exit(1);
							}
							aux = string_from_format("Envie correctamente: STORE clave: <%s>",
									parsed.argumentos.STORE.clave);
							logger_mensaje(aux);
							free(aux);

							free(bufferEnvio_store);

							break;
						default:
							//fprintf(stderr, "No pude interpretar <%s>\n", line);
							aux = string_from_format("No pude interpretar <%s>", line);
							logger_mensaje_error(aux);
							free(aux);

							close(fd_planificador); // si ya no conversare mas con el cliente, lo cierro
							close(fd_coordinador);
							exit(EXIT_FAILURE);
						}

					}else{
						char* aux = string_from_format("Aborto por clave muy larga detectado por el PARSE");
						logger_mensaje_error(aux);
						free(aux);

						close(fd_planificador); // si ya no conversare mas con el cliente, lo cierro
						close(fd_coordinador);
						if (line)
							free(line);
						fclose(fp);
						exit(EXIT_FAILURE);
					}

					//recv resultado de la sentencia q le mande al coordinador!!!!
					if ((numbytes = recv(fd_coordinador, &resultado_coordinador,
							sizeof(int32_t), 0)) <= 0) {
						if (numbytes == 0) {
							// conexión cerrada
							char* aux = string_from_format("Se desconecto el coordinador");
							logger_mensaje_error(aux);
							free(aux);

						} else {
							char* aux = string_from_format("ERROR: al recibir respuesta del coordinador");
							logger_mensaje_error(aux);
							free(aux);
						}
						close(fd_planificador); // si ya no conversare mas con el cliente, lo cierro
						close(fd_coordinador);
						exit(1); //muero
					} else {
						char* aux = string_from_format("Recibi respuesta del coordinador");
						logger_mensaje(aux);
						free(aux);
						t_respuesta_para_planificador respuesta_planificador = {
								.id_tipo_respuesta = 0, .id_esi =
										ID_ESI_OBTENIDO, .mensaje = "", .clave =
										"" };
						switch (resultado_coordinador) {
						case ABORTA: //recibi respuesta q coordinador no lo pudo hacer
							strcpy(respuesta_planificador.mensaje, "ABORTO!");
							respuesta_planificador.mensaje[strlen(
									respuesta_planificador.mensaje)] = '\0';
							respuesta_planificador.id_tipo_respuesta = ABORTA_PLANIFICADOR; //para q el planificador me finalize;

							aux = string_from_format("COORDINADOR ME DIJO QUE ABORTO!");
							logger_mensaje_error(aux);
							free(aux);

							break;
						case OK: //recibi respuesta q coordinador lo hizo bien
							strcpy(respuesta_planificador.mensaje, "OK");
							respuesta_planificador.mensaje[strlen(
									respuesta_planificador.mensaje)] = '\0';
							respuesta_planificador.id_tipo_respuesta = OK_PLANIFICADOR;
							break;
						case OK_BLOQUEADO: //recibi respuesta q coordinador trato pero esta bloqueado
							strcpy(respuesta_planificador.mensaje,
									"ME BLOQUEARON");
							respuesta_planificador.mensaje[strlen(
									respuesta_planificador.mensaje)] = '\0';

							//si me bloquearon es por un GET, entonces mando mi clave
							strcpy(respuesta_planificador.clave,
									parsed.argumentos.GET.clave);
							respuesta_planificador.clave[strlen(
									respuesta_planificador.clave)] = '\0';
							respuesta_planificador.id_tipo_respuesta = OK_PLANIFICADOR;

							aux = string_from_format("COORDINADOR ME DIJO BLOQUEADO!");
							logger_mensaje_error(aux);
							free(aux);

							break;

						}

						//send resultado al planificador
						if (send(fd_planificador, &respuesta_planificador,
								sizeof(t_respuesta_para_planificador), 0)
								== -1) {
							char* aux = string_from_format("No se pudo enviar respuesta al planificador");
							logger_mensaje_error(aux);
							free(aux);
							exit(1);
						}
						aux = string_from_format("Respuesta enviado al planificador correctamente");
						logger_mensaje(aux);
						free(aux);

					}
					destruir_operacion(parsed);
				} else { //No tengo nada mas para leer(es decir, la linea que me piden excede la cantida de lineas que tiene mi script)
					char* aux = string_from_format("Recibi numero de linea: %d para leer",numero_linea_leer);
					logger_mensaje(aux);
					free(aux);
					//Send resultado al planificador q ya no tengo mas lineas para leer
					t_respuesta_para_planificador respuesta_planificador = {
							.id_tipo_respuesta = FINALIZO_TODO_PLANIFICADOR, .id_esi = ID_ESI_OBTENIDO,
							.mensaje = "" };
					strcpy(respuesta_planificador.mensaje,
							"TERMINE DE LEER TODO CAPO");
					respuesta_planificador.mensaje[strlen(
							respuesta_planificador.mensaje)] = '\0';

					if (send(fd_planificador, &respuesta_planificador,
							sizeof(t_respuesta_para_planificador), 0) == -1) {
						char* aux = string_from_format("No se pudo enviar respuesta al planificador");
						logger_mensaje_error(aux);
						free(aux);

						exit(1);
					}
					aux = string_from_format("TERMINE DE LEER TODO!!");
					logger_mensaje(aux);
					free(aux);

					break; //Salgo de loop ya que no recibire ninguna peticion mas del planificador
				}

				fclose(fp);
			}else{
				//planificador me pidio que lear la linea 0
				char* aux = string_from_format("Error al recibir la linea para leer, planificador pidio linea 0");
				logger_mensaje_error(aux);
				free(aux);

				close(fd_planificador);
				close(fd_coordinador);
				fclose(fp);
				if (line)
					free(line);
				exit(1); //muero
			}
		}
	}


	fclose(fp);
	if (line)
		free(line);

	close(fd_planificador);
	close(fd_coordinador);
	return EXIT_SUCCESS;
}

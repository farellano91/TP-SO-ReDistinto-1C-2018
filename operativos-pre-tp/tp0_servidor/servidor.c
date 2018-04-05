/*
 * servidor.c
 *
 *  Created on: 25/3/2018
 *      Author: utnso
 */
#include "servidor.h"

void sigchld_handler(int s) {
	while (wait(NULL) > 0)
		;
}

//Funcion que envia un saludo al cliente y se queda esperando los mensajes del cliente
void atenderCliente(TipoDatosCliente *datoC) {
	
	char * saludo = malloc(sizeof(char)*17);
	strcpy(saludo,"SYSTEM UTNSO 0.1");
	
	
	if (send(datoC->idSocketCliente, saludo, 17, 0) == -1) {
		printf("No se pudo enviar saludo al cliente");
	}
	free(saludo);
	
	Alumno alumno;
	int result_recv = recv(datoC->idSocketCliente, &alumno,sizeof(Alumno),MSG_WAITALL);
	printf("%s\n",alumno.nombre );
	printf("%s\n",alumno.apellido);

	//Mando la estructura de header
	ContentHeader * ch = malloc(sizeof(int) * 2);
	ch->id = 18;

	void* bufferContenido = malloc(30);
	memcpy(bufferContenido,"12345678901234567890123456789",30);

	ch->len = 30;
	if (send(datoC->idSocketCliente, ch, sizeof(int) * 2, 0) == -1) {
		printf("No se pudo enviar saludo al cliente");
	}

	//Mando el buffer con los datos siguientes
	
	if (send(datoC->idSocketCliente, bufferContenido, ch->len, 0) == -1) {
		printf("No se pudo enviar saludo al cliente");
	}

	free(ch);

	//Recibo el buffer con header + digest
	void * buffer = malloc(sizeof(ContentHeader) + MD5_DIGEST_LENGTH);
	if (recv(datoC->idSocketCliente,buffer,sizeof(ContentHeader) + MD5_DIGEST_LENGTH,0) == -1) {
		printf("No se pudo recivir el header + md5 del cliente");
	}
	ContentHeader ch2 = {.id=0 , .len=0};
	memcpy(&ch2,buffer,sizeof(ContentHeader));
	

	void * digestRecivido = malloc(ch2.len);

	memcpy(digestRecivido,buffer + sizeof(ContentHeader),ch2.len);

	//Calculo mi digest
	void * digest = malloc(MD5_DIGEST_LENGTH);
  	MD5_CTX context;
  	MD5_Init(&context);
  	MD5_Update(&context, bufferContenido, strlen(bufferContenido) + 1);
  	MD5_Final(digest, &context);

	//comparo con el digestRecivido
	int respuesta = 1;
	if ( (*(int*)(digest)) == (*(int*)(digestRecivido)) ) 
    {
        respuesta = 0;
    }
	//Envio la respuesta
	if(send(datoC->idSocketCliente, &respuesta , sizeof(int), 0) == 1){
    	printf("No se pudo enviar la respuesta al cliente");
  }

	free(buffer);
	free(bufferContenido);
	free(digestRecivido);
	free(digest);
}

void levantarServidor() {
	int sockfd; // Escuchar sobre: sock_fd, nuevas conexiones sobre: idSocketCliente
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del idSocketCliente
	int sin_size;
	struct sigaction sa;
	int yes = 1;

	//1° CREAMOS EL SOCKET
	//sockfd: numero o descriptor que identifica al socket que creo
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		printf("Error al abrir el socket de escucha\n");
		exit(1);
	}
	printf("Se creo el socket %d\n", sockfd);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = PF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(MYPORT);    // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = inet_addr(MYIP); //INADDR_ANY (aleatoria) o 127.0.0.1 (local)
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	//2° Relacionamos los datos de my_addr <=> socket
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		printf("Fallo el bind\n");
		perror("bind");
		exit(1);
	}

	//3° Listen: se usa para dejar al socket escuchando las conexiones que se acumulan en una cola hasta que
	//la aceptamos
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		printf("Fallo el listen\n");
		exit(1);
	}
	printf("Socket escuchando!!!\n");

	//-------
	sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	//--------
	sin_size = sizeof(struct sockaddr_in);

	//4° Accept idSocketCliente: numero de socket del cliente que se acaba de conectar
	while (1) {
		
		TipoDatosCliente * datosCliente = malloc(sizeof(TipoDatosCliente));
		datosCliente->ipCliente = malloc(sizeof(char) * 30);

		puts("Ingrese 'si' para recibir una nueva conexion o 'no' para terminar ");
		//scanf solo inserta 2 y el 3ro lo deja para el \0
		char * entrada = malloc(sizeof(char) * 2 + 1);
		//espero de entrada solo SI  o NO 
		scanf("%s",entrada);
		if(strcmp(entrada,"no") == 0){

			free(entrada);
			free(datosCliente->ipCliente);
			free(datosCliente);
			break;
		}
		free(entrada);

		if ((datosCliente->idSocketCliente = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size))
				== -1) {
			perror("accept");
		}
		
		strcpy(datosCliente->ipCliente,inet_ntoa(their_addr.sin_addr));
		
		printf("Socket cliente %d de IP %s\n", datosCliente->idSocketCliente,
				datosCliente->ipCliente);

		
		atenderCliente(datosCliente);
		free(datosCliente->ipCliente);
		free(datosCliente);
	}
	close(sockfd);
}



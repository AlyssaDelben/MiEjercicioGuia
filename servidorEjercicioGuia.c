#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

int contador;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *AtenderCliente (void *socket){
	
	int sock_conn;
	int *s;
	s = (int *) socket;
	sock_conn = *s;
	
	int ret;
	char peticion[512];
	char respuesta[512];
	// INICIALITZACIONS
	
	//wocle de atencion al cliente
	int terminar=0;
	while(terminar==0)
	{
		
		// Ahora recibimos su peticion
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibida una petición\n");
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		//Escribimos la peticion en la consola
		
		printf ("La petición es: %s\n",peticion);
		char *p = strtok(peticion, "/");
		int codigo =  atoi (p);
		char nombre[20];
		if ((codigo != 0) && (codigo!= 5)){
			p = strtok( NULL, "/");
			strcpy (nombre, p);
			printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		
		if (codigo==0)
			terminar=1;
		if (codigo ==1){
			//piden la longitd del nombre
			sprintf (respuesta,"%d",strlen (nombre));
		}
		else if (codigo ==2)
		{
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (respuesta,"SI");
			else
				strcpy (respuesta,"NO");
			
		}
		else if (codigo ==3)
		{
			//quiere saber si el nombre es palindromo
			int palindromo=1;
			
			for(int i=0; i<(strlen(nombre)/2); i++){
				if(tolower(nombre[i]) != tolower(nombre[strlen(nombre)-i-1]))
					palindromo=0;
			}
			
			if (palindromo==1)
				strcpy (respuesta,"SI");
			else
				strcpy (respuesta,"NO");
			
		}
		else if (codigo==4)
		{
			for(int i=0; i<strlen(nombre); i++){
				nombre[i]=toupper(nombre[i]);
			}
			
			strcpy(respuesta, nombre);
		}
		else{
			sprintf(respuesta, "%d", contador);
		}
		
		if( codigo!=0){
			printf("Respuesta: %s\n", respuesta);
			// Enviamos la respuesta
			write (sock_conn,respuesta, strlen(respuesta));
		}
		
		if((codigo==1) || (codigo==2) || (codigo==3) || (codigo==4)){
			pthread_mutex_lock (&mutex);
			contador=contador+1;
			pthread_mutex_unlock (&mutex);
		}
	}	
}

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	
	// Atenderemos peticiones
	contador=0;
	int i=0;
	int sockets[100];
	pthread_t thread;
	
	for(;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexon\n");
		
		sockets[i] = sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		
		// Se acabo el servicio para este cliente
		pthread_create(&thread, NULL, AtenderCliente, &sockets[i]);
		
		i++;
	}
}

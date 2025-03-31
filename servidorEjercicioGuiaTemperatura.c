#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

int sock_conn, ret;
char peticion[512];
char respuesta[512];
// INICIALITZACIONS

void *AtenderCliente (void *socket){
	
	int sock_conn;
	int *s;
	s = (int *) socket;
	sock_conn = *s;
	
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
		float temperatura;
		if (codigo !=0){
			p = strtok( NULL, "/");
			temperatura = atof(p);
			printf ("Codigo: %d, temperatura: %f\n", codigo, temperatura);
		}
		
		if (codigo ==0)
			terminar = 1;
		
		if (codigo ==1){
			//piden la temperatura en Fareneiht
			temperatura = (temperatura * 1.8) + 32;
			sprintf (respuesta,"%f", temperatura);
		}
		else
		{
			//piden la temperatura en Celsius
			temperatura = (temperatura - 32) * 10/18;
			sprintf (respuesta,"%f", temperatura);
		}
		
		if( codigo !=0){
			printf("Respuesta: %s\n", respuesta);
			// Enviamos la respuesta
			write (sock_conn,respuesta, strlen(respuesta));
		}
	}	
}

int main(int argc, char *argv[])
{
	int sock_listen;
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
	serv_adr.sin_port = htons(9051);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	
	// Atenderemos peticiones
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

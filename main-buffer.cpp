#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>	
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MULTICAST_ADDR "255.0.0.37"

void *thread_function_1(void *arg);
void *thread_function_2(void *arg);
int* led_number(int k);
pthread_mutex_t t_mutex; 

#define WORK_SIZE 1024
char work_area[WORK_SIZE];

int buffer = 0;
double tempo[2]= {1,1};

int main(){
    int res;
    pthread_t a_thread, b_thread;
    void *thread_result;
    int i=0;
    
    int server_sockfd, client_sockfd;
    size_t server_len;
    socklen_t client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    
    struct ip_mreq mreq;  // para endere�o multicast
    
    unsigned short porta = 9711;

    res = pthread_mutex_init(&t_mutex, NULL);    
    if (res != 0) {
        perror("Iniciacao do Mutex falhou");
        exit(EXIT_FAILURE);
    }

    unlink("server_socket");  // remocao de socket antigo
    if ( (server_sockfd = socket(AF_INET, SOCK_DGRAM, 0) )  < 0  )  // cria um novo socket
    {
        printf(" Houve erro na ebertura do socket ");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(porta);
    
    
    server_len = sizeof(server_address);
    
    if(  bind(server_sockfd, (struct sockaddr *) &server_address, server_len) < 0 )
    {
        perror("Houve error no Bind");
        exit(1);
    }
    
    
    // use setsockopt() para requerer inscri��o num grupo multicast
    mreq.imr_multiaddr.s_addr=inet_addr(MULTICAST_ADDR);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(server_sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    res = pthread_create(&a_thread, NULL, thread_function_1, NULL);
    if (res != 0) {
        perror("Criacao da Thread 1 falhou");
        exit(EXIT_FAILURE);
    }
    
    res = pthread_create(&b_thread, NULL, thread_function_2, NULL);
    if (res != 0) {
        perror("Criacao da Thread 2 falhou");
        exit(EXIT_FAILURE);
    }

    
    while(1){

        recvfrom(server_sockfd, &tempo, sizeof(tempo),0,(struct sockaddr *) &client_address, &client_len);
			        
		sleep(1);
		                
    }
    printf("\nMAIN() --> Esperando as threads terminar...\n");
    
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Juncao da Thread falhou");
        exit(EXIT_FAILURE);
    }
    
    printf("MAIN() --> Thread foi juntada com sucesso\n");
    pthread_mutex_destroy(&t_mutex);
    exit(EXIT_SUCCESS);
}

void *thread_function_1(void *arg) {
	
	int *values;
	values = malloc(8*sizeof(int));

	int sockfd;
    int len;
    struct sockaddr_in address;
    
    unsigned short porta = 9811;
    
    sockfd  = socket(AF_INET, SOCK_DGRAM,0);  // criacao do socket
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    address.sin_port = htons(porta);
    
    len = sizeof(address);

	while(1){
		pthread_mutex_lock(&t_mutex);
		if(buffer < 7)
			buffer = buffer + 1;
		printf("buffer: %d\n", buffer);
		if (buffer == 7)
			values = led_number(7);
		else if (buffer == 6)
			values = led_number(6);
		else if (buffer == 5)
			values = led_number(5);			
		else if (buffer == 4)
			values = led_number(4);
		else if (buffer == 3)
			values = led_number(3);
		else if (buffer == 2)
			values = led_number(2);
		else if (buffer == 1)
			values = led_number(1);
	
		sendto(sockfd, &values,sizeof(values),0,(struct sockaddr *) &address, len);
		pthread_mutex_unlock(&t_mutex);		
		sleep(tempo[0]*5);		
	}

	free(values);
	close(sockfd);
}

void *thread_function_2(void *arg) {	
	int *values;
	values = malloc(8*sizeof(int));

	int sockfd;
    int len;
    struct sockaddr_in address;
    
    unsigned short porta = 9811;
    
    sockfd  = socket(AF_INET, SOCK_DGRAM,0);  // criacao do socket
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    address.sin_port = htons(porta);
    
    len = sizeof(address);

	while(1){
		pthread_mutex_lock(&t_mutex);
		if(buffer > 0)
			buffer = buffer - 1;
		printf("buffer: %d\n", buffer);
		
		if (buffer == 6)
			values = led_number(6);
		else if (buffer == 5)
			values = led_number(5);			
		else if (buffer == 4)
			values = led_number(4);
		else if (buffer == 3)
			values = led_number(3);
		else if (buffer == 2)
			values = led_number(2);
		else if (buffer == 1)
			values = led_number(1);
		else if (buffer == 0)
			values = led_number(0);

		sendto(sockfd, &values,sizeof(values),0,(struct sockaddr *) &address, len);
		pthread_mutex_unlock(&t_mutex);
		sleep(tempo[1]*5);		
	}

	free(values);

	close(sockfd);
}

int* led_number(int k) {

	int *values;
	values = malloc(8*sizeof(int));

	if (k == 7)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 6)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 5)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 4)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 3)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 2)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 1)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}
	else if (k == 0)
	{
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		values[5] = 0;
		values[6] = 0;
		values[7] = 1;
	}

	return values;
}

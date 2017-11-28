///////////////////////////////////////////////
// DCA 0125 - SISTEMAS DE TEMPO REAL
// ALUNOS:	Alexandre José Justino de França
//			Natan de Lima Silva
//			Rian Lopes dos Santos Martins
// PROJETO FINAL: Serviço Multicast
///////////////////////////////////////////////

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MULTICAST_ADDR "225.0.0.37"

void *thread_sendSensorFunction(void* idThread) {
    printf("sensor_thread--> Teste\n");

    int sockfd;
    int len;
    socklen_t len_recv;
    struct sockaddr_in address;
    int result;

    unsigned short porta = 9734;
    sockfd  = socket(AF_INET, SOCK_DGRAM,0);  // criacao do socket

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    address.sin_port = htons(porta);

    len = sizeof(address);
    double adcRead[2] = {0, 0};

    while(1){
    	printf("Teste envio adc\n");
    	adcRead[0] = 255; // ler adc 1
    	adcRead[1] = 245; // ler adc 2
    	sendto(sockfd, &adcRead,sizeof(adcRead),0,(struct sockaddr *) &address, len);
        sleep(1);
    }

    close(sockfd);
    pthread_exit((void *) "Obrigado pelo seu tempo de CPU");
}

void *thread_rcvValueFunction(void* selectedPort) {
    printf("values_thread--> Teste\n");
    int server_sockfd, client_sockfd;
    size_t server_len;
    socklen_t client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    
    struct ip_mreq mreq;  // para endere�o multicast
    
    int porta = 9734;
    
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
    
    
    while(1){
        double valor[7] = {0, 0, 0, 0, 0, 0, 0};
        
        printf("Servidor esperando ...\n");
        
        client_len = sizeof(client_address);
        if(recvfrom(server_sockfd, &valor, sizeof(valor),0,
                    (struct sockaddr *) &client_address, &client_len) < 0 )
        {
            perror(" erro no RECVFROM( )");
            exit(1);
        }
        printf(" Valor 1 recebido foi = %f\n", valor[0]);
        printf(" Valor 2 recebido foi = %f\n", valor[1]);
        printf(" Valor 3 recebido foi = %f\n", valor[2]);
        printf(" Valor 4 recebido foi = %f\n", valor[3]);
        printf(" Valor 5 recebido foi = %f\n", valor[4]);
        printf(" Valor 6 recebido foi = %f\n", valor[5]);
        printf(" Valor 7 recebido foi = %f\n", valor[6]);

        int btnPressed = 1; // leitura do botao

        if(btnPressed){
        	// pega leitura da porta
        	
        	// atualiza valor da porta
        	if(porta == 9733){
        		printf("\tvolta valor original\n");
        		porta = 9734;
        	}
        	else{
        		printf("\tseta outro valor\n");
        		porta = 9733;
        	}

        	// refaz conexao 
		    unlink("server_socket");  // remocao de socket antigo
		    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		    server_address.sin_family = AF_INET;
		    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		    server_address.sin_port = htons(porta);
		    server_len = sizeof(server_address);
		    bind(server_sockfd, (struct sockaddr *) &server_address, server_len);		    
		    // use setsockopt() para requerer inscri��o num grupo multicast
		    mreq.imr_multiaddr.s_addr=inet_addr(MULTICAST_ADDR);
		    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
		    setsockopt(server_sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
        }
        
    }
    pthread_exit((void *) "Obrigado pelo seu tempo de CPU");
}

int main(){

	int sensorAns, valuesAns;
	pthread_t sensor_thread, values_thread;
	void *thread_result1, *thread_result2;

	sensorAns = pthread_create(&sensor_thread, NULL, thread_sendSensorFunction, (void *)0);
    if (sensorAns != 0) {
        perror("A Craição da sensor_thread falhou");
        exit(EXIT_FAILURE);
    }

    valuesAns = pthread_create(&values_thread, NULL, thread_rcvValueFunction, (void *)9734);
    if (valuesAns != 0) {
        perror("A Craição da sensor_thread falhou");
        exit(EXIT_FAILURE);
    }

    sensorAns = pthread_join(sensor_thread, &thread_result1);
    if (sensorAns != 0) {
        perror("O thread_join falhou");
        exit(EXIT_FAILURE);
    }

    printf("MAIN()--> O thread_join1 retornou:   %s\n", (char *)thread_result1);

    valuesAns = pthread_join(values_thread, &thread_result2);
    if (valuesAns != 0) {
        perror("O thread_join falhou");
        exit(EXIT_FAILURE);
    }

    printf("MAIN()--> O thread_join1 retornou:   %s\n", (char *)thread_result1);


	exit(EXIT_SUCCESS);
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>	
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MULTICAST_ADDR "255.0.0.37"

using namespace BlackLib;

void *thread_function_1(void *arg);
void *thread_function_2(void *arg);
void led_number(int k);
pthread_mutex_t t_mutex; 

#define WORK_SIZE 1024
char work_area[WORK_SIZE];

int buffer = 0;
double tempo[2]= {1,1};

BlackGPIO botao(GPIO_66,input);
BlackGPIO pin1(GPIO_5,output);
BlackGPIO pin2(GPIO_48,output);
BlackGPIO pin3(GPIO_31,output);
BlackGPIO pin4(GPIO_60,output);
BlackGPIO pin5(GPIO_30,output);
BlackGPIO pin6(GPIO_4,output);
BlackGPIO pin7(GPIO_15,output);
ADC adc_t1(AIN0);
ADC adc_t2(AIN1);

int main(){
    int res;
    pthread_t a_thread, b_thread;
    void *thread_result;
    int i=0;
    socklen_t len_recv;
    struct sockaddr_in address;
    int sockfd;
    int len;
    int result;

    res = pthread_mutex_init(&t_mutex, NULL);    
    if (res != 0) {
        perror("Iniciacao do Mutex falhou");
        exit(EXIT_FAILURE);
    }
    
    while(1){
    i=0;
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

    unsigned short porta = 9711;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    address.sin_port = htons(porta);    

    while(i==0){
	                
        tempo[1]= adc_t1.getPercentValue()/100.0;
        tempo[2]= adc_t2.getPercentValue()/100.0;
	if(botao.getValue()!="1"){
		pthread_cancel(a_thread);
		pthread_cancel(b_thread);		
		led_number(0);
		pthread_mutex_unlock(&t_mutex);
		i = 1;
		buffer = 0 ;
		sleep(1);
		
	}
		        
	sleep(0.5);
   	}
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
	while(1){
		pthread_mutex_lock(&t_mutex);
		if(buffer < 7)
			buffer = buffer + 1;
		printf("buffer: %d\n", buffer);
		if (buffer == 7)
			led_number(7);
		else if (buffer == 6)
			led_number(6);
		else if (buffer == 5)
			led_number(5);			
		else if (buffer == 4)
			led_number(4);
		else if (buffer == 3)
			led_number(3);
		else if (buffer == 2)
			led_number(2);
		else if (buffer == 1)
			led_number(1);
	
		pthread_mutex_unlock(&t_mutex);		
		sleep(tempo[1]*5);		
	}
}

void *thread_function_2(void *arg) {	
	while(1){
		pthread_mutex_lock(&t_mutex);
		if(buffer > 0)
			buffer = buffer - 1;
		printf("buffer: %d\n", buffer);
		
		if (buffer == 6)
			led_number(6);
		else if (buffer == 5)
			led_number(5);			
		else if (buffer == 4)
			led_number(4);
		else if (buffer == 3)
			led_number(3);
		else if (buffer == 2)
			led_number(2);
		else if (buffer == 1)
			led_number(1);
		else if (buffer == 0)
			led_number(0);

		pthread_mutex_unlock(&t_mutex);
		sleep(tempo[2]*5);		
	}
}

void led_number(int k) {
	if (k == 7)
	{
		pin1.setValue(low);
		pin2.setValue(low);
		pin3.setValue(low);
		pin4.setValue(high);
		pin5.setValue(low);
		pin6.setValue(high);
		pin7.setValue(high);
	}
	else if (k == 6)
	{
		pin1.setValue(high);
		pin2.setValue(high);
		pin3.setValue(high);
		pin4.setValue(high);
		pin5.setValue(high);
		pin6.setValue(high);
		pin7.setValue(low);
	}
	else if (k == 5)
	{
		pin1.setValue(high);
		pin2.setValue(low);
		pin3.setValue(high);
		pin4.setValue(high);
		pin5.setValue(high);
		pin6.setValue(high);
		pin7.setValue(low);
	}
	else if (k == 4)
	{
		pin1.setValue(low);
		pin2.setValue(low);
		pin3.setValue(high);
		pin4.setValue(high);
		pin5.setValue(high);
		pin6.setValue(low);
		pin7.setValue(high);
	}
	else if (k == 3)
	{
	 	pin1.setValue(high);
		pin2.setValue(low);
		pin3.setValue(high);
		pin4.setValue(high);
		pin5.setValue(low);
		pin6.setValue(high);
		pin7.setValue(high);
	}
	else if (k == 2)
	{
		pin1.setValue(high);
		pin2.setValue(high);
		pin3.setValue(high);
		pin4.setValue(low);
		pin5.setValue(low);
		pin6.setValue(high);
		pin7.setValue(high);

	}
	else if (k == 1)
	{
	 	pin1.setValue(low);
		pin2.setValue(low);
		pin3.setValue(low);
		pin4.setValue(high);
		pin5.setValue(low);
		pin6.setValue(low);
		pin7.setValue(high);

	}
	else if (k == 0)
	{
	 	pin1.setValue(high);
		pin2.setValue(high);
		pin3.setValue(low);
		pin4.setValue(high);
		pin5.setValue(high);
		pin6.setValue(high);
		pin7.setValue(high);
	}
}

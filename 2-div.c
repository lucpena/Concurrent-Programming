#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"


int i = 0;
int y = 10;


void * t1(void * arg){
    i = 5;
    pthread_exit(0);    
}


void * t2(void * arg){
     int x = y/i;
     printf("Valor de x: %d\n",x);
     pthread_exit(0);    
}

int main() {

	while(1) {
		pthread_t thread1,thread2;

		pthread_create(&thread1, NULL, t1, NULL);
		pthread_create(&thread2, NULL, t2, NULL);

		pthread_join(thread1,NULL);
		pthread_join(thread2,NULL);

		printf("\n");
		sleep(1);
	    y += 10;
		i = 0;
	}
	return 0;
}





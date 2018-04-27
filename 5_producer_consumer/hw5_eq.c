#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>


#define BUFFER_SIZE 16

int buffer[BUFFER_SIZE];
int itemsPerProducer;
int itemsPerConsumer;
pthread_mutex_t lock;
pthread_cond_t ready_to_read;
pthread_cond_t ready_to_write;
int counter = 0;

struct producerData{
    int pid;
    int numProduced;
};

int isBufferFull() {
    int i;
    int sum;
    for(i = 0; i < BUFFER_SIZE; i++) {
        if(buffer[i] > 0) {
            sum++;
        }
    }

    if(sum == 16){
        sum = 1;
    } else {
        sum = 0;
    }

    return sum;
}

int isBufferEmpty() {
    int i;
    int sum = 0;
    for(i = 0; i < BUFFER_SIZE; i++) {
        if(buffer[i] > 0) {
            sum++;
        }
    }

    if(sum == 0){
        sum = 1;
    } else {
        sum = 0;
    }

    return sum;

}


void *produce(void *param) {
    struct producerData *pData = (struct producerData *) param;
    int item;
    int i = 0;
    
    while(i < pData->numProduced) {
        item = pData->pid * pData->numProduced + i;
        
        pthread_mutex_lock(&lock);
        while(isBufferFull()) {
            pthread_cond_wait(&ready_to_write, &lock);
        }

        
        if(counter < BUFFER_SIZE) {
            if(item == 0) {
                item = 1;
            }

            buffer[counter] = item;
            printf("[SLOT %i] Added %i \n", counter+1, item);
            counter++;
        } else {
            exit(1);
        }

        pthread_cond_signal(&ready_to_read);
        pthread_mutex_unlock(&lock);
   
        i++;
    }
    
    pthread_exit(0); 
}



void *consume(void *param) {
    int numConsumed = *(int *) param;
    int item; 
    int i = 0;

    while(i < numConsumed) {
        pthread_mutex_lock(&lock);

        while(isBufferEmpty()) {
            pthread_cond_wait(&ready_to_read, &lock);
        }
        
        
        if(counter > 0) {
            counter--;
            item = buffer[counter];
            printf("[SLOT %i] Consumed %i \n", counter+1, item);
            buffer[counter] = 0;
        } else {
            exit(1);
        }
        pthread_cond_signal(&ready_to_write);
        pthread_mutex_unlock(&lock);

        i++;   
    }
    
    pthread_exit(0);
}


int main(int argc, const char * argv[]) {
    int numProducers, numConsumers, numItemsProduced;
    pthread_t *producers;
    pthread_t *consumers;
    int i;
    
    if (argc != 4) {
        printf( "Please input 3 arguments (Producers, Consumers, Items)\n");
        return 1;
    }
    
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n ERROR: Failed to initialize mutex.\n");
        return 1;
    }
    
    numProducers = pow(2, atoi(argv[1]));
    numConsumers = pow(2, atoi(argv[2]));
    numItemsProduced = pow(2, atoi(argv[3]));
    
    itemsPerProducer = numItemsProduced / numProducers;
    itemsPerConsumer = numItemsProduced / numConsumers;
    
    printf("PRODUCERS: %i \n", numProducers);
    printf("CONSUMERS: %i \n", numConsumers);
    printf("ITEMS: %i \n\n", numItemsProduced);
    
    producers = (pthread_t *)malloc(numProducers * sizeof(pthread_t));
    consumers = (pthread_t *)malloc(numConsumers * sizeof(pthread_t));
    struct producerData pData[numProducers];

    pthread_cond_init(&ready_to_read, 0);
    pthread_cond_init(&ready_to_write, 0);
    
    for(i = 0; i < numProducers; i++){
        pData[i].pid = i;
        pData[i].numProduced = itemsPerProducer;
        
        if(pthread_create(&producers[i], NULL, produce, &pData[i]) != 0){
            printf("\n ERROR: Failed to create producer thread.\n");
            return 1;
        }
    }
    
    for(i = 0; i < numConsumers; i++){
        if(pthread_create(&consumers[i], NULL, consume, &itemsPerConsumer) != 0){
            printf("\n ERROR: Failed to create consumer thread.\n");
            return 1;
        }
    }
    
    for(i = 0; i < numProducers; i++){
        pthread_join(producers[i], NULL);
    } 
    
    for(i = 0; i < numConsumers; i++){
        pthread_join(consumers[i], NULL);
    }
    
    free(producers);
    free(consumers);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&ready_to_read);
    pthread_cond_destroy(&ready_to_write);
    
    
    printf("SUCCESS. \n");
    return 0;
    
}
        
 


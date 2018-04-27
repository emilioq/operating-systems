#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 67108864 //64MB
#define THREADS 4

static char buffer[BUFFER_SIZE];
int totalCount[128];
int countPerThread[128][THREADS] = {0};
float times[5];
float timeElapsed;
float totalTimes;

void * countChars(void * args);
                        
struct Data {
    int lowerBound;
    int upperBound;
    int id;
};

void * countChars(void * args) {
    struct Data *params = (struct Data *) args;
    int low = params->lowerBound;
    int up = params->upperBound;
    int id = params->id;
    
    int deciValue;
    for (int i = low; i < up; i++) {
        deciValue = (int)buffer[i];
        countPerThread[deciValue][id]++;
    }
    pthread_exit(0);
    return 0;
}

int main (int argc, char *argv[]) {
    for(int i = 0; i < 5; i++) {
        
        int trial = i+1;
    
        clock_t clock1 = clock();
        printf("[TRIAL %d] \n", trial);
        printf("Using %d threads: \n", THREADS);
        int file = open(argv[1], O_RDONLY);
        int totalRead, partitionSize, temp = 0;
        
        for(int i = 0; i < 128; i++) {
            totalCount[i] = 0;
        }
        
        if(file < 0) {
            printf("File not found\n");
            return 0;
        }
        
        //reading and storing in buffer
        totalRead = read(file, buffer, BUFFER_SIZE);
        close(file);
        
        partitionSize = totalRead / THREADS;
        if(partitionSize < 1) {
            partitionSize = 1;
        }
        
        pthread_t threads[THREADS] = {0};
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        
        for (int i = 0; i < THREADS; i++) {
            struct Data *params = malloc (sizeof (struct Data));
            params-> id = i;
            params-> lowerBound = temp;
            
            temp += partitionSize;
            if (temp > totalRead - 1) {
                temp = totalRead -1;
            }
            
            if (i == THREADS - 1) {
                temp = totalRead - 1;   
            }
            params->upperBound = temp;
            
            pthread_create(&threads[i], &attr, countChars, params);
            pthread_join(threads[i], NULL);
            
            if (temp == totalRead - 1) {
                break;
            }
            
        }
        
        
        for(int i = 0; i < 128; i++) {
            for(int j = 0; j < THREADS; j++) {
                totalCount[i] += countPerThread[i][j];
            }
            
            if(i < 33 || i == 127) {
                printf("%d occurences of 0x%x \n", (totalCount[i] / trial), i);
            } else {
                printf("%d occurences of '%c' \n", (totalCount[i] / trial), (char)i);
            }
        }
        
        //printf("\n Uses %d threads\n\n", THREADS);
        
        clock_t clock2 = clock();
        timeElapsed = (double)(clock2 - clock1) / CLOCKS_PER_SEC; 
        printf("Time elapsed: %f seconds\n\n", (timeElapsed));
        times[i] = timeElapsed;
        totalTimes += timeElapsed;
    }
    
    for (int i = 0; i < (sizeof(times) / sizeof(times[0])); i++) {
        printf("Trial: %i took %f seconds\n", i+1, times[i]);
    }
    
    printf("Average: %f seconds\n", totalTimes/5);
        
    
    return 0;
                    
}
        
        
        
        
 


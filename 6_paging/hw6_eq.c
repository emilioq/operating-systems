#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define REFS 100

int pageSize;
int memorySize;
int pageAlgorithm;
int frameNum;
int pageFaults = 0;


int isInArray(int numToFind, int array[], int size){
    int index = 0;
    do {
        if (array[index] == numToFind)
            return index;
        index++;
        
    } while (index < size);
    return -1;
}

int FIFO(int pageTable[], int size){
    int replace = 0;
    int *frames = (int *)malloc(frameNum * sizeof(int));
    
    for (int i = 0; i < frameNum; i++){
        frames[i] = -1;
    }
    for (int i = 0; i < size; i++){ 
        if (isInArray(pageTable[i], frames, frameNum) < 0){
            frames[replace % frameNum] = pageTable[i];
            replace++;
            pageFaults++;
        }
    }
    return pageFaults;
}


int LRU(int pageTable[], int size) {
    int index = 0;
    int innerIndex = 0;
    
    int *frames = (int *)malloc(frameNum * sizeof(int));
    
    for (int i = 0; i < frameNum; i++){
        frames[i] = -1;
    }
    
    int *checkTableA = (int *)malloc(frameNum * sizeof(int));
    int *checkTableB = (int *)malloc(frameNum * sizeof(int));
    
    
    frames[innerIndex] = pageTable[innerIndex];
    
    pageFaults++;
    innerIndex++;
    
    for (int i = 1; i < size; i++) {
        if (isInArray(pageTable[i], frames, frameNum) < 0){
            index = frameNum;
            pageFaults++;
            
            if (innerIndex < frameNum){
                frames[innerIndex] = pageTable[i];
                innerIndex++;
            } else {
                
                for (int j = 0; j < frameNum; j++) {
                    checkTableA[j] = 0;
                    for (int k = i - 1; k < pageSize; k--){
                        if (frames[j] != pageTable[k])
                            checkTableA[j]++;
                        else
                            break;
                    }
                }
                
                for (int j = 0; j < frameNum; j++) {
                    checkTableB[j] = checkTableA[j];
                }
                
                for (int j = 0; j < frameNum; j++){
                    for (int k = j; k < frameNum; k++){
                        if (checkTableB[j] < checkTableB[k]){
                            int temp = checkTableB [j];
                            checkTableB[j] = checkTableB[k];
                            checkTableB[k] = temp;
                        }
                    }
                }
                
                for (int j = 0; j < frameNum; j++){
                    if (checkTableA[j] == checkTableB[0])
                        frames[j] = pageTable[i];
                }
            }
        }
    }
    return pageFaults;
}




int main(int argc, char *argv[]) {
    if (argc < 5){
        printf("\n ERROR: You must input 4 arguments \n");
        return 1;
    }
    
    pageSize = pow(2, atoi(argv[1]));
    memorySize = pow(2, atoi(argv[2]));
    
    if (strcmp(argv[3], "FIFO") == 0){
        pageAlgorithm = 0;
    } else if (strcmp(argv[3], "LRU") == 0) {
        pageAlgorithm = 1;
    } else {
        printf("\n ERROR: Could not specify page-replacement algorithm \n");
        return 1;
    }
    
    frameNum = atoi(argv[4]);
    
    
    if (pageAlgorithm == 0){
        printf("Page Size: %d bytes\n Memory Size: %d bytes\n Algorithm: FIFO\n Frames Allocated: %d\n", pageSize, memorySize, frameNum);
    }
    else{
        printf("Page Size: %d bytes\n Memory Size: %d bytes\n Algorithm: LRU\n Frames Allocated: %d\n", pageSize, memorySize, frameNum);
    }
    
    
    int pageTable[REFS];
    
    int count = 0;
    int num;
    char exit[8] = " ";
    
    printf("Enter address: ");
    while (fgets(exit, sizeof(exit), stdin)){
        exit[strlen(exit) - 1] = '\0';
        if (strcmp(exit, "") == 0) 
            break;
        for (int i = 0; i < 8; i++) {
            if (exit[i] == 'A' || exit[i] == 'a')
                exit[i] = '3';
            else if(exit[i] == 'B' || exit[i] == 'b')
                exit[i] = '5';
            else if(exit[i] == 'C' || exit[i] == 'c')
                exit[i] = '7';
            else if(exit[i] == 'D' || exit[i] == 'd')
                exit[i] = '9';
            else if(exit[i] == 'E' || exit[i] == 'e')
                exit[i] = '0';
            else if(exit[i] == 'F' || exit[i] == 'f')
                exit[i] = '1';
            else if(exit[i] == 'X' || exit[i] == 'x')
                exit[i] = '4';
        }
        pageTable[count] = atoi(exit);
        count++;
    }
    
    if (pageAlgorithm == 0){
        printf("\n Page Faults: %d\n", FIFO(pageTable, count));
    }
               
    if (pageAlgorithm == 1){
        printf("\n Page Faults: %d\n", LRU(pageTable, count));
    }
    
    return 0;
    
}

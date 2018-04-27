#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main (int argc, char*argv[]) {
    
    int srcFile, newFile;
    char buf[BUFFER_SIZE];
    ssize_t bytesRead = 0, bytesWritten = 0, bytesCopied = 0;
    
    //Checking if user entered two parameters
    if(argc != 3) {
        fprintf(stderr, "ERROR: Two parameters required\n");
        return -1;
    }
    
    
    
    //Opening the source file, read only
    srcFile = open(argv[1], O_RDONLY);
    
    //Checking if source file exists
    if (srcFile < 0) {
        fprintf(stderr, "ERROR: Cannot open source file.\n");
        close(srcFile);
        return -1;
    }
    
    
    
    //Creating the new file, write only, user must create file
    newFile = open(argv[2], O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);
    
    //Checking if new file exists
    if (newFile < 0) {
        fprintf(stderr, "ERROR: Cannot create new file.\n");
        close(srcFile);
        close(newFile);
        return -1;
    }
    
    
    
    //Writing file to new file
    do {
        bytesRead = read(srcFile, buf, BUFFER_SIZE - 1);
        if (bytesRead < 0) {
            fprintf(stderr, "ERROR: Cannot read file.\n");
            close(srcFile);
            close(newFile);
            return -1;
        }
        
        bytesWritten = write(newFile, buf, bytesRead);
        if (bytesWritten < 0) {
            fprintf(stderr, "ERROR: Cannot write to file.\n");
            close(srcFile);
            close(newFile);
            return -1;
        }
        
        bytesCopied += bytesWritten;
        
    } while (bytesRead != 0);
    
    printf("Copied %u bytes.\n", bytesCopied);
    
    close(srcFile);
    close(newFile);
    
    return 0;
}

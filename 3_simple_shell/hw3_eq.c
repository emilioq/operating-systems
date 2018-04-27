#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char parse(char *string, char **argv) {
    char *subStr;
    int i = 0;
    
    //split string into substrings
    subStr = strtok(string, " \t\n");
    while(subStr != NULL) {
        argv[i] = subStr;
        i++;
        if(i > 4) {
            printf("ERROR: Exceeded 4 arguments\n");
            exit(1);
        }
        subStr = strtok(NULL, " \t\n");
            
    }
    
    argv[i] = NULL;
    
    
    //user enter empty line
    if (argv[0] == NULL) {
        printf("No input detected\n");
        return 0;
    }
    
    //exit command
    if(strcmp(argv[0], "exit") == 0){
        exit(0);
    }
    
    return **argv;
    
}


int main () {
    size_t bufsize = 1024;
    char *str = NULL;
    char *argv[4];
    
    pid_t pid;
    
    //loops until exit command or error
    while(1) {
        printf("hw3Shell> ");
        
        //read user input into buffer
        getline(&str, &bufsize, stdin);
        
        //function that splits the string into substrings 
        //and returns the command arguments
        parse(str, argv);
        
        pid = fork();
        
        //fork error
        if (pid == -1) {
            printf("ERROR: Cannot fork\n");
        }

        //user input error
        if (pid == 0) {
            execvp(argv[0], argv);
            printf("ERROR: Cannot run command\n");
            exit(1);
        }
        
        wait(NULL);
    }
}

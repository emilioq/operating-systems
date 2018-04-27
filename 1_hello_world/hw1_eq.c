#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define myName "Emilio Quiambao\n"

int main ()
{
    char myStr[50];
    snprintf(myStr, 50, "\nHello 415, I am %s", myName);
    write(1, myStr, strlen(myStr));
    
    return 0;
}

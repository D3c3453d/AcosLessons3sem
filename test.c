#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void logger(char* str, ...){
    va_list valist;
    va_start(valist, str);

    while (str != NULL) {
        printf("%s", str);
        str = va_arg(valist, char*);
    }
    printf("\n");

    va_end(valist);
}

int main() {
    logger("asdasd  ","asdasdasda", NULL);
}
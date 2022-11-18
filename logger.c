#include <stdarg.h>
#include <stdio.h>

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
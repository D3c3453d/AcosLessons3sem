#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]){
    char *v[] = {"ls", "-lh", (char*) NULL};
    execv("ls", v);

    printf("%s\n", strerror(errno));
    return 0;
}
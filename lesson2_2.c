#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    char *v[] = {"ls", "-lh", (char*) NULL};
    execvp("ls", v);
    return 0;
}
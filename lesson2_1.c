#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char* argv[]){
    char *v[] = {"/bin/ls", "-lh", (char*) NULL};
    execv("/bin/ls", v);
    return 0;
}
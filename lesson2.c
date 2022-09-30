#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>



int main(int argc, char* argv[]){
    //if(fork())
    //    first();
    //else second();
    execl("/bin/ls", "/bin/ls", "/", "-lh", (char*) NULL);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    int d = open("test.txt", O_WRONLY | O_CREAT, 0666);
    int l = write(d, "hello, world!\n", 14);
    close(d);
    return 0;
}
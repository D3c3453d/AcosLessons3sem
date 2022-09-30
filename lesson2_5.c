#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    int d = open("test.txt", O_RDONLY);
    char str[20];
    int l = pread(d, str, 20, 7);
    printf("%s", str);
    close(d);
    return 0;
}
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "./logger.c"

void handler(int sig){
    printf("Writing to pipe failed\n");
}

int main(){
    signal(SIGPIPE, handler);

    int fd = open("label.fifo", O_WRONLY);
    int N = 4;
    char buf[N+1] = "ping";
    int len = N;

    int n = write(fd, buf, len);
    printf("%d\n", n);
    sleep(3);
    n = write(fd, buf, len); // SIGPIPE - канал закрыт
    printf("%d\n", n);
    close(fd);
}
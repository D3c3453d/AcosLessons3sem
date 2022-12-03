#include <string.h>
#include "./logger.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

//clinet
int main(){
    struct sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(7777); // 0 если не важно
    //addrServer.sin_addr.s_addr = INADDR_ANY; // 1 вариант
    inet_aton("127.0.0.1", &addrServer.sin_addr); // 2 вариант

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    //memset(addrSelf.sin_zero, 0, sizeof(addrSelf.sin_zero));
    if(connect(fd, (struct sockaddr*) &addrServer, sizeof(addrServer)) == -1){
        perror(strerror(errno));
        return 1;
    }

    char buf[128];

    if(write(fd, "Hello", 6) == -1){
        perror(strerror(errno));
        return 1;
    }

    int n = read(fd, buf, 128);
    if(n == -1){
        perror(strerror(errno));
        return 1;
    }
    buf[n] = 0;
    printf("%s", buf);
}
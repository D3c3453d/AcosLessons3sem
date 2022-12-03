#include <string.h>
#include "./logger.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

//server
int main(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrSelf;
    addrSelf.sin_family = AF_INET;
    addrSelf.sin_port = htons(7777); // 0 если не важно
    addrSelf.sin_addr.s_addr = INADDR_ANY; // 1 вариант
    //inet_aton("127.0.0.1", &addrSelf.sin_addr); // 2 вариант

    memset(addrSelf.sin_zero, 0, sizeof(addrSelf.sin_zero));

    if(bind(sock, (struct sockaddr*)  &addrSelf, sizeof(addrSelf)) == -1){
        perror(strerror(errno));
        return 1;
    }

    if(listen(sock, 20) == -1){
        perror(strerror(errno));
        return 1;
    }

    struct sockaddr_in addrClient;
    int clientLen = sizeof(addrClient);

    int fd;
    while((fd = accept(sock, (struct sockaddr*) &addrClient, &clientLen))>=0){
        char buf[128];
        int n = read(fd, buf, 128);
        buf[n] = 0;
        printf("%s\n", buf);
        write(fd, buf, n);
        close(fd);
    }
}
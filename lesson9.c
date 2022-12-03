#include <string.h>
#include "./logger.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addrSelf;
    addrSelf.sin_family = AF_INET;
    addrSelf.sin_port = htons(7777); // 0 если не важно
    //addrSelf.sin_addr.s_addr = INADDR_LOOPBACK; // 1 вариант
    inet_aton("127.0.0.1", &addrSelf.sin_addr); // 2 вариант

    memset(addrSelf.sin_zero, 0, sizeof(addrSelf.sin_zero));

    if(bind(fd, (struct sockaddr*)  &addrSelf, sizeof(addrSelf)) == -1){
        perror(strerror(errno));
        return 1;
    }

    int bufSize = 1000;
    while(1){
        struct sockaddr_in addrRemote;
        int addrRemoteLen = sizeof(addrRemote);
        char buf[bufSize];
        int n = recvfrom(fd, buf, bufSize, 0, (struct sockaddr*) &addrRemote, (socklen_t*)&addrRemoteLen);
        //sendto(fd, buf, bufSize, 0, (struct sockaddr*) &addrRemote, addrRemoteLen);
        logger(buf, NULL);
    }
}
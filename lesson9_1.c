#include <string.h>
#include "./logger.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

int main(){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addrSelf;
    addrSelf.sin_family = AF_INET;
    addrSelf.sin_port = htons(0); // 0 если не важно

    //addrSelf.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 1 вариант
    inet_aton("127.0.0.1", &addrSelf.sin_addr); // 2 вариант

    memset(addrSelf.sin_zero, 0, sizeof(addrSelf.sin_zero));

    if(bind(fd, (struct sockaddr*)  &addrSelf, sizeof(addrSelf)) == -1){
        perror(strerror(errno));
        return 1;
    }


    int bufSize = 1000;
    struct sockaddr_in addrRemote;
    //addrRemote.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    inet_aton("127.0.0.1", &addrRemote.sin_addr);
    addrRemote.sin_port = htons(7777);
    int addrRemoteLen = sizeof(addrRemote);

    while(1) {
        char buf[bufSize];
        buf[0] = 'A';
        sendto(fd, buf, bufSize, 0, (struct sockaddr *) &addrRemote, addrRemoteLen);
        int n = recvfrom(fd, buf, bufSize, 0, (struct sockaddr *) &addrRemote, (socklen_t * ) & addrRemoteLen);
    }
}
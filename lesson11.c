#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./logger.c"
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

//server
int main(){
    char* socketName = "./client.sock";
    unlink(socketName)
    int sock;
    if((sock = socket(PF_UNIX, SOCK_SEQPACKET, 0)) == -1){
        perror(strerror(errno));
        return 1;
    }

    struct sockaddr_un addrSelf;
    memset(&addrSelf, 0, sizeof(sockaddr_un));

    addrSelf.sun_family = AF_UNIX;
    strcpy(addrSelf.sun_path, socketName, sizeof(addrSelf.sun_path)-1);
    int addrLen = SUN_LEN(&addrSelf);

    if(bind(sock, (struct sockaddr*)  &addrSelf, addrLen) == -1){
        perror(strerror(errno));
        return 1;
    }

    if(listen(sock, 20) == -1){
        perror(strerror(errno));
        return 1;
    }

    struct sockaddr_un addrRemote;
    int addrRemoteLen = sizeof(addrRemote);
    int dataSock;
    while((dataSock = accept(sock, (struct sockaddr*) &addrRemote, (socklen_t *)&addrRemoteLen))>=0){
        addrRemoteLen = sizeof(addrRemote);
        char buf[128];
        int n = read(sock, buf, 128);
        buf[n] = 0;
        printf("%s\n", buf);
        write(sock, buf, n);
        close(sock);
    }
}
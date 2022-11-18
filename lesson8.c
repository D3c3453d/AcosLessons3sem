#include <fcntl.h>
#include <unistd.h>
#include "./logger.c"

/*
 fifo начинает работать только тогда,когда он одновременно открыт и на запись и на чтение,
 если программы его откроет только в одном режиме(только на запись или только на чтение), то они будут ждать.
 т.е. нужно чтобы в одно месте был: fd = open("label.fifo", O_RDONLY),
 а в другом: fd = open("label.fifo", O_WRONLY)

 cat < label.fifo вывод
 cat > label.fifo ввод
*/


int main(){
    int fd = open("label.fifo", O_RDONLY);
    int N = 4;
    char buf[N];

    int n = 0;
    int len = N;
    do {
        n+=read(fd, buf+n, len); //возвращает число считанных байт, 0 если канал закрыт или eof
    } while (n<N);

    buf[n] = 0; //конвертация принятых байт в строку?

    logger(buf, NULL);

    close(fd);
}
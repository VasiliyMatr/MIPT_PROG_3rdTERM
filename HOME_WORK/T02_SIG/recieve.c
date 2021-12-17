
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int fd = 0;

void actFunc( int signal )
{
    static size_t bitId = 0;
    static unsigned char byte = 0;

    if (signal == SIGCONT)
    {
        if (kill(-1, SIGCONT) == -1)
            exit (-1);
        return;
    }

    byte += (signal == SIGUSR2) << (7 - bitId++ % 8);
    int writeStatus = 0;
    if (bitId % 8 == 0)
    {
        if (write (fd, &byte, 1) != 1)
            exit (-1);
        byte = 0;
    }

    if (kill(-1, SIGCONT) == -1)
        exit (-1);
}

int main( int argc, char ** argv )
{
    if (argc != 2)
        exit (-1);

    printf("%d\n", getpid());

    const char * fileName = argv[1];
    fd = open (fileName, O_CREAT | O_WRONLY, 0777);
    if (fd == -1)
        exit (-1);

    struct sigaction act;
    act.sa_handler = &actFunc;
    act.sa_flags = 0;

    if (sigaction (SIGUSR1, &act, NULL) == -1)
        exit (-1);
    if (sigaction (SIGUSR2, &act, NULL) == -1)
        exit (-1);
    if (sigaction (SIGCONT, &act, NULL) == -1)
        exit (-1);

    while (1) pause ();
}
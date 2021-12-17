
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

int pid = 0;
int fd = 0;

void actFunc( int signal )
{
    static size_t bitId = 0;
    static unsigned char symbol = 0;
    static int status = 0;

    if (bitId % 8 == 0)
        status = read (fd, &symbol, 1);

    if (status != 1)
    {
        if (kill (pid, SIGKILL) == -1)
            exit(-1);
        exit (0);
    }
    else
    {
        if (((unsigned int)symbol >> (7 - (bitId++ % 8))) & 1)
        {
            if (kill (pid, SIGUSR2) == -1)
                exit(-1);
        }
        else
        {
            if (kill (pid, SIGUSR1) == -1)
                exit(-1);
        }
    }
}

int main( int argc, char ** argv )
{
    if (argc != 3)
        exit (-1);

    pid = atoi (argv[2]);
    if (pid <= 0)
        exit (-1);

    const char * fileName = argv[1];
    fd = open (fileName, O_RDONLY);
    if (fd == -1)
        exit (-1);

    struct sigaction act;
    act.sa_handler = &actFunc;
    act.sa_flags = 0;

    if (sigaction (SIGCONT, &act, NULL) == -1)
        exit (-1);

    if (kill (pid, SIGCONT) == -1)
        exit (-1);
    while (1) pause ();
}

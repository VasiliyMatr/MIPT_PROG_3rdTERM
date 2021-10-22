
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define COND_PRINT_ERR_N_RET( COND, STR )   \
do {                                        \
if (COND)                                   \
{                                           \
    int errnoCpy = errno;                   \
    fprintf (stderr,                        \
        STR "\n"                            \
        "%s" "\n",                          \
        strerror (errnoCpy)                 \
    );                                      \
                                            \
    return errnoCpy;                        \
}                                           \
}                                           \
while (0)

// <prog name> <out file name> <to exec>
int main( int argc, char **argv )
{
    if (argc < 3)
    {
        fprintf (stderr,
            "Incorrect usage:" "\n"
            "Expected: %s <out file name> <to exec>" "\n",
            argv[0]);
        return -1;
    }

    #define MAX_ARG_NUM 4096
    if (argc > MAX_ARG_NUM)
    {
        fprintf (stderr,
            "To many args, max val is %d" "\n",
            MAX_ARG_NUM);
        return -1;
    }

    int fd = open (argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    COND_PRINT_ERR_N_RET (fd == -1, "Can't open output file:");
    COND_PRINT_ERR_N_RET (dup2 (fd, STDOUT_FILENO) == -1, "Error with output file:");
    close (fd);
    execvp (argv[2], argv + 2);
    COND_PRINT_ERR_N_RET (1, "Error with <to exec> arg:");

    return 0;
}
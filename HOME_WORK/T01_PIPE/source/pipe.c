
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wordexp.h>
#define _GNU_SOURCE
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

#include "../headers/errors.h"

#define BUFFS_SIZES 1024

int main( int argc, char ** argv )
{
    condPrintNExit (argc != 2, "Wrong number of args" "\n", EINVAL);
    
    int fd = open (argv[1], O_RDONLY);
    condPrintNExit (fd == -1, "Can't open input file:", errno);

    char buff[BUFFS_SIZES];
    for (int i = 0; i < BUFFS_SIZES; ++i) {buff[i] = 0;}

    ssize_t readenBytesNum = read (fd, buff, BUFFS_SIZES);
    condPrintNExit (readenBytesNum == -1, "Error with input file:", errno);
    condPrintNExit (readenBytesNum == 0, "Empty input file.", EINVAL);

    char * ptrs[BUFFS_SIZES];
    for (int i = 0; i < BUFFS_SIZES; ++i) {ptrs[i] = NULL;}

    ptrs[0] = buff;
    size_t pipeSize = 1;

    for (; pipeSize < BUFFS_SIZES; ++pipeSize) // searching for all piped cmds
    {
        char * nextPipePose = strchr (ptrs[pipeSize-1], '|');
        if (nextPipePose == NULL)
            break;
        *nextPipePose = '\0';
        ptrs[pipeSize] = nextPipePose + 1;
    }

    wordexp_t we = {.we_wordv = NULL, .we_offs = 0, .we_wordc = 0};

    for (size_t cmdId = 0; cmdId < pipeSize; ++cmdId)
    {
        int pipeFds[2] = {-1, -1};
        int pipeRetVal = pipe (pipeFds);
        condPrintNExit (pipeRetVal != 0, "Can't open pipe:", errno);

        wordexp (ptrs[cmdId], &we, WRDE_REUSE);

        int pid = fork ();
        condPrintNExit (pid == -1, "Fork error:", errno);
        if (pid ==  0)
        {
            dup2 (pipeFds[1], STDOUT_FILENO);

            execvp (we.we_wordv[0], we.we_wordv);
            condPrintNExit (1, "execvp error:", errno);
            return -1;
        }

        close (pipeFds[1]);
        dup2 (pipeFds[0], STDIN_FILENO);
        close (pipeFds[0]);
    }

    wordfree (&we);

    int c = 0;
    while ((c = getchar ()) != EOF)
        putc (c, stderr);

    return 0;
}
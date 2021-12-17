
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
#include <sys/wait.h>

#include "../headers/errors.h"

#define BUFFS_SIZES 1024

static void clearBuff( char * buff, ssize_t size );
static size_t parsePipes( char * buff, char ** cmdPtrs );

int main( int argc, char ** argv )
{
    condPrintNExit (argc != 2, "Wrong number of args" "\n", EINVAL);
    
    int fd = open (argv[1], O_RDONLY);
    condPrintNExit (fd == -1, "Can't open input file:", errno);

    char buff[BUFFS_SIZES] = "";

    ssize_t readenBytesNum = read (fd, buff, BUFFS_SIZES);
    condPrintNExit (readenBytesNum == -1, "Error with input file:", errno);
    condPrintNExit (readenBytesNum == 0, "Empty input file.", EINVAL);
    close (fd);

    clearBuff (buff, readenBytesNum);

    char * cmdPtrs[BUFFS_SIZES] = { NULL };
    size_t pipeSize = parsePipes (buff, cmdPtrs);

    wordexp_t we = {.we_wordv = NULL, .we_offs = 0, .we_wordc = 0};
    int pid = 0;

    for (size_t cmdId = 0; cmdId < pipeSize; ++cmdId)
    {
        int pipeFds[2] = {-1, -1};
        int pipeRetVal = pipe (pipeFds);
        condPrintNExit (pipeRetVal, "Can't open pipe:", errno);

        int wordexpStatus = wordexp (cmdPtrs[cmdId], &we, WRDE_REUSE);
        condPrintNExit (wordexpStatus, "Error with parsing cmd line.", EINVAL);

        pid = fork ();
        condPrintNExit (pid == -1, "Fork error:", errno);
        if (pid ==  0)
        {
            if (cmdId != pipeSize - 1)
            {
                int dupStatus = dup2 (pipeFds[1], STDOUT_FILENO);
                condPrintNExit (dupStatus == -1, "dup2 error:", errno);
            }
            close (pipeFds[0]);
            close (pipeFds[1]);

            execvp (we.we_wordv[0], we.we_wordv);
            fprintf (stderr, "When tried to exec \'%s\':" "\n", cmdPtrs[cmdId]);
            condPrintNExit (1, "execvp error:", errno);

            return -1;
        }

        if (cmdId != pipeSize - 1)
        {
            int dupStatus = dup2 (pipeFds[0], STDIN_FILENO);
            condPrintNExit (dupStatus == -1, "dup2 error:", errno);
        }

        close (pipeFds[1]);
        close (pipeFds[0]);
    }

    wordfree (&we);

    int status = 0;
    pid_t waitRetVal = waitpid (pid, &status, 0);
    condPrintNExit (waitRetVal != pid, "wait error:", errno);

    return 0;
}

static void clearBuff( char * buff, ssize_t size )
{
    for (ssize_t i = 0; i < size; ++i)
        if (isspace (buff[i]))
            buff[i] = ' ';
}

static size_t parsePipes( char * buff, char ** cmdPtrs )
{
    cmdPtrs[0] = buff;
    size_t pipeSize = 1;

    for (; pipeSize < BUFFS_SIZES; ++pipeSize) // searching for all piped cmds
    {
        char * nextPipePose = strchr (cmdPtrs[pipeSize-1], '|');
        if (nextPipePose == NULL)
            break;
        *nextPipePose = '\0';
        cmdPtrs[pipeSize] = nextPipePose + 1;
    }

    return pipeSize;
}
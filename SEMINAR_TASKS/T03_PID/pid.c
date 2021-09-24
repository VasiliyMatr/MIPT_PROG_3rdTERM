

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

int main()
{

    const char OUT_NAME[] = "pid-out.txt";
    const size_t PROCS_NUM = 1024;
    char buff[64] = "";

    int fd = open (OUT_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
    {
        return -1;
    }

    for (int procNum = 0; procNum < PROCS_NUM; ++procNum)
    {
        pid_t pid = fork ();
        if (pid == -1)
        {
            close (fd);
            return -1;
        }

        if (pid == 0)
        {
            int noteSize = sprintf (buff,
                     "PID: %d PPID: %d" "\n",
                     getpid (), getppid ());

            ssize_t writtenNum = write (fd, buff, noteSize);
            if (writtenNum == -1)
                return -1;
            return 0;
        }

        int status = 0;
        pid_t waitRetVal = waitpid (pid, &status, 0);

        if (!WIFEXITED (status) || WEXITSTATUS(status))
        {
            close (fd);
            return -1;
        }

        int noteSize = sprintf (buff,
                 "CPID: %d" "\n",
                 waitRetVal);

        ssize_t writtenNum = write (fd, buff, noteSize);
        if (writtenNum == -1)
        {
            close (fd);
            return -1;
        }
    }

    return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <poll.h>

int main( int argc, char ** argv )
{
    struct pollfd fds[4096];
    if (argc > sizeof (fds) / sizeof (struct pollfd))
    {
        fprintf (stderr, "To large input." "\n\n");
        return -1;
    }

    size_t filesNum = argc - 1;

    for (size_t argId = 1; argId < argc; ++argId)
    {
        int fd = open (argv[argId], O_RDONLY | O_CREAT, 0666);

        if (fd == -1)
        {
            int errno_ = errno;
            fprintf (stderr, "Error with input file %s" "\n", argv[argId]);
            fprintf (stderr, "%s" "\n", strerror (errno_));
            return -1;
        }

        fds[argId - 1].fd = fd;
        fds[argId - 1].events = POLL_IN;
    }

    while (1)
    {
        int pollStatus = poll (fds, filesNum, -1);
        if (pollStatus == -1)
        {
            int errno_ = errno;
            fprintf (stderr, "Error with poll" "\n");
            fprintf (stderr, "%s" "\n", strerror (errno_));
        }

        for (size_t fdId = 0; fdId < filesNum; ++fdId)
        {
            if (fds[fdId].revents & POLLIN) // copying all data to stdout
            {
                char buff[4096] = {};

                for (;;)
                {
                    int numOfReadenBytes = read (fds[fdId].fd, buff, sizeof (buff));
                    if (numOfReadenBytes == -1)
                    {
                        int errno_ = errno;
                        fprintf (stderr, "Error with input file %s" "\n", argv[fdId + 1]);
                        fprintf (stderr, "%s" "\n", strerror (errno_));
                        return -1;
                    }

                    printf ("%.*s", numOfReadenBytes, buff);

                    if (numOfReadenBytes < sizeof (buff))
                        break;
                }
            }
        }
    }
}
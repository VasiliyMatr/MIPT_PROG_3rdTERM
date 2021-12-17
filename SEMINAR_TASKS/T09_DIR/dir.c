
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

typedef struct dirent direntry;

void printErrorNExit( const char* msg, int error )
{
    fprintf (stderr, "%s" "\n", msg);
    if (error != EINVAL)
        fprintf (stderr, "%s" "\n", strerror (error));

    exit (-1);
}

int main( int argc, char** argv )
{
    if (argc != 2)
        printErrorNExit ("Wrong args.", EINVAL);

    DIR* dir = opendir (argv[1]);
    if (dir == NULL)
        printErrorNExit ("Error with directory open:", errno);

    errno = 0;
    direntry* entry = readdir (dir);

    while (entry != NULL)
    {
        printf ("%s" "\n", entry->d_name);
        entry = readdir (dir);
    }

    if (errno != 0)
        printErrorNExit ("Error with directory read:", errno);

    return 0;
}
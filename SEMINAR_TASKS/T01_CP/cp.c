
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

const int ERR_FILE_DESCR_VAL = -1;

int main( int argc, char ** argv )
{

    if (argc != 3)
    {
        fprintf (stderr, "usage: %s <src> <dst>" "\n", argv[0]);
        return -1;
    }

    char* srcPath = argv[1];
    char* dstPath = argv[2];

    int srcDescr = open (srcPath, O_RDONLY);
    if (srcDescr == ERR_FILE_DESCR_VAL)
    {
        perror ("open");
        return -1;
    }

    int dstDescr = open (dstPath, O_WRONLY | O_CREAT, 0666);
    if (dstDescr == ERR_FILE_DESCR_VAL)
    {
        perror ("open");
        close (srcDescr);
        return -1;
    }

    const size_t buffSize = 4096;
    char buff[buffSize] = {};

    while (1)
    {
        int readedBytesNum = read (srcDescr, (void*) buff, buffSize);

        if (readedBytesNum == ERR_FILE_DESCR_VAL)
        {
            perror ("read");
            close (srcDescr);
            close (dstDescr);
            return -1;
        }

        if (readedBytesNum == 0)
        {
            close (srcDescr);
            close (dstDescr);
            return 0;
        }

        int writtenBytesNum = 0;

        for (int addWrittenBytesNum = 0; writtenBytesNum < readedBytesNum; writtenBytesNum += addWrittenBytesNum)
        {
            addWrittenBytesNum =
                write (dstDescr, (void*) (buff + writtenBytesNum), readedBytesNum - writtenBytesNum);

            if (addWrittenBytesNum == ERR_FILE_DESCR_VAL)
            {
                perror ("write");
                close (srcDescr);
                close (dstDescr);
                return -1;
            }
        }
    }

    return 0;
}

#include <trans.h>

static struct errStatus_t openInAndOut( char * inName, char * outName,
                                        int * inDescrPtr, int* outDescrPtr );

struct errStatus_t transByArgs( int argc, char ** argv )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    if (argc > 3) { errStatus.errCode_ = EC_2MANY_ARGS_; return errStatus; }
    if (argc < 3) { errStatus.errCode_ = EC_2FEW_ARGS_; return errStatus; }

    if (isBadPtr (argv) || isBadPtr (argv[0]) ||
        isBadPtr (argv[1]) || isBadPtr (argv[2]))
    {
        errStatus.errCode_ = EC_BAD_PTR_;
        return errStatus;
    }

    char * inName = argv[1];
    char * outName = argv[2];
    int inDescr = -1;
    int outDescr = -1;

    errStatus = openInAndOut (inName, outName, &inDescr, &outDescr);
    if (errStatus.errCode_ != EC_OK_)
        return errStatus;

    errStatus = transByDescr (inDescr, outDescr);
    if (errStatus.errCode_ != EC_OK_)
        return errStatus;

  // ??? COULD HAVE ERRORS HERE ???
    close (inDescr);
    close (outDescr);

    return errStatus;
}

static struct errStatus_t openInAndOut( char * inName, char * outName,
                                        int * inDescrPtr, int* outDescrPtr )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    if (isBadPtr (inName) || isBadPtr (outName) ||
        isBadPtr (inDescrPtr) || isBadPtr (outDescrPtr))
    {
        errStatus.errCode_ = EC_BAD_PTR_;
        return errStatus;
    }

    *inDescrPtr = open (inName, O_RDONLY);
    if (*inDescrPtr == -1)
    {
        errStatus.errCode_ = EC_IN_ERR_;
        errStatus.errno_ = errno;
        return errStatus;
    }

    *outDescrPtr = open (outName, O_CREAT | O_WRONLY);
    if (*outDescrPtr == -1)
    {
        errStatus.errCode_ = EC_OUT_ERR_;
        errStatus.errno_ = errno;
        return errStatus;
    }

    errStatus.errCode_ = EC_OK_;
    return errStatus;
}

struct errStatus_t transByDescr( int inDescr, int outDescr )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    if (inDescr < 0 || outDescr < 0)
    {
        errStatus.errCode_ = EC_BAD_FILE_DESCR_;
        return errStatus;
    }

    const size_t buffSize = 4096;
    char buff[4096] = "";

    while (1)
    {
        int numOfReadenBytes = read (inDescr, buff, buffSize);
        if (numOfReadenBytes == -1)
        {
            errStatus.errCode_ = EC_IN_ERR_;
            errStatus.errno_ = errno;
            return errStatus;
        }

        int numOfWrittenBytes = write (outDescr, buff, numOfReadenBytes);
        if (numOfWrittenBytes == -1)
        {
            errStatus.errCode_ = EC_OUT_ERR_;
            errStatus.errno_ = errno;
            return errStatus;
        }

        if (numOfReadenBytes != buffSize)
            break;
    }

    errStatus.errCode_ = EC_OK_;
    return errStatus;
}
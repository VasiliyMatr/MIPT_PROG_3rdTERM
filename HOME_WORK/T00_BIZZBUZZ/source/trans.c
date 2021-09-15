
#include <trans.h>

#define COND_RET( COND_, EC_ )                     \
if (COND_)                                         \
{                                                  \
    struct errStatus_t errStatus = { EC_, errno }; \
    return errStatus;                              \
}

enum transPolicy_t
{
    TP_COPY = 0,
    TP_REPL_BIZZ,
    TP_REPL_BUZZ,
    TP_REPL_BIZZ_BUZZ
};

static struct errStatus_t openInAndOut( char * inName, char * outName,
                                        int * inDescrPtr, int* outDescrPtr );

static struct errStatus_t transNumber( int inDescr, int outDescr );

static struct errStatus_t copy(
    int srcDescr, int dstDescr, size_t numOfBytes2Copy );

struct errStatus_t transByArgs( int argc, char ** argv )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    COND_RET (argc > 3, EC_2MANY_ARGS_);
    COND_RET (argc < 3, EC_2FEW_ARGS_);
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
    {
        close (inDescr);
        close (outDescr);

        return errStatus;
    }

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

    *inDescrPtr = open (inName, O_RDONLY | O_LARGEFILE);
    COND_RET (*inDescrPtr == -1, EC_IN_ERR_);

    *outDescrPtr = open (outName, O_CREAT | O_WRONLY | O_LARGEFILE, 0600);
    COND_RET (*outDescrPtr == -1, EC_OUT_ERR_);

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

    char buff[4096] = "";

    ssize_t readenBytesNum = read (inDescr, buff, sizeof (buff));

    while (readenBytesNum != 0)
    {
        COND_RET (readenBytesNum == -1, EC_IN_ERR_);

        size_t buffOffset = 0;
        char prevByte = ' ';

        for (; buffOffset < readenBytesNum; ++buffOffset)
        {
            char byte = buff[buffOffset];

            if (isspace (prevByte) && (isdigit (byte) || byte == '-'))
            {
                ssize_t writtenBytesNum = write (outDescr, buff, buffOffset);
                COND_RET (readenBytesNum == -1, EC_OUT_ERR_);

                off64_t newFilePos =
                    lseek64 (inDescr, writtenBytesNum - readenBytesNum, SEEK_CUR);
                COND_RET (newFilePos == -1, EC_IN_ERR_);

                errStatus = transNumber (inDescr, outDescr);
                if (errStatus.errCode_ != EC_OK_)
                    return errStatus;

                break;
            }

            prevByte = byte;
        }

        if (buffOffset == readenBytesNum)
        {
            ssize_t writtenBytesNum = write (outDescr, buff, buffOffset);
            COND_RET (writtenBytesNum == -1, EC_OUT_ERR_);
        }

        readenBytesNum = read (inDescr, buff, sizeof (buff));
    }

    errStatus.errCode_ = EC_OK_;
    return errStatus;
}

static const char BIZZ[] = "bizz";
static const char BUZZ[] = "buzz";

static struct errStatus_t transNumber( int inDescr, int outDescr )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    if (inDescr < 0 || outDescr < 0)
    {
        errStatus.errCode_ = EC_BAD_FILE_DESCR_;
        return errStatus;
    }

    char buff[4096] = "";
    size_t buffOffset = 0;

    off64_t startOffset = lseek64 (inDescr, 0, SEEK_CUR);
    COND_RET (startOffset == -1, EC_IN_ERR_);

    ssize_t readenBytesNum = read (inDescr, buff, sizeof (buff)); // errors check will be made later

    if (buff[0] == '-')
        ++buffOffset;

    bool canBeTransformed = 1;
    bool shouldTranslate = 0;
    size_t digitsSum = 0;
    size_t lastDigit = 0;

    while (readenBytesNum != 0)
    {
        COND_RET (readenBytesNum == -1, EC_IN_ERR_);

        for (; buffOffset < readenBytesNum; ++buffOffset)
        {
            char byte = buff[buffOffset];

            if (isdigit(byte))
            {
                digitsSum += byte - '0';
                lastDigit = byte - '0';
            }

            else
            {
                if (isspace(byte))
                {
                    shouldTranslate = 1;
                    break;
                }

                canBeTransformed = 0;
            }
        }

        if (shouldTranslate)
            break;

        readenBytesNum = read (inDescr, buff, sizeof (buff)); // check in the cycles beginning
        
        buffOffset = 0;
    }

    int writtenBytesNum = 0;
    bool buzzed = !(digitsSum % 3);
    bool bizzed = !(lastDigit % 5);

    if (canBeTransformed && (buzzed || bizzed)) // translating number
    {
        if (buzzed)
        {
            writtenBytesNum = write (outDescr, BUZZ, sizeof (BUZZ) - 1);
            COND_RET (writtenBytesNum == -1, EC_OUT_ERR_);
        }

        if (bizzed)
        {
            writtenBytesNum = write (outDescr, BIZZ, sizeof (BIZZ) - 1);
            COND_RET (writtenBytesNum == -1, EC_OUT_ERR_);
        }

        off64_t newFilePos = lseek64 (inDescr, -(readenBytesNum - buffOffset), SEEK_CUR);
        COND_RET (newFilePos == -1, EC_IN_ERR_);

        errStatus.errCode_ = EC_OK_;
        return errStatus;
    }

    off64_t currOffset = lseek64 (inDescr, 0, SEEK_CUR);
    COND_RET (currOffset == -1, EC_IN_ERR_);
    off64_t newFilePos = lseek64 (inDescr, startOffset, SEEK_SET);
    COND_RET (newFilePos == -1, EC_IN_ERR_);

    size_t copySize = currOffset - (readenBytesNum - buffOffset) - startOffset;

    return copy (inDescr, outDescr, copySize);
}

#define MIN( FT, SD ) FT < SD ? FT : SD

static struct errStatus_t copy( int srcDescr, int dstDescr, size_t numOfBytes2Copy )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    char buff[4096] = "";

    for (size_t copiedBytesNum = 0; copiedBytesNum < numOfBytes2Copy;)
    {
        size_t toReadBytesNum = sizeof (buff) > numOfBytes2Copy - copiedBytesNum ?
            numOfBytes2Copy - copiedBytesNum : sizeof (buff);

        int readenBytesNum = read (srcDescr, buff, toReadBytesNum);
        COND_RET (readenBytesNum == -1, EC_IN_ERR_);

        int writtenBytesNum = write (dstDescr, buff, readenBytesNum);
        COND_RET (writtenBytesNum == -1, EC_OUT_ERR_);

        copiedBytesNum += writtenBytesNum;
    }

    errStatus.errCode_ = EC_OK_;
    return errStatus;
}
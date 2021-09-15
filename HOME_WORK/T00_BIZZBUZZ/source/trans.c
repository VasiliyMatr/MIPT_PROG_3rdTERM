
#include <trans.h>

#define COND_ERR_STAT_RET( COND_, EC_ )             \
if (COND_)                                          \
{                                                   \
    struct errStatus_t errStatus = {EC_, errno };   \
    return errStatus;                               \
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

    COND_ERR_STAT_RET (argc > 3, EC_2MANY_ARGS_);
    COND_ERR_STAT_RET (argc < 3, EC_2FEW_ARGS_);
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
    COND_ERR_STAT_RET (*inDescrPtr == -1, EC_IN_ERR_);

    *outDescrPtr = open (outName, O_CREAT | O_WRONLY, 0777);
    COND_ERR_STAT_RET (*outDescrPtr == -1, EC_OUT_ERR_);

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

    #define BUFF_SIZE 4096
    char buff[BUFF_SIZE] = "";

    while (1)
    {
        int readenBytesNum = read (inDescr, buff, BUFF_SIZE);
        if (readenBytesNum == -1)
        {
            errStatus.errCode_ = EC_IN_ERR_;
            errStatus.errno_ = errno;
            return errStatus;
        }

        if (readenBytesNum == 0)
        {
            errStatus.errCode_ = EC_OK_;
            return errStatus;
        }

        int buffOffset = 0;
        char prevByte = ' ';

        for (; buffOffset < readenBytesNum; ++buffOffset)
        {
            char byte = buff[buffOffset];

            if (isspace (prevByte) && (isdigit (byte) || byte == '-'))
            {
                int writtenBytesNum = write (outDescr, buff, buffOffset);
                if (writtenBytesNum == -1)
                {
                    errStatus.errCode_ = EC_OUT_ERR_;
                    errStatus.errno_ = errno;
                    return errStatus;
                }

                lseek (inDescr, writtenBytesNum - readenBytesNum, SEEK_CUR);

                errStatus = transNumber (inDescr, outDescr);
                if (errStatus.errCode_ != EC_OK_)
                    return errStatus;

                break;
            }

            prevByte = byte;
        }

        if (buffOffset == readenBytesNum)
        {
            int writtenBytesNum = write (outDescr, buff, buffOffset);
            if (writtenBytesNum == -1)
            {
                errStatus.errCode_ = EC_OUT_ERR_;
                errStatus.errno_ = errno;
                return errStatus;
            }
        }
    }

    errStatus.errCode_ = EC_OK_;
    return errStatus;

    #undef BUFF_SIZE
}

static const char BIZZ[] = "bizz";
static const char BUZZ[] = "buzz";
static const char BIZZ_BUZZ[] = "bizz-buzz";

static struct errStatus_t transNumber( int inDescr, int outDescr )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    if (inDescr < 0 || outDescr < 0)
    {
        errStatus.errCode_ = EC_BAD_FILE_DESCR_;
        return errStatus;
    }

    #define BUFF_SIZE 4096
    char buff[BUFF_SIZE] = "";
    int buffOffset = 0;

    off_t startOffset = lseek (inDescr, 0, SEEK_CUR);

    int readenBytesNum = read (inDescr, buff, BUFF_SIZE);
    if (buff[0] == '-')
        ++buffOffset;

    int canBeTransformed = 1;
    int digitsSum = 0;
    int lastDigit = 0;

    while (readenBytesNum != 0)
    {
        if (readenBytesNum == -1)
        {
            errStatus.errCode_ = EC_IN_ERR_;
            errStatus.errno_ = errno;
            return errStatus;
        }

        for (; buffOffset < readenBytesNum; ++buffOffset)
        {
            char byte = buff[buffOffset];

            if (isdigit(byte))
            {
                digitsSum += byte - '0';
                lastDigit = byte;
            }

            else
            {
                if (isspace(byte))
                {
                    int writtenBytesNum = 0;

                    if (!canBeTransformed)
                    {
                        off_t currOffset = lseek (inDescr, 0, SEEK_CUR);
                        lseek (inDescr, startOffset, SEEK_SET);

                        return copy (inDescr, outDescr, currOffset - (readenBytesNum - buffOffset) - startOffset);
                    }

                    if (lastDigit == '5' || lastDigit == '0')
                    {
                        if (digitsSum % 3 == 0)
                            writtenBytesNum = write (outDescr, BIZZ_BUZZ, sizeof (BIZZ_BUZZ) - 1);
                        else
                            writtenBytesNum = write (outDescr, BIZZ, sizeof (BIZZ) - 1);
                    }

                    else if (digitsSum % 3 == 0)
                        writtenBytesNum = write (outDescr, BUZZ, sizeof (BUZZ) - 1);
                    else
                    {
                        off_t currOffset = lseek (inDescr, 0, SEEK_CUR);
                        lseek (inDescr, startOffset, SEEK_SET);

                        return copy (inDescr, outDescr, currOffset - (readenBytesNum - buffOffset) - startOffset);
                    }

                    if (writtenBytesNum == -1)
                    {
                        errStatus.errCode_ = EC_OUT_ERR_;
                        errStatus.errno_ = errno;
                        return errStatus;
                    }

                    lseek (inDescr, -(readenBytesNum - buffOffset), SEEK_CUR);

                    errStatus.errCode_ = EC_OK_;
                    return errStatus;
                }

                canBeTransformed = 0;
            }
        }

        readenBytesNum = read (inDescr, buff, BUFF_SIZE);
        buffOffset = 0;
    }

    #undef BUFF_SIZE

    errStatus.errCode_ = EC_UNDEF_ERR_;
    return errStatus;
}

#define MIN( FT, SD ) FT < SD ? FT : SD

static struct errStatus_t copy( int srcDescr, int dstDescr, size_t numOfBytes2Copy )
{
    struct errStatus_t errStatus = { EC_UNDEF_ERR_, EINVAL };

    #define BUFF_SIZE 4096
    char buff[BUFF_SIZE] = "";

    for (size_t copiedBytesNum = 0; copiedBytesNum < numOfBytes2Copy;)
    {
        int readenBytesNum = read (srcDescr, buff,
                MIN (BUFF_SIZE, numOfBytes2Copy - copiedBytesNum));
        int writtenBytesNum = write (dstDescr, buff, readenBytesNum);

        copiedBytesNum += writtenBytesNum;
    }

    errStatus.errCode_ = EC_OK_;
    return errStatus;
}
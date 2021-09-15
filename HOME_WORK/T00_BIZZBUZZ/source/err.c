
#include <err_.h>

const char* getErrMsg( enum errCode_t errCode )
{
    switch (errCode)
    {
        case EC_OK_:
            return EM_OK_;
        case EC_2MANY_ARGS_:
            return EM_2MANY_ARGS_;
        case EC_2FEW_ARGS_:
            return EM_2FEW_ARGS_;
        case EC_IN_ERR_:
            return EM_IN_ERR_;
        case EC_OUT_ERR_:
            return EM_OUT_ERR_;

        default:
            return EM_UNDEF_ERR_;
    }
}

void printErrStatus( struct errStatus_t errStatus )
{
    fprintf (stderr, "%s" "\n", getErrMsg (errStatus.errCode_));
    if (errStatus.errno_ != EINVAL && errStatus.errno_ != 0)
        fprintf (stderr, "%s" "\n", strerror (errStatus.errno_));
    fputc ('\n', stderr);
}

int isBadPtr( void* ptr )
{
    return ptr == NULL;
}
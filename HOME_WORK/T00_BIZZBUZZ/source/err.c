
#include <err.h>

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

int isBadPtr( void* ptr )
{
    return ptr == NULL;
}
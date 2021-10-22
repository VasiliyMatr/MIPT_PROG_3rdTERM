
#include "../headers/errors.h"

void condPrintNExit( bool condVal, const char * errorMsg, int errnoVal )
{
    if (condVal)
    {
        fprintf (stderr, "%s", errorMsg);
        if (errnoVal != EINVAL)
            fprintf (stderr, "\n" "%s" "\n", strerror (errnoVal));
        exit (0);
    }
}
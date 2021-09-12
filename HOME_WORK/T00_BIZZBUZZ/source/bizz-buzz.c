
#include <trans.h>

int main (int argc, char ** argv)
{
    struct errStatus_t errStatus = transByArgs (argc, argv);

    if (errStatus.errCode_ != EC_OK_)
    {
        fprintf (stderr, "%s" "\n", getErrMsg (errStatus.errCode_));
        if (errStatus.errno_ != EINVAL)
            fprintf (stderr, "%s" "\n", strerror (errStatus.errno_));
        fputc ('\n', stderr);

        return -1;
    }

    return 0;
}
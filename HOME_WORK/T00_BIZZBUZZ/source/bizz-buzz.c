
#include <trans.h>

int main (int argc, char ** argv)
{
    struct errStatus_t errStatus = transByArgs (argc, argv);

    if (errStatus.errCode_ != EC_OK_)
    {
        printErrStatus (errStatus);
        return -1;
    }

    return 0;
}
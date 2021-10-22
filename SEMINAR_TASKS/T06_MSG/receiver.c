
#include "msg.h"

int main()
{
    int qid = msgget (ftok ("a", 100), IPC_CREAT | 0660);
    if (qid == -1)
        return -1;

    while (1)
    {
        struct mbuf buf = {.mtype = 0};
        int status = msgrcv (qid, (void*)&buf, BUFF_SIZE, 0, 0);
        if (status == -1)
            return -1;

        printf ("%s", buf.mtext);
    }
}
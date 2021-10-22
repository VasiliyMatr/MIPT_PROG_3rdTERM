
#include "msg.h"

int main()
{
    int qid = msgget (ftok ("a", 100), IPC_CREAT | 0660);
    if (qid == -1)
        return -1;

    while (1)
    {
        char * line = NULL;
        size_t len = 0;
        ssize_t status = getline (&line, &len, stdin);
        if (len > BUFF_SIZE)
        {
            free (line);
            return -1;
        }

        struct mbuf buf = {.mtype = 1};
        memcpy (buf.mtext, line, len);
        status = msgsnd (qid, (void*)&buf, BUFF_SIZE, 0);
        if (status == -1)
            return -1;
    }
}
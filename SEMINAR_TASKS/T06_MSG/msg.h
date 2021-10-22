
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>

#define BUFF_SIZE 1024

struct mbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[BUFF_SIZE];    /* message data */
};


#include <pthread.h>
#include <math.h>
#include <stdio.h>

int threadNum = 0;
int pieces = 0;
double* calcs = NULL;

const double UPPER_LIMIT = M_PI / 2;
const double LOWER_LIMIT = M_PI / 4;
const double DIFF = UPPER_LIMIT - LOWER_LIMIT;

void * doCacl( void * idAddr )
{
    int id = *(double*)idAddr;
    double value = 0;
    double dx = DIFF/pieces;

    int upper = id == (threadNum - 1) ? pieces : (id + 1) * (pieces / threadNum);

    for (ssize_t i = id * (pieces / threadNum); i < upper; ++i)
    {
        double x = LOWER_LIMIT + DIFF * i / pieces;
        value += dx * x/sin(x);
    }

    calcs[id] = value;

    return idAddr;
}

int main( int argc, char ** argv )
{
    if (argc != 3)
        exit (-1);

    threadNum = atoi (argv[1]);
    pieces = atoi (argv[2]);
    if (threadNum <= 0 || pieces <= 0 || threadNum >= pieces)
        exit (-1);

    pthread_t * threads = (pthread_t*) calloc (threadNum, sizeof (pthread_t));
    if (threads == NULL) exit (-1);
    calcs = (double*) calloc (threadNum, sizeof (double));
    if (calcs == NULL) exit (-1);

    for (ssize_t i = 0; i < threadNum; ++i)
    {
        calcs[i] = i;
        pthread_create (threads + i, NULL, &doCacl, calcs + i);
    }

    double value = 0;
    for (ssize_t i = 0; i < threadNum; ++i)
    {
        pthread_join (threads[i], NULL);
        value += calcs[i];
    }

    printf ("%lf" "\n", value);
}

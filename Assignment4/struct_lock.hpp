#include <semaphore.h>
 struct struct_lock
{
    int state ; //read=0;write =1;
    sem_t lock;
     struct_lock ()
    {
        sem_init(&lock,0,1);
        state=0;
    };
};
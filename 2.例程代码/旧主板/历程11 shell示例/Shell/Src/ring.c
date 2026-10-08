#include <stdio.h>
#include "ring.h"


int ring_init(ring * ringp, char * array_ptr, unsigned size)
{
    ringp->array = array_ptr;
    ringp->size = size;
    ringp->head = 0;
    ringp->tail = 0;
    return 0;
}

int ring_push(ring * ringp,char data)
{
    if(ring_is_full(ringp))
    {
        return -2;
    }
    ringp->array[ringp->tail] = data;
    ringp->tail = (ringp->tail + 1) % ringp->size ;
    return 0;
}


int ring_poll(ring * ringp,char * val)
{
   if(ring_is_empty(ringp))
    {
        return -3;
    }
    *val = ringp->array[ringp->head];
    ringp->head = (ringp->head + 1) % ringp->size ;
    return 0;
}



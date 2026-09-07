#ifndef __ring_H__
#define __ring_H__

#include "main.h"
typedef struct {  
    unsigned int head;   
    unsigned int tail;
    unsigned int size;
    char *array;   
}ring;    

#define ring_is_empty(q) ((q)->head == (q)->tail)
#define ring_is_full(q) ((((q)->tail+1)%(q)->size) == (q)->head )




int ring_init(ring * ringp, char * array_ptr, unsigned size);
int ring_push(ring * ringp,char data);
int ring_poll(ring * ringp,char * val);




#endif

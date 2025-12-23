#ifndef DRIVER_CONTAINER_RING_BUFFER_H_
#define DRIVER_CONTAINER_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct
{
    uint8_t *buffer;
    size_t buffer_size;

    size_t head;
    size_t tail;

    size_t element_size;
}ring_buffer_t;

extern void ring_buffer_init(ring_buffer_t * const rb, void *buffer, size_t buffer_size, size_t element_size);
extern bool ring_buffer_push(ring_buffer_t * const rb, void *value);
extern bool ring_buffer_pop(ring_buffer_t * const rb, void *value);
extern size_t ring_buffer_available_slot(ring_buffer_t * const rb);
extern size_t ring_buffer_use_slot(ring_buffer_t * const rb);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_CONTAINER_RING_BUFFER_H_ */

#ifndef MEK_OBSERVER_H_
#define MEK_OBSERVER_H_

#include <stdint.h>
#include <stddef.h>

#include "task.h"


typedef struct
{
    MEK_Task_t ** list_node;
    uint16_t list_size;
    uint8_t list_count;
} obs_sub_t;

extern void MEK_observerInit(obs_sub_t * const sub, MEK_Task_t * list[], uint16_t list_size);
extern bool MEK_observerAttach(obs_sub_t * const sub, MEK_Task_t * node);
extern bool MEK_observerDetach(obs_sub_t * const sub, MEK_Task_t const * const node);
extern void MEK_observerNotify(obs_sub_t * const sub, MEK_Evt_t const * const e);

#endif /* MEK_OBSERVER_H_ */

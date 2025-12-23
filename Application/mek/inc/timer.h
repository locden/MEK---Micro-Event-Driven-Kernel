#ifndef MEK_TIMER_H_
#define MEK_TIMER_H_

#include "sys_assert.h"
#include "message.h"
#include "task.h"

typedef struct MEK_TimeEvt_t MEK_TimeEvt_t;



struct MEK_TimeEvt_t{

	MEK_Evt_t super;

    MEK_Task_t * task;
    MEK_TimeEvt_t * next;

    uint32_t ctr;
    uint32_t interval;
};


void MEK_TimeEvt_ctor(struct MEK_TimeEvt_t * me, MSignal sig, MEK_Task_t * task);
void MEK_TimeEvt_arm(struct MEK_TimeEvt_t * me, uint32_t, uint32_t interval);
bool MEK_TimeEvt_disarm(struct MEK_TimeEvt_t * me);
void MEK_TimeEvt_tick();


#endif /* MEK_TIMER_H_ */

#include "timer.h"

static MEK_TimeEvt_t *timeEvt_head = (MEK_TimeEvt_t *)0;


void MEK_TimeEvt_ctor(struct MEK_TimeEvt_t * me, MSignal sig, MEK_Task_t * task)
{
    me->super.sig = sig;
    me->task = task;
    me->ctr = 0U;
    me->interval = 0U;

    me->next = timeEvt_head;
    timeEvt_head = me;
}
void MEK_TimeEvt_arm(struct MEK_TimeEvt_t * me, uint32_t ctr, uint32_t interval)
{
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    me->ctr = ctr;
    me->interval = interval;

//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
}
bool MEK_TimeEvt_disarm(struct MEK_TimeEvt_t * me)
{
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    bool status = (me->ctr != 0U);
    me->ctr = 0U;
    me->interval = 0U;

//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();

    return status;
}
void MEK_TimeEvt_tick()
{
    MEK_CRITICAL_SECTION_VAR;
    for(MEK_TimeEvt_t * t = timeEvt_head; t != (MEK_TimeEvt_t*)0; t = t->next )
    {
//        MEK_PORT_CRIT_ENTRY();
        MEK_CRITICAL_SECTION_START();

        if(t->ctr == 0U)
        {
//            MEK_PORT_CRIT_EXIT();
            MEK_CRITICAL_SECTION_END();
        }
        else if(t->ctr == 1U)
        {
            t->ctr = t->interval;

//            MEK_PORT_CRIT_EXIT();
            MEK_CRITICAL_SECTION_END();

            MEK_Task_post( t->task , &t->super);
        }
        else
        {
            --t->ctr;
//            MEK_PORT_CRIT_EXIT();
            MEK_CRITICAL_SECTION_END();
        }

//        MEK_PORT_CRIT_EXIT();
        MEK_CRITICAL_SECTION_END();
    }
}

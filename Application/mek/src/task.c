#include "sys_assert.h"
#include "task.h"
#include "app_bsp.h"
#include "SEGGER_RTT.h"

SYS_MODULE_NAME("MEK Task") /* for DBC assertions in this module */

MEK_Task_t * task_list[MEK_PORT_MAX_TASK];

static uint32_t task_readySetFlag = 0;

int MEK_Task_run(void)
{
//    MEK_PORT_INT_DISABLE();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    for( ;; )
    {
        if(task_readySetFlag != 0U)
        {
            uint32_t p = MEK_LOG2(task_readySetFlag);

            MEK_Task_t * const task = task_list[p];

//            MEK_PORT_INT_ENABLE();
            MEK_CRITICAL_SECTION_END();

            SEGGER_RTT_printf(0, "\n[ DISPATCH START ] ---------------------  \n");

            /* the task must have some events in the queue */
            SYS_ASSERT(100, task->nUsed > 0U);

            MEK_Evt_t const * e = task->qBuff[task->tail];

            if (task->tail == 0U) { /* need to wrap the tail? */
                task->tail = task->end; /* wrap around */
            }
            else {
                --task->tail;
            }

//            MEK_PORT_INT_DISABLE();
            MEK_CRITICAL_SECTION_START();

            if ((--task->nUsed) == 0U) {
                task_readySetFlag &= ~(1U << (p - 1U));
            }
            MEK_CRITICAL_SECTION_END();

            const uint32_t start = DWT->CYCCNT;

            MHsm_dispatcher(&task->super, e); /* virtual call */

            uint32_t cycles = DWT->CYCCNT - start;
            uint32_t us = cycles / (SystemCoreClock / 1000000U);
            SEGGER_RTT_printf(0, "[ %s ] TimeHandle: %lu us\n", task->name, us);

            MEK_CRITICAL_SECTION_START();
            /* free block if use block of memory pool to send a data */
            if(e->is_usedPool == true)
            {
                MEK_Evt_t * const temp_e = (MEK_Evt_t *)e;

                SYS_ASSERT(110, temp_e->refCtr != 0);

                // decrease event counter if event use Pool
                temp_e->refCtr--;

                if(temp_e->refCtr == 0)
                {
                    MEK_Event_clearPool( M_EVENT_CAST( temp_e ) );
                }
            }
            MEK_CRITICAL_SECTION_END();

            SEGGER_RTT_printf(0, "[ DISPATCH END   ] ---------------------  \n");
        }
        else
        {
            MEK_CRITICAL_SECTION_END();
            MEK_onIdleCond();
//            MEK_PORT_INT_DISABLE(); /* disable before looping back */
            MEK_CRITICAL_SECTION_START();
        }
    }

#ifdef __GNUC__ /* GNU compiler? */
    return 0;
#endif
}

void MEK_Task_ctor(MEK_Task_t * const me, MStateHandler initialize)
{
    MHsm_ctor( &me->super, initialize);
}
void MEK_Task_start(MEK_Task_t * const me,
                    uint8_t prio,
                    MEK_Evt_t const **qBuff, uint16_t qLen,
                    MEK_Evt_t const * const e,
                    const char *task_name)
{

    SYS_REQUIRE(200,
                (0U < prio) && (prio <= MEK_PORT_MAX_TASK)
                && (qBuff != (MEK_Evt_t const **)0) && (qLen > 0U)
                && (task_list[prio] == (MEK_Task_t *)0));

    me->prio = prio;
    me->name = task_name;

    me->qBuff = qBuff;
    me->end = qLen - 1;
    me->head = 0U;
    me->tail = 0U;
    me->nUsed = 0U;


    task_list[me->prio] = me;


    MHsm_init(&me->super, (MEK_Evt_t*)0);
}
void MEK_Task_post(MEK_Task_t * const me, MEK_Evt_t const * const e)
{
    SYS_REQUIRE(300, me->nUsed <= me->end);

//    MEK_PORT_CRIT_ENTRY();

    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    if(e->is_usedPool == true)
    {
        MEK_Evt_t * const e_temp = M_CAST(MEK_Evt_t *, e); // trick -> cast const event to non-const event

        // Increase counter for event
        e_temp->refCtr++;
    }

    me->qBuff[me->head] = e;

    if(me->head == 0)
    {
        me->head = me->end;
    }
    else
    {
        --me->head;
    }

    ++me->nUsed;
    task_readySetFlag |= (1U << (me->prio - 1));

    MEK_CRITICAL_SECTION_END();
//    MEK_PORT_CRIT_EXIT();
}


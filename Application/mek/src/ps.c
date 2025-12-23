#include "ps.h"
#include "sys_assert.h"

SYS_MODULE_NAME( "Publish subscribe" );

static MSignal * sub_list_event = NULL;
static uint16_t         sub_list_max = 0;

void MEK_psInit(MSignal * const sub_list, const uint16_t max_sig)
{
    SYS_REQUIRE(10, max_sig > M_USER_SIG);

    sub_list_event = sub_list;
    sub_list_max   = max_sig;

    memset(sub_list_event, 0 , sizeof(MSignal) * sub_list_max);
}

void MEK_subscribe(MEK_Task_t * const task_sub, MSignal sig)
{
    SYS_REQUIRE(100 , sig < sub_list_max && sig >= M_USER_SIG);
    SYS_REQUIRE(110 , task_sub != (MEK_Task_t * )0);

    uint8_t prio = task_sub->prio;

    SYS_ASSERT(120, prio != 0);

    sub_list_event[sig] |= (1U << prio);
}

void MEK_unsubscribe(MEK_Task_t * const task_sub, MSignal sig)
{
    SYS_REQUIRE(200 , sig < sub_list_max && sig >= M_USER_SIG);
    SYS_REQUIRE(210 , task_sub != (MEK_Task_t * )0);

    uint8_t prio = task_sub->prio;

    SYS_ASSERT(220, prio != 0);

    sub_list_event[sig] &= ~(1U << prio);
}

void MEK_publish(MEK_Evt_t const * const e)
{
    SYS_REQUIRE(300, e->sig < sub_list_max && e->sig >= M_USER_SIG);
    SYS_REQUIRE(310, e->refCtr == 0);

    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    uint32_t current_bit_set = sub_list_event[e->sig];
    uint8_t t = MEK_LOG2(sub_list_event[e->sig]);
    MEK_CRITICAL_SECTION_END();

    while(t != 0)
    {

        MEK_Task_post(task_list[t - 1], e);

        MEK_CRITICAL_SECTION_START();

        sub_list_event[e->sig] &=~ (1U << (t - 1));
        t = MEK_LOG2(sub_list_event[e->sig]);

        MEK_CRITICAL_SECTION_END();
    }

    MEK_CRITICAL_SECTION_START();
    sub_list_event[e->sig] = current_bit_set;
    MEK_CRITICAL_SECTION_END();
}


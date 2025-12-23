#include "observer.h"

#include "sys_assert.h"

void MEK_observerInit(obs_sub_t * const sub, MEK_Task_t * list[], uint16_t list_size)
{
    sub->list_node = list;
    sub->list_size = list_size;
    sub->list_count = 0U;

    /* Initialize NULL for list */
    for(uint16_t i = 0; i < sub->list_size; ++i)
    {
        sub->list_node[i] = (MEK_Task_t *)0;
    }
}
bool MEK_observerAttach(obs_sub_t * const sub, MEK_Task_t * node)
{
    if(sub == (obs_sub_t *)0 || node == (MEK_Task_t *)0) return false;

    if(sub->list_count > sub->list_size - 1)
    {
        return false;
    }

    sub->list_node[sub->list_count++] = node;

    return true;
}

bool MEK_observerDetach(obs_sub_t * const sub, MEK_Task_t const * const node)
{
    if(sub == (obs_sub_t *)0 || node == (MEK_Task_t *)0) return false;

    for(uint8_t i = 0; i <= sub->list_count; i++)
    {
        if(sub->list_node[i] == node)
        {
            for(uint8_t j = i; j <= sub->list_count; j++)
            {
                sub->list_node[j] = sub->list_node[j + 1];
            }
            --sub->list_count;
            return true;
        }
    }
    return false;
}

void MEK_observerNotify(obs_sub_t * const sub, MEK_Evt_t const * const e)
{
    for(uint8_t i = 0; i <= sub->list_count; i++)
    {
        if(sub->list_node)
        {
            MEK_Task_post( sub->list_node[i] , e );
        }
    }
}

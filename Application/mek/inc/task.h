#ifndef MEK_TASK_H_
#define MEK_TASK_H_

#include "mek_port.h"
#include <stdint.h>
#include <stdbool.h>

#include "message.h"
#include "hsm.h"


typedef struct
{
    MHsm super;
    uint8_t prio;
    const char *name;

    MEK_Evt_t const ** qBuff;
    uint8_t end;
    uint8_t head;
    uint8_t tail;
    uint8_t nUsed;
}MEK_Task_t;

extern MEK_Task_t * task_list[MEK_PORT_MAX_TASK];


void MEK_Task_ctor(MEK_Task_t * const me, MStateHandler initialize);
void MEK_Task_start(MEK_Task_t * const me,
                    uint8_t prio,
                    MEK_Evt_t const **qBuff, uint16_t qLen,
                    MEK_Evt_t const * const e,
                    const char *task_name);
void MEK_Task_post(MEK_Task_t * const me, MEK_Evt_t const * const e);
int MEK_Task_run(void);
#endif /* MEK_TASK_H_ */

#ifndef MEK_INC_PS_H_
#define MEK_INC_PS_H_

#include "task.h"
#include "message.h"



extern void MEK_psInit(MSignal * const sub_list, const uint16_t max_sig);
extern void MEK_subscribe(MEK_Task_t * const task_sub, MSignal sig);
extern void MEK_unsubscribe(MEK_Task_t * const task_sub, MSignal sig);
extern void MEK_publish(MEK_Evt_t const * const e);


#endif /* MEK_INC_PS_H_ */

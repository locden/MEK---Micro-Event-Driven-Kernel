#ifndef APP_BSP_H_
#define APP_BSP_H_

#include "app_main.h"
#include "main.h"

extern void BSP_delay_us(uint32_t ticks);
extern uint32_t BSP_tickCtr_us(void);
extern uint32_t BSP_tickCtr_ms(void);
extern void BSP_delay_ms(uint32_t ticks);



#endif /* APP_BSP_H_ */

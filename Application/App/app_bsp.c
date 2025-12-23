#include "app_bsp.h"
#include "sys_assert.h"
#include "message.h"
#include "task.h"

static uint32_t volatile l_tickCtr;


//............................................................................

//............................................................................

uint32_t BSP_tickCtr_us(void)
{
    uint32_t tickCtr;

    __disable_irq();
    tickCtr = l_tickCtr;
    __enable_irq();

    return tickCtr;
}
void BSP_delay_us(uint32_t ticks)
{
    uint32_t start = BSP_tickCtr_us();
    while ( ( BSP_tickCtr_us() - start ) < ticks ) {}
}
uint32_t BSP_tickCtr_ms(void)
{
    uint32_t tickCtr;

    __disable_irq();
    tickCtr = HAL_GetTick();
    __enable_irq();

    return tickCtr;
}
void BSP_delay_ms(uint32_t ticks)
{
    uint32_t start = BSP_tickCtr_ms();
    while ( ( BSP_tickCtr_ms() - start ) < ticks ) {}
}
void TIM7_IRQHandler(void)
{
    TIM7->SR &= ~TIM_SR_UIF;
    ++l_tickCtr;
}


//............................................................................

//............................................................................




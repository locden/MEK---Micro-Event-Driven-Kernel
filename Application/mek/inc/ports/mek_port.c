#include "mek_port.h"
#include "timer.h"

void MEK_onIdleCond()
{
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
//    MEK_PORT_INT_ENABLE(); /* NOTE: enable interrupts for MEK */
}

enum KernelUnaware
{
    MAX_KERNEL_UNAWARE_PRI,
};

enum KernelAware
{
    USART1_PRI = MEK_KERNEL_AWARE_IRQ,
    USART6_PRI = USART1_PRI,
    SYSTICK_PRI,
};

void MEK_onStart()
{
    // Set 5 bit full preemtive and 0 subprio
    NVIC_SetPriorityGrouping(0);


    SysTick_Config((SystemCoreClock / BSP_TICKS_PER_SEC) + 1U);

    /* set priorities of ISRs used in the system */
    NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRI);

    NVIC_SetPriority(USART1_IRQn, USART1_PRI);
    NVIC_SetPriority(USART6_IRQn, USART6_PRI);
}


void SysTick_Handler(void)
{
    MEK_TimeEvt_tick();
}


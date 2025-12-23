#include "sys_assert.h"
#include "app_main.h"



void SYS_fault_handler(char const * const module, int const label) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)label;

    /* set PRIMASK to disable interrupts and stop LTK right here */
    __asm volatile ("cpsid i");

#ifndef NDEBUG
    for (;;) { /* keep blinking LED2 */
//        uint32_t volatile ctr;
//        for (ctr = 10000U; ctr > 0U; --ctr)
//        {
//
//        }
//        for (ctr = 10000U; ctr > 0U; --ctr)
//        {
//
//        }
    }
#endif
    NVIC_SystemReset();
}

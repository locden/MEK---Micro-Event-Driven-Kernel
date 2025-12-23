#ifndef MEK_PORTS_MEK_PORT_H_
#define MEK_PORTS_MEK_PORT_H_

#include "app_main.h"

#define BSP_TICKS_PER_SEC                1000U // 1ms

#define MEK_PORT_TASK_ATTR               MEK_TaskPrio prio;

#define MEK_PORT_MAX_TASK                32U




#define MEK_MAX_KERNEL_UNAWARE_IRQ       2
#define MEK_KERNEL_AWARE_IRQ             3
#define MEK_BASEPRI                      (MEK_KERNEL_AWARE_IRQ << (8 - __NVIC_PRIO_BITS))

#define MEK_CRITICAL_SECTION_VAR         uint32_t basepri_
#define MEK_CRITICAL_SECTION_START()            \
        do                                      \
        {                                       \
            basepri_ = __get_BASEPRI();         \
            __set_BASEPRI(MEK_BASEPRI);         \
        } while(0)

#define MEK_CRITICAL_SECTION_END()              \
        do                                      \
        {                                       \
            __set_BASEPRI(basepri_);            \
        } while(0)


#define MEK_PORT_INT_DISABLE()           __asm volatile ("cpsid i")
#define MEK_PORT_INT_ENABLE()            __asm volatile ("cpsie i")

#define MEK_PORT_CRIT_ENTRY()            MEK_PORT_INT_DISABLE()
#define MEK_PORT_CRIT_EXIT()             MEK_PORT_INT_ENABLE()

typedef uint32_t MEK_ReadySet;

#define PRIVATE     static
#define PUBLIC
#define PROTECTED

/* special idle callback to handle the "idle condition" in MEK0 */
extern void MEK_onIdleCond(void);
extern void MEK_onStart();

#if (__ARM_ARCH == 6) /* ARMv6-M? */

/* MEK_LOG2() implementation for ARMv6-M (no CLZ instruction) */
static inline uint_fast8_t MEK_LOG2(uint32_t x) {
    static uint8_t const log2LUT[16] = {
        0U, 1U, 2U, 2U, 3U, 3U, 3U, 3U,
        4U, 4U, 4U, 4U, 4U, 4U, 4U, 4U
    };
    uint_fast8_t n = 0U;
    MEK_ReadySet tmp;

    #if (MEK_PORT_MAX_TASK > 16U)
    tmp = (MEK_ReadySet)(x >> 16U);
    if (tmp != 0U) {
        n += 16U;
        x = tmp;
    }
    #endif
    #if (MEK_PORT_MAX_TASK > 8U)
    tmp = (x >> 8U);
    if (tmp != 0U) {
        n += 8U;
        x = tmp;
    }
    #endif
    tmp = (x >> 4U);
    if (tmp != 0U) {
        n += 4U;
        x = tmp;
    }
    return n + log2LUT[x];
}

#else /* ARMv7-M+ have CLZ instruction for fast LOG2 computations */

#if defined __ARMCC_VERSION
    #define MEK_LOG2(x_) ((uint_fast8_t)(32U - __builtin_clz((unsigned)(x_))))
#elif defined __GNUC__
    #define MEK_LOG2(x_) ((uint_fast8_t)(32U - __builtin_clz((unsigned)(x_))))
#elif defined __ICCARM__
    #include <intrinsics.h>
    #define MEK_LOG2(x_) ((uint_fast8_t)(32U - __CLZ((unsigned long)(x_))))
#endif /* compiler type */

#endif

#endif /* MEK_PORTS_MEK_PORT_H_ */

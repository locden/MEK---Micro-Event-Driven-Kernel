#ifndef DRIVER_HAL_HAL_H_
#define DRIVER_HAL_HAL_H_

#include "stdint.h"
#include "stdarg.h"
#include "stdbool.h"
#include "stdio.h"

#include "gpio.h"
#include "usart.h"
#include "spi.h"

#include "task.h"
#include "timer.h"

#if (__ARM_ARCH == 6) /* ARMv6-M? */

/* HAL_LOG2() implementation for ARMv6-M (no CLZ instruction) */
static inline uint_fast8_t HAL_LOG2(uint32_t x) {
    static uint8_t const log2LUT[16] = {
        0U, 1U, 2U, 2U, 3U, 3U, 3U, 3U,
        4U, 4U, 4U, 4U, 4U, 4U, 4U, 4U
    };
    uint_fast8_t n = 0U;
    HAL_ReadySet tmp;

    #if (HAL_PORT_MAX_TASK > 16U)
    tmp = (HAL_ReadySet)(x >> 16U);
    if (tmp != 0U) {
        n += 16U;
        x = tmp;
    }
    #endif
    #if (HAL_PORT_MAX_TASK > 8U)
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
    #define HAL_LOG2(x_) ((uint_fast8_t)(32U - __builtin_clz((unsigned)(x_))))
#elif defined __GNUC__
    #define HAL_LOG2(x_) ((uint_fast8_t)(32U - __builtin_clz((unsigned)(x_))))
#elif defined __ICCARM__
    #include <intrinsics.h>
    #define HAL_LOG2(x_) ((uint_fast8_t)(32U - __CLZ((unsigned long)(x_))))
#endif /* compiler type */
#endif


#define HAL_ENUM(type, name) type name; enum




#endif /* DRIVER_HAL_HAL_H_ */

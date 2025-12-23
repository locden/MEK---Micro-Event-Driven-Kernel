#ifndef DRIVER_HAL_HAL_GPIO_H_
#define DRIVER_HAL_HAL_GPIO_H_

#include "gpio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

extern uint8_t hal_gpio_register_trigger_callback(uint16_t pin_number, void (*TriggerCallback)(void));

#endif /* DRIVER_HAL_HAL_GPIO_H_ */

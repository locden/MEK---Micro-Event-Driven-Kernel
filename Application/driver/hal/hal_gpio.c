#include "../hal/hal_gpio.h"

#if defined __GNUC__
    #define EXTI_LOG2(x_) ((uint8_t)(32U - __builtin_clz((unsigned)(x_))) - 1U)
#endif

typedef struct
{
    uint16_t pin_number;
    void (*TriggerCallback)(void);
    bool isUsed;
} Exti_type;

static Exti_type exti_handle[16] = { 0 };

uint8_t hal_gpio_register_trigger_callback(uint16_t pin_number, void (*TriggerCallback)(void))
{
    uint8_t pin = EXTI_LOG2(pin_number);
    if(pin > 15) return 0;

    if(exti_handle[pin].isUsed == false)
    {
        exti_handle[pin].isUsed = true;
        exti_handle[pin].pin_number = pin_number;
        exti_handle[pin].TriggerCallback = TriggerCallback;

        return 1;
    }
    return 0;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint8_t pin = EXTI_LOG2(GPIO_Pin);

    if(exti_handle[pin].isUsed)
    {
        if(exti_handle[pin].TriggerCallback)
        {
            (*exti_handle[pin].TriggerCallback)();
        }
    }
}


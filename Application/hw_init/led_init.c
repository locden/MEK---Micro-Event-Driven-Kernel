#include "led_init.h"

void LedRedInit() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
     /*Configure GPIO pins : PBPin PBPin */
    GPIO_InitStruct.Pin = LED_RED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
void LedRedOff()
{
    GPIOB->ODR &=~ (1U << 14);
}
void LedRedOn()
{
    GPIOB->ODR |=  (1U << 14);
}
void LedRedToggle()
{
    GPIOB->ODR ^= (1U << 14);
}

void LedGreenInit() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
     /*Configure GPIO pins : PBPin PBPin */
    GPIO_InitStruct.Pin = LED_GREEN_Pin|LED_RED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
void LedGreenOff()
{
    GPIOB->ODR &=~ (1U << 0);
}
void LedGreenOn()
{
    GPIOB->ODR |=  (1U << 0);
}
void LedGreenToggle()
{
    GPIOB->ODR ^= (1U << 0);
}

void LedYellowInit() {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
      /*Configure GPIO pin : PtPin */
      GPIO_InitStruct.Pin = LED_YELLOW_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(LED_YELLOW_GPIO_Port, &GPIO_InitStruct);
}
void LedYellowOff()
{
    GPIOE->ODR &=~ (1U << 1);
}
void LedYellowOn()
{
    GPIOE->ODR |=  (1U << 1);
}
void LedYellowToggle()
{
    GPIOE->ODR ^= (1U << 1);
}


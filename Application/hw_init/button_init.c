#include <button_init.h>

void btn_startPause_init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin  = SW_BOARD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SW_BOARD_GPIO_Port, &GPIO_InitStruct);
}
uint8_t btn_startPause_read() {
    return HAL_GPIO_ReadPin(SW_BOARD_GPIO_Port , SW_BOARD_Pin);
}




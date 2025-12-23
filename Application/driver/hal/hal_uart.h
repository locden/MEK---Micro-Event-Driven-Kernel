#ifndef DRIVER_HAL_HAL_UART_H_
#define DRIVER_HAL_HAL_UART_H_

#include "ring_buffer.h"

#include "hal.h"

typedef uint32_t UART_Baudrate;
typedef uint8_t  UART_Buffer;
typedef uint16_t UART_BufferLen;
typedef void (* uartReceiveCharacterCallBack)(void * , uint8_t );


typedef enum
{
    UART_OK              = 0x00,
    UART_ERROR_HAL       = 0x01,
    UART_BUSY            = 0x02,
    UART_ERROR_TIMEOUT   = 0x03,
    UART_ERROR_PARAM,
    UART_ERROR_EXIST,
    UART_ERROR_NOT_EXIST,
} UART_ReturnType_t;


typedef enum
{
    UART_ID_1 = 0x00,
    UART_ID_2 = 0x01,
    UART_ID_3 = 0x02,
    UART_ID_4 = 0x03,
    UART_ID_5 = 0x04,
    UART_ID_6 = 0x05,
    UART_MAX_ID,
} UART_Id_t;


typedef struct
{
    uint32_t baudrate;
    uint32_t wordLength;
    uint32_t stopBit;
    uint32_t parity;
    uint32_t mode;
    uint32_t hwFlowControl;
    uint32_t overSampling;
} UART_Config_t;



#define UART_Init(__id , ...)                                                       \
            UART_Init_( (__id), (UART_Config_t){ .baudrate      = 115200U,             \
                                                 .wordLength    = UART_WORDLENGTH_8B,  \
                                                 .stopBit       = UART_STOPBITS_1,     \
                                                 .parity        = UART_PARITY_NONE,    \
                                                 .mode          = UART_MODE_TX_RX,     \
                                                 .hwFlowControl = UART_HWCONTROL_NONE, \
                                                 .overSampling  = UART_OVERSAMPLING_16,\
                                                 __VA_ARGS__ } )



static inline uint32_t UART_GetPCLK(USART_TypeDef *uart)
{
#if defined(STM32F4) || defined(STM32F7)
    if (uart == USART1 || uart == USART6)
    {
        return HAL_RCC_GetPCLK2Freq();   // APB2
    }
    else
    {
        return HAL_RCC_GetPCLK1Freq();   // APB1
    }
#endif
}

extern UART_ReturnType_t UART_Init_(UART_Id_t id, UART_Config_t config);
extern UART_ReturnType_t UART_Open(UART_Id_t id);
extern UART_ReturnType_t UART_Close(UART_Id_t id);
extern UART_ReturnType_t UART_RegisterReceiveCallback(UART_Id_t id, uartReceiveCharacterCallBack cb, void * handlerCallback);
extern UART_ReturnType_t UART_WriteAsyn(UART_Id_t id, uint8_t * pData, uint32_t length);
extern UART_ReturnType_t UART_Write(UART_Id_t id, uint8_t * pData, uint32_t length);

#endif /* DRIVER_HAL_HAL_UART_H_ */

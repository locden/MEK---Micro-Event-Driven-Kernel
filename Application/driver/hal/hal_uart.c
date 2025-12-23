#include "../hal/hal_uart.h"

typedef struct
{

    UART_HandleTypeDef          pUart;

    UART_Baudrate               baudrate;

    bool                        isUsed;

    ring_buffer_t               rx_rb;
    uint8_t                     rx_data;
    uint8_t                     rx_buffer[1024];

    ring_buffer_t               tx_rb;
    uint8_t                     tx_data;
    uint8_t                     tx_buffer[20];

    uartReceiveCharacterCallBack cb;
    void * handlerCallback;
} UART_UserHandle_t;


static UART_UserHandle_t uart_handle[UART_MAX_ID] = { 0 };
static USART_TypeDef * uart_instance[UART_MAX_ID] = { USART1 , USART2 , USART3 , UART4 , UART5 , USART6 };
static IRQn_Type uart_irq[UART_MAX_ID] = { USART1_IRQn , USART2_IRQn , USART3_IRQn , UART4_IRQn , UART5_IRQn , USART6_IRQn };

UART_ReturnType_t UART_Init_(UART_Id_t id, UART_Config_t config)
{
    if(uart_handle[id].isUsed == true)
    {
        return UART_ERROR_EXIST;
    }

    UART_UserHandle_t *handle = &uart_handle[id];

    handle->isUsed = true;
    handle->handlerCallback = NULL;

    HAL_UART_DeInit( &handle->pUart );
    handle->pUart.Instance         = uart_instance[id];
    handle->pUart.Init.BaudRate     = config.baudrate;
    handle->pUart.Init.WordLength   = config.wordLength;
    handle->pUart.Init.StopBits     = config.stopBit;
    handle->pUart.Init.Parity       = config.parity;
    handle->pUart.Init.Mode         = config.mode;
    handle->pUart.Init.HwFlowCtl    = config.hwFlowControl;
    handle->pUart.Init.OverSampling = config.overSampling;
    HAL_UART_Init( &handle->pUart );



    ring_buffer_init(&handle->rx_rb,
                     handle->rx_buffer,
                     1024,
                     sizeof(uint8_t));

    ring_buffer_init(&handle->tx_rb,
                     handle->tx_buffer,
                     20,
                     sizeof(uint8_t));

    __HAL_UART_ENABLE_IT(&handle->pUart, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&handle->pUart, UART_IT_TXE);
    return UART_OK;
}

UART_ReturnType_t UART_Open(UART_Id_t id)
{
    UART_UserHandle_t *handle = &uart_handle[id];
    if(handle->isUsed == false) return UART_ERROR_NOT_EXIST;

    HAL_UART_Receive_IT(&handle->pUart, &handle->rx_data, 1);

    return UART_OK;
}

UART_ReturnType_t UART_Close(UART_Id_t id)
{
    UART_UserHandle_t *handle = &uart_handle[id];

    HAL_UART_DeInit(&handle->pUart);

    return UART_OK;
}

UART_ReturnType_t UART_RegisterReceiveCallback(UART_Id_t id, uartReceiveCharacterCallBack cb, void * handlerCallback)
{
    UART_UserHandle_t *handle = &uart_handle[id];

    if(handle->cb == NULL)
    {
        handle->cb                = cb;
        handle->handlerCallback   = handlerCallback;
        return UART_OK;
    }

    return UART_ERROR_HAL;
}

static void UART_TxStart(UART_Id_t id)
{
    UART_UserHandle_t *handle = &uart_handle[id];

    if(! __HAL_UART_GET_IT_SOURCE(&handle->pUart, UART_IT_TXE))
    {
        /* Interrupt disabled, kick start it */
        uint8_t c;
        ring_buffer_pop(&handle->tx_rb, &c);
        handle->pUart.Instance->DR = c;
        __HAL_UART_ENABLE_IT(&handle->pUart, UART_IT_TXE);
    }
}

UART_ReturnType_t UART_WriteAsyn(UART_Id_t id, uint8_t * pData, uint32_t length)
{
    UART_UserHandle_t *handle = &uart_handle[id];

    for(uint8_t i = 0 ; i < length; ++i)
    {
        ring_buffer_push(&handle->tx_rb, pData + i);
    }
    UART_TxStart(id);
    return UART_OK;
}

UART_ReturnType_t UART_Write(UART_Id_t id, uint8_t * pData, uint32_t length)
{
    UART_UserHandle_t *handle = &uart_handle[id];
    UART_ReturnType_t ret = HAL_UART_Transmit(&handle->pUart, pData, length, HAL_MAX_DELAY);
    return ret;
}

void UART_IRQHandler( UART_Id_t id )
{
    UART_UserHandle_t *handle = &uart_handle[id];

    /* RECEIVE HANDLING */
    if( __HAL_UART_GET_FLAG( &handle->pUart, UART_FLAG_RXNE ) )
    {
        uint8_t c = handle->pUart.Instance->DR;

        if( handle->cb )
        {
            (*handle->cb)(handle->handlerCallback , c );
        }
        else
        {
            ring_buffer_push(&handle->rx_rb, &c);
        }
    }

//    /* TRANSMIT HANDLING */
//    if(__HAL_UART_GET_FLAG(&handle->pUart, UART_FLAG_TXE))
//    {
//        if(ring_buffer_available_slot(&handle->tx_rb) > 0)
//        {
//            /* Transmit next available character */
//            uint8_t c;
//            ring_buffer_pop(&handle->tx_rb, &c);
//            handle->pUart.Instance->DR = c;
//        }
//        else
//        {
//            /* Stop transmit interrupts */
//            __HAL_UART_DISABLE_IT(&handle->pUart, UART_IT_TXE);
//        }
//    }
}

//void USART1_IRQHandler(void)
//{
//    UART_IRQHandler(UART_ID_1);
//}

void USART2_IRQHandler(void)
{
    UART_IRQHandler(UART_ID_2);
}

void USART3_IRQHandler(void)
{
    UART_IRQHandler(UART_ID_3);
}

void UART4_IRQHandler(void)
{
    UART_IRQHandler(UART_ID_4);
}

void UART5_IRQHandler(void)
{
    UART_IRQHandler(UART_ID_5);
}

void USART6_IRQHandler(void)
{
    UART_IRQHandler(UART_ID_6);
}








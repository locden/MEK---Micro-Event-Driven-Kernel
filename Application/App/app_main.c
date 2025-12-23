#include <app_bsp.h>
#include <app_main.h>
#include "task.h"
#include "message.h"
#include "observer.h"
#include "ps.h"


#include "timeBomb.h"


int main ()
{
    /* Init System and peripheral */
    System_init();

    /* Init TimeStamp */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* Init Segger console */
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer( 0 , NULL , NULL , 0 , SEGGER_RTT_MODE_NO_BLOCK_SKIP );

    /* Init event Pool */
    MEK_Event_PoolInit();

    /* Init Button */
    button_init(&btn_startPause,
    			10U ,
				BUTTON_HW_PRESSED_HIGH ,
				btn_startPause_init,
				btn_startPause_read,
				btn_startPause_callback);

    button_enable(&btn_startPause);

    /* Init led */
    led_init(&led_red,    LedRedInit,    LedRedOn,    LedRedOff);   
    led_init(&led_green,  LedGreenInit,  LedGreenOn,  LedGreenOff);
    led_init(&led_yellow, LedYellowInit, LedYellowOn, LedYellowOff);

 
    /* Init time boom AO */
    TimeBome_instance();
    static MEK_Evt_t const * tbQSto[10];        /* Queue event store */
    MEK_Task_start(AO_timeBomb,                 /* Active object */
    		       2,                           /* Priority */
				   tbQSto,                      /* Queue event store */
				   M_ARRAY_LEN(tbQSto),         /* Queue event size */
				   (void*)0);

    /* Start Hook */
    MEK_onStart();                              
    return MEK_Task_run();
}


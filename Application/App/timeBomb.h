#ifndef EXAMPLE_TIMEBOMB_H_
#define EXAMPLE_TIMEBOMB_H_

#include "task.h"
#include "timer.h"
#include "app_main.h"

enum EventSignal {

	BUTTON1_PRESSED_SIG = M_USER_SIG ,
	BUTTON1_RELEASED_SIG ,
	BUTTON2_PRESSED_SIG,
	BUTTON2_RELEASED_SIG ,
	TIMEOUT_SIG,

	MAX_SIG
};

typedef struct TimeBomb_t TimeBomb_t;


struct TimeBomb_t{
	MEK_Task_t super;	/* inherit Active_t super class */

	/* Attribute */
	MEK_TimeEvt_t te;
	uint32_t blink_ctr;
};


extern MEK_Task_t * const AO_timeBomb;
extern void TimeBome_instance(void);

#endif /* EXAMPLE_TIMEBOMB_H_ */

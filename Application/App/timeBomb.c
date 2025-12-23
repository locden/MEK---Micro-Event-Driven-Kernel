#include <TimeBomb/timeBomb.h>


static void TimeBomb_ctor( TimeBomb_t * const me );
static MState TimeBomb_initial(TimeBomb_t * const me, MEK_Evt_t const * const e);
static MState TimeBomb_armed(TimeBomb_t * const me, MEK_Evt_t const * const e);
static MState Timebomb_wait4button(TimeBomb_t * const me, MEK_Evt_t const * const e);
static MState TimeBomb_blink(TimeBomb_t * const me, MEK_Evt_t const * const e);
static MState TimeBomb_pause(TimeBomb_t * const me, MEK_Evt_t const * const e);
static MState TimeBomb_boom(TimeBomb_t * const me, MEK_Evt_t const * const e);
static MState TimeBomb_defused(TimeBomb_t * const me, MEK_Evt_t const * const e);


static TimeBomb_t timeBomb;

MEK_Task_t * const AO_timeBomb = &timeBomb.super;

static MState TimeBomb_initial(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	(void)e;
	return M_TRAN(&Timebomb_wait4button);
}


static MState TimeBomb_armed(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	MState status;
    switch (e->sig) {
        case M_EXIT_SIG: {
			led_off(&led_red);
			led_off(&led_green);
			led_off(&led_yellow);
            status = M_HANDLED();
            break;
        }
        case M_INIT_SIG: {
            status = M_TRAN(Timebomb_wait4button);
            break;
        }
        case BUTTON2_PRESSED_SIG: {
        	LOG_PRINTF("%d", ((testEvent *)e)->foo);
            status = M_TRAN(TimeBomb_defused);
            break;
        }
        default: {
            status = M_SUPER(MHsm_top);
            break;
        }
    }
    return status;
}


static MState Timebomb_wait4button(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	MState status;
	switch( e->sig ) {

		case (M_ENTRY_SIG) : {
			led_on(&led_green);
			status = M_HANDLED();
			break;
		}
		case (M_EXIT_SIG) : {
			led_off(&led_green);
			status = M_HANDLED();
			break;
		}
		case (BUTTON1_PRESSED_SIG) : {
			me->blink_ctr = 5U;
			status = M_TRAN(&TimeBomb_blink);
			break;
		}
		default: {
			status = M_SUPER(&TimeBomb_armed);
			break;
		}

	}

	return status;
}
static MState TimeBomb_blink(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	MState status;
	switch( e->sig ) {

		case (M_ENTRY_SIG) : {
			led_on(&led_red);
			MEK_TimeEvt_arm(&me->te, 500U, 0U);			/* Arm timer 500ms */
			status = M_HANDLED();
			break;
		}
		case (M_EXIT_SIG) : {
			led_off(&led_red);
			status = M_HANDLED();
			break;
		}
		case (TIMEOUT_SIG) : {
			status = M_TRAN(TimeBomb_pause);
			break;
		}
		default: {
			status = M_SUPER(&TimeBomb_armed);
			break;
		}
	}
	return status;
}
static MState TimeBomb_pause(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	MState status;
	switch( e->sig ) {

		case (M_ENTRY_SIG) : {
			MEK_TimeEvt_arm(&me->te, 500U, 0U);			/* Arm timer 500ms */
			status = M_HANDLED();
			break;
		}
		case (M_EXIT_SIG) : {
			status = M_HANDLED();
			break;
		}

		case (TIMEOUT_SIG) : {
			me->blink_ctr--;

			if(me->blink_ctr > 0U) {
				status = M_TRAN(TimeBomb_blink);

			}
			else {
				status = M_TRAN(TimeBomb_boom);
			}
			break;
		}
		default: {
			status = M_SUPER(&TimeBomb_armed);
			break;
		}
	}
	return status;
}
static MState TimeBomb_boom(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	MState status;
	switch( e->sig ) {

		case (M_ENTRY_SIG) : {
			led_on(&led_red);
			led_on(&led_green);
			led_on(&led_yellow);

		    status = M_HANDLED();
			break;
		}
		case (M_EXIT_SIG) : {
			led_off(&led_red);
			led_off(&led_green);
			led_off(&led_yellow);
			status = M_HANDLED();
			break;
		}
		default: {
			status = M_SUPER(&TimeBomb_armed);
			break;
		}
	}
	return status;
}

static MState TimeBomb_defused(TimeBomb_t * const me, MEK_Evt_t const * const e) {
	MState status;
    switch (e->sig) {
        case M_ENTRY_SIG: {
        	led_off(&led_yellow);
            status = M_HANDLED();
            break;
        }
        case BUTTON2_PRESSED_SIG: {

            status = M_TRAN(TimeBomb_armed);
            break;
        }
        default: {
            status = M_SUPER(MHsm_top);
            break;
        }
    }
    return status;
}




void TimeBomb_ctor( TimeBomb_t * const me ) {

	MEK_Task_ctor(&me->super, (MStateHandler)&TimeBomb_initial);
	MEK_TimeEvt_ctor(&me->te, TIMEOUT_SIG, &me->super);
}

void TimeBome_instance(void)
{
	TimeBomb_ctor(&timeBomb);
}

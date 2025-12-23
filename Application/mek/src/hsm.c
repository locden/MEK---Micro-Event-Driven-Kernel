#include "hsm.h"
#include "message.h"


static const MEK_Evt_t MEK_reservedEvt[] =
{
        { (MSignal)MEK_EMPTY_SIG } ,
        { (MSignal)M_ENTRY_SIG   } ,
        { (MSignal)M_EXIT_SIG    } ,
        { (MSignal)M_INIT_SIG    } ,
};

MState MHsm_top(MHsm * const me , MEK_Evt_t const * const e)
{
    M_UNUSED(me);
    M_UNUSED(e);
    return M_IGNORED();
}

void MHsm_init(MHsm * const me , MEK_Evt_t const * const e)
{
    MStateHandler t;

    /* Ensure the initialize has been taken */
    assert((*(me->state))(me , e) == M_RET_TRAN);

    /* holds the source state of the transition. The first source is the top state */
    t = (MStateHandler)&MHsm_top;

    do
    {
        /* The temporary array stores the pointer to handler function in the exit path
         * from the target state of the initialize transition */
        MStateHandler entryPath[MHSM_MAX_NESTED_STATE];

        /* Index for entry Path */
        int8_t ei = 0;

        /* The current state (current leaf state) */
        entryPath[0] = me->state;

        /*
         * Because entryPath saved the current state(leaf) so we need g to trigger Empty event
         * to bubble up to super state
         */
        MEK_TRIG_(me->state , MEK_EMPTY_SIG);

        /*
         * loop
         *  */
        while(me->state != t)
        {
            entryPath[++ei] = me->state;
            MEK_TRIG_(me->state , MEK_EMPTY_SIG);
        }

        /* restore the target of the initial tran. */
        me->state = entryPath[0];

        assert(ei < MHSM_MAX_NESTED_STATE);

        do
        {
            MEK_ENTRY_(entryPath[ei]);
        } while((--ei) >= 0);

        t = entryPath[0];
    } while(MEK_TRIG_(t, M_INIT_SIG) == M_RET_TRAN);

    /*
     * After finish loop t(source state) = leap state
     * Back a pointer me->state to right position (leaf state)
     * => by UML -> state always in leaf state;
     *
     * S            ->top most
     *      |_
     *      |    S1        ->leaf state
     *      |_
     *          S2
     *           |_
     *             S21  ->leaf state
     *  */
    me->state = t;
}
void MHsm_dispatcher(MHsm * const me , MEK_Evt_t const * const e)
{
    MStateHandler entryPath[MHSM_MAX_NESTED_STATE];
    MStateHandler s;
    MStateHandler t;
    MState r;

    /* Save the target state */
    t = me->state;

    do
    {
        /* Save the source state */
        s = me->state;

        /* Execute event */
        r = (*s)(me, e);

        /*  */
        if (r == M_RET_UNHANDLED) {
            (void) MEK_TRIG_(s, MEK_EMPTY_SIG);
            r = M_RET_SUPER;
        }
    } while(r == M_RET_SUPER);

    if( r == M_RET_TRAN )
    {

        int8_t ip = -1;
        int8_t iq;

        /* Save the target state */
        entryPath[0] = me->state;
        entryPath[1] = t;

        /* do loop from leaf state to top which handle the event */
        while(t != s)
        {
            if( MEK_TRIG_( t , M_EXIT_SIG ) == M_RET_HANDLED )
            {
                MEK_TRIG_( t , MEK_EMPTY_SIG );
            }
            t = me->state;
        }

        /* target of the transition */
        t = entryPath[0];

        /* source = target => self transition */
        if( s == t )
        {
            /* Exit source */
            MEK_EXIT_(s);

            /* Entry target */
            ip = 0;
        }
        else
        {
            MEK_TRIG_(t, MEK_EMPTY_SIG);
            t = me->state;
            if( s == t) {
                ip = 0;
            }
            else {
                MEK_TRIG_(s, MEK_EMPTY_SIG);
                if(me->state == t) {
                    MEK_EXIT_(s);
                    ip = 0;
                }
                else {
                    if(me->state == entryPath[0]) {
                        MEK_EXIT_(s);
                    }
                    else {
                        iq = 0;
                        ip = 1;
                        entryPath[1] = t;
                        t = me->state;

                        r = MEK_TRIG_(entryPath[1], MEK_EMPTY_SIG);
                        while(r == M_RET_SUPER)
                        {
                            entryPath[++ip] = me->state;
                            if(me->state == s) {
                                iq = 1;

                                assert(ip < MHSM_MAX_NESTED_STATE);
                                --ip;
                                r = M_RET_HANDLED;
                            }
                            else {
                                r = MEK_TRIG_(me->state, MEK_EMPTY_SIG);
                            }
                        }
                        if(iq == 0) {
                            assert( ip < MHSM_MAX_NESTED_STATE);

                            MEK_EXIT_(s);

                            iq = ip;
                            r = M_RET_IGNORED;
                            do {
                                if(t == entryPath[iq]) {
                                    r = M_RET_HANDLED;
                                    ip = iq - 1;
                                    iq = -1;
                                }
                                else {
                                    --iq;
                                }
                            } while(iq >= 0);

                            if(r != M_RET_HANDLED) {
                                r = M_RET_IGNORED;
                                do {
                                    if(MEK_TRIG_(t, M_EXIT_SIG) == M_RET_HANDLED) {
                                        MEK_TRIG_(t, MEK_EMPTY_SIG);
                                    }
                                    t = me->state;
                                    iq = ip;
                                    do {
                                        if(t == entryPath[iq]) {
                                            ip = iq - 1;
                                            iq = -1;
                                            r = M_RET_HANDLED;
                                        }
                                        else {
                                            --iq;
                                        }
                                    } while(iq >= 0);
                                } while(r != M_RET_HANDLED);
                            }
                        }
                    }
                }
            }
        }

        for(; ip >= 0; --ip) {
            MEK_ENTRY_(entryPath[ip]);
        }
        t = entryPath[0];
        me->state = t;

        while(MEK_TRIG_(t, M_INIT_SIG) == M_RET_TRAN)
        {
            ip = 0;
            entryPath[0] = me->state;
            MEK_TRIG_(me->state, MEK_EMPTY_SIG);
            while(me->state != t) {
                entryPath[++ip] = me->state;
                MEK_TRIG_(me->state, MEK_EMPTY_SIG);
            }
            me->state = entryPath[0];

            assert(ip < MHSM_MAX_NESTED_STATE);

            do {
                MEK_ENTRY_(entryPath[ip]);
            }while ((--ip) >= (int8_t)0);

            t = entryPath[0];
        }
    }
    me->state = t;
}

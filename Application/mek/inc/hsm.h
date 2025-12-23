#ifndef MEK_HSM_H_
#define MEK_HSM_H_

#include "stdint.h"
#include <assert.h>
#include "message.h"


typedef uint8_t MState;
typedef struct MHsm MHsm;

typedef MState (*MStateHandler)( struct MHsm * const , MEK_Evt_t const * const );

struct MHsm
{
    MStateHandler state;
};

#define MHsm_ctor(me_ , initialize_) ((me_)->state = (initialize_))

extern void MHsm_init(MHsm * const me , MEK_Evt_t const * const e);
extern void MHsm_dispatcher(MHsm * const me , MEK_Evt_t const * const e);
extern MState MHsm_top(MHsm * const me , MEK_Evt_t const * const e);

#define MEK_TRIG_(state_ , sig_) \
            ((*(state_))(me, &MEK_reservedEvt[sig_]))

#define MEK_ENTRY_(state_) \
            MEK_TRIG_(state_, M_ENTRY_SIG)

#define MEK_EXIT_(state_) \
            MEK_TRIG_(state_, M_EXIT_SIG)

#define M_TRAN(target_)    \
            (((MHsm *)me)->state = (MStateHandler)(target_) , M_RET_TRAN)

#define M_SUPER(super_) \
            (((MHsm *)me)->state = (MStateHandler)(super_) , M_RET_SUPER)



#define M_RET_SUPER          ((MState)0U)
#define M_RET_UNHANDLED      ((MState)1U)

#define M_RET_HANDLED        ((MState)2U)
#define M_RET_IGNORED		 ((MState)3U)

#define M_RET_ENTRY     	 ((MState)4U)
#define M_RET_EXIT      	 ((MState)5U)
#define M_RET_INIT 	 		 ((MState)6U)

#define M_RET_TRAN           ((MState)7U)


#define M_IGNORED()           M_RET_IGNORED
#define M_HANDLED()           M_RET_HANDLED
#define M_INIT()              M_RET_INIT
#define Q_UNHANDLED()         M_RET_UNHANDLED

#endif /* MEK_HSM_H_ */

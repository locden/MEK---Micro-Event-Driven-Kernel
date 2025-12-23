#ifndef MEK_MESSAGE_H_
#define MEK_MESSAGE_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>

#define POOL_NUM_BLOCKS                100  /* Number of memory blocks */
#define POOL_BLOCK_SIZE                64  /* Size of each memory block in bytes */

#define ALIGNMENT                       4   /* Align memory blocks to 4-byte boundaries */


#define MHSM_MAX_NESTED_STATE           8

typedef uint16_t MSignal;

#define MEK_EMPTY_SIG                   0

enum ReservedSignals {
	M_ENTRY_SIG = 1,       /* for triggering the entry action in a state */
	M_EXIT_SIG  = 2,       /* for triggering the exit action from a state */
	M_INIT_SIG  = 3,       /* dispatched to AO before entering event-loop */
	M_USER_SIG  = 4,       /* first signal available to the users */
};

typedef struct {

    MSignal sig;            /* Signal of event */

    uint16_t refCtr;        /* Mask for Pool */
    bool is_usedPool;
} MEK_Evt_t;


/**
 * @brief Initialize the global event pool.
 *
 * This function must be called once during system startup
 * before any event allocations are performed.
 */
extern void MEK_Event_PoolInit();

/**
 * @brief Allocate a new event from the event pool.
 *
 * @param[in] sig The signal (MSignal) to associate with the new event.
 *
 * @return Pointer to the allocated event object,
 *         or NULL if no free event blocks are available.
 */
extern void * MEK_Event_Allocate(MSignal sig);


/**
 * @brief Return an event back to the pool.
 *
 * This function releases the memory of an event previously
 * allocated by MEK_Event_Allocate(), making it available
 * for future reuse.
 *
 * @param[in] e Pointer to the event to be released.
 */
extern void MEK_Event_clearPool(MEK_Evt_t * const e);

/**
 * @brief Get the peak number of event blocks used.
 *
 * Useful for profiling and tuning the pool size.
 *
 * @return Maximum number of event blocks that were simultaneously in use.
 */
extern uint32_t MEK_Event_getMaxPoolWasUsed();


/**
 * @brief Get the number of free event blocks available.
 * @return Number of free blocks currently in the pool.
 */
extern uint32_t MEK_Event_getFreeBlockCount();



#define M_UNUSED(x_)               ((void)x_)
#define M_ARRAY_LEN(x_)            ((sizeof(x_)) / (sizeof(x_[0])))
#define M_CAST(TYPE, TARGET)       ((TYPE)(TARGET))
#define M_EVENT_CAST(e_)           (M_CAST(MEK_Evt_t *, (e_)))
#define M_EVENT_INIT(sig_)           \
{                                    \
    .sig         = sig_,             \
    .refCtr      = 0,                \
    .is_usedPool = false             \
}

#endif /* MEK_MESSAGE_H_ */

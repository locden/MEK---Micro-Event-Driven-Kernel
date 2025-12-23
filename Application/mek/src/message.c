#include "message.h"
#include "sys_assert.h"
#include "mek_port.h"
SYS_MODULE_NAME( "Message" );

typedef struct memBlock
{
    struct memBlock * next;
} mem_block_t;

typedef struct {

    alignas(max_align_t) uint8_t pool[POOL_NUM_BLOCKS][POOL_BLOCK_SIZE];
    mem_block_t * free_list;
    uint16_t nFree;
    uint16_t nMax;
} memory_pool_t;

/*********************** STATIC VARIALBLE *****************************/
static memory_pool_t memPool;


_Static_assert(POOL_BLOCK_SIZE >= sizeof(mem_block_t *), "POOL_BLOCK_SIZE is too small!!");
_Static_assert(POOL_BLOCK_SIZE % 8 == 0, "POOL_BLOCK_SIZE must be multiple of alignof(max_align_t)");

/*********************** DECLARE FUNCTION *****************************/
/**
 * @brief Initialize the memory pool.
 *
 * This function prepares the memory pool for allocation by
 * setting up internal structures and marking all blocks as free.
 */
static void mem_pool_init(void);

/**
 * @brief Allocate one block from the memory pool.
 *
 * @return Pointer to the allocated memory block,
 *         or NULL if no free blocks are available.
 */
static void * mem_pool_allocate(void);

/**
 * @brief Free a previously allocated memory block.
 *
 * @param[in] ptr Pointer to the memory block to be released.
 *                Must be a pointer previously returned by mem_pool_allocate().
 */
static void mem_pool_free(void * ptr);

/**
 * @brief Get the number of free blocks currently available in the pool.
 *
 * @return Number of free blocks.
 */
static uint32_t mem_pool_getFreeBlockCount();

/**
 * @brief Get the maximum number of blocks used at the same time since init.
 *
 * This is useful for analyzing memory usage and tuning the pool size.
 *
 * @return Peak number of blocks used simultaneously.
 */
static uint32_t mem_pool_getMaxBlockUsed();

static void mem_pool_init(void)
{
    MEK_CRITICAL_SECTION_VAR;

    /* Entry critical section (Disable all interrupt) */
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_START();

    /* All blocks are not use when initialize => "max used block" = 0 */
    memPool.nMax      = 0;

    /* All blocks are not use when initialize => "free block available" = POOL_NUM_BLOCKS */
    memPool.nFree = POOL_NUM_BLOCKS;

    /*
    *   free_list
    *      |
    *      v
    *   +-----------+   next   +-----------+   next   +-----------+       ...      +-----------+      NULL
    *   | B[0]      |--------->| B[1]      |--------->| B[2]      |-----> ... ---->| B[N-1]    |----->◼
    *   +-----------+          +-----------+          +-----------+       ...      +-----------+
    */

    /*
     * Initialize the free list pointer to the first block
     * Casting the first cell to mem_block_t* makes the block header overlap
     * the block storage. This requires the block to be properly aligned
     */
    memPool.free_list = (mem_block_t *)&memPool.pool[0][0];

    /* Link all blocks into a singly linked free list */
    for(size_t i = 0; i < POOL_NUM_BLOCKS - 1; i++) {
        ((mem_block_t *)&memPool.pool[i])->next = (mem_block_t *)&memPool.pool[i + 1];
    }

    /* The last block is pointed to NULL */
    ((mem_block_t *)&memPool.pool[POOL_NUM_BLOCKS - 1])->next = NULL;

    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
}

static void * mem_pool_allocate(void)
{
    /* Entry critical section (Disable all interrupt) */
//	MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* No free blocks left? Return NULL */
    if( memPool.free_list == (mem_block_t *)0 )
    {
//        MEK_PORT_CRIT_EXIT();
        MEK_CRITICAL_SECTION_END();
        return NULL;
    }

    /*
     *BEFORE:   Block is available to allocate (free_list != NULL)
     *
     *   free_list
     *      |
     *      v
     *   +-----------+   next   +-----------+   next   +-----------+       ...      +-----------+      NULL
     *   | B[0]      |--------->| B[1]      |--------->| B[2]      |-----> ... ---->| B[N-1]    |----->◼
     *   +-----------+          +-----------+          +-----------+       ...      +-----------+
     *       |
     *       | pop head
     *       v
     *
     * AFTER:
     *
     *   new_block = Block[0]        free_list
     *       |                          |
     *       v                          v
     *   +-----------+              +-----------+   next   +-----------+       ...      +-----------+      NULL
     *   | B[0]      |   (detached) | B [1]     |--------->| B[2]      |-----> ... ---->| B[N-1]    |----->◼
     *   +-----------+              +-----------+          +-----------+       ...      +-----------+
     *
     */

    /* Pop the head of the free list and update accounting
     * PRE:  free_list != NULL
     * POST: returns former head; free_list moves to the next node; nFree--, nMax updated if needed
     * NOTE: new_block is now detached from the free list and can be used by the caller
     */
    mem_block_t * new_block = memPool.free_list;

    /* Point free_list pointer to next block */
    memPool.free_list = new_block->next;

    /* ENSURE nFree always > 0 before decrease */
    SYS_ASSERT(200, memPool.nFree > 0);

    /* Reduce Free slot after allocation */
    memPool.nFree--;

    /* Detect maximum block was used */
    uint32_t used = POOL_NUM_BLOCKS - memPool.nFree;
    if(used > memPool.nMax) {
        memPool.nMax = used;
        SYS_ASSERT(210 , memPool.nMax <= POOL_NUM_BLOCKS);
    }
    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();

    /* Return a pointer to new block */
    return (void *)new_block;
}

static void mem_pool_free(void * ptr)
{
    /* Entry critical section (Disable all interrupt) */
//	MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Ensure pointer to block is not NULL */
    if (ptr == (mem_block_t *)0)
    {
//        MEK_PORT_CRIT_EXIT();
        MEK_CRITICAL_SECTION_END();
        return;
    }

    /*
     * Push the returned block to the head of the free list
     *
     * BEFORE:
     *
     *   used_block = Block[0]       free_list
     *       |                          |
     *       v                          v
     *   +-----------+              +-----------+   next   +-----------+       ...      +-----------+      NULL
     *   | B[0]      |   (detached) | B [1]     |--------->| B[2]      |-----> ... ---->| B[N-1]    |----->◼
     *   +-----------+              +-----------+          +-----------+       ...      +-----------+
     *
     * AFTER:
     *
     *    free_list
     *       |
     *       v
     *   +-----------+   next   +-----------+   next   +-----------+       ...      +-----------+      NULL
     *   | B[0]      |--------->| B [1]     |--------->| B[2]      |-----> ... ---->| B[N-1]    |----->◼
     *   +-----------+          +-----------+          +-----------+       ...      +-----------+
     */
    mem_block_t * free_block = (mem_block_t *)ptr;
    free_block->next = memPool.free_list;

    /* free_list points to the most recently freed block (LIFO) */
    memPool.free_list = free_block;

    /* nFree is incremented by 1 (bounded by POOL_NUM_BLOCKS) */
    memPool.nFree++;

    /* ENSURE nFree always <= POOL_NUM_BLOCKS */
    SYS_ASSERT(300, memPool.nFree <= POOL_NUM_BLOCKS);

    /* Exit critical section (Enable all interrupt) */
//	MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
}

static uint32_t mem_pool_getFreeBlockCount()
{
    /* Entry critical section (Disable all interrupt) */
//	MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Get a current Total of free block */
    uint32_t count = memPool.nFree;

    /* Exit critical section (Enable all interrupt) */
//	MEK_PORT_CRIT_EXIT();
	MEK_CRITICAL_SECTION_END();
    return count;
}

static uint32_t mem_pool_getMaxBlockUsed()
{
    /* Entry critical section (Disable all interrupt) */
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Get a current Total of free block */
    uint32_t count = memPool.nMax;

    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
    return count;
}



/*********************** APPLICATION IMPLEMENTATION *****************************/
void MEK_Event_PoolInit()
{
    /* Initalize memory pool for application */
    mem_pool_init();
}

void * MEK_Event_Allocate(MSignal sig)
{
    /* Allocate pool for application */
    MEK_Evt_t *e = (MEK_Evt_t *)mem_pool_allocate();

    /* No block for allocate */
    if (e == NULL)
    {
        return NULL;
    }

    /* Entry critical section (Disable all interrupt) */
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Get signal */
    e->sig = sig;

    /* Init refCtr */
    e->refCtr = 0;

    /* Mark as used block pool */
    e->is_usedPool = true;

    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
    return (void *)e;
}

void MEK_Event_clearPool(MEK_Evt_t * const e)
{
    /* Entry critical section (Disable all interrupt) */
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Turn off flag use Pool */
    e->is_usedPool = false;

    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();

    /* Free block */
    mem_pool_free(e);
}

uint32_t MEK_Event_getMaxPoolWasUsed()
{
    /* Entry critical section (Disable all interrupt) */
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Get current Maximum block was used for application */
    uint32_t count = mem_pool_getMaxBlockUsed();

    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
    return count;
}

uint32_t MEK_Event_getFreeBlockCount()
{
    /* Entry critical section (Disable all interrupt) */
//    MEK_PORT_CRIT_ENTRY();
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    /* Get current Total's free block for application */
    uint32_t count = mem_pool_getFreeBlockCount();

    /* Exit critical section (Enable all interrupt) */
//    MEK_PORT_CRIT_EXIT();
    MEK_CRITICAL_SECTION_END();
    return count;
}

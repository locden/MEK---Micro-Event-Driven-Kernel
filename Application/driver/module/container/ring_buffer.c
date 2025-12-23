#include "ring_buffer.h"

#include "mek_port.h"
#include "sys_assert.h"

SYS_MODULE_NAME( "ring buffer" )

void ring_buffer_init(ring_buffer_t * const rb, void *buffer, size_t buffer_size, size_t element_size)
{
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    rb->buffer = buffer;
    rb->element_size = element_size;
    rb->buffer_size = buffer_size * rb->element_size;
    rb->head   = 0;
    rb->tail   = 0;

    MEK_CRITICAL_SECTION_END();
}

bool ring_buffer_push(ring_buffer_t * const rb, void * value)
{
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    size_t head = rb->head;
    size_t tail = rb->tail;

    MEK_CRITICAL_SECTION_END();

    size_t next_head = head + rb->element_size;

    if(next_head == rb->buffer_size)
    {
        next_head = 0;
    }

    if(next_head != tail)
    {
        memcpy(&rb->buffer[head], value, rb->element_size);

        MEK_CRITICAL_SECTION_START();
        rb->head = next_head;
        MEK_CRITICAL_SECTION_END();
    }
    else
    {
        MEK_CRITICAL_SECTION_END();
        return false;
    }

    return true;
}

bool ring_buffer_pop(ring_buffer_t * const rb, void *value)
{
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();

    size_t tail = rb->tail;
    size_t head = rb->head;

    MEK_CRITICAL_SECTION_END();

    if(tail == head)
    {
        return false;
    }

    memcpy(value, &rb->buffer[tail], rb->element_size);
    tail += rb->element_size;

    if(tail == rb->buffer_size)
    {
        tail = 0;
    }

    MEK_CRITICAL_SECTION_START();
    rb->tail = tail;
    MEK_CRITICAL_SECTION_END();

    return true;
}

size_t ring_buffer_available_slot(ring_buffer_t * const rb)
{
    SYS_ASSERT(400, rb->element_size > 0);
    SYS_ASSERT(410, rb->buffer_size % rb->element_size == 0);

    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();
    size_t tail = rb->tail;
    size_t head = rb->head;
    MEK_CRITICAL_SECTION_END();

    size_t ret;
    // Cái "- element" đầu tiên để xác đụng chính xac buffer thì do ví dụ m khai báo buffer của rb là 4 đi thì nó chỉ chạy từ 0 -> 3
    // Cái "- element" còn lại là 1 block chừa lại để xác định trạng thái giữa head và tail
    //(do không dùng counter để đếm block nên phải chừa 1 để xác định là đã ---> đầy, hay đang trống.
    if(head == tail)       ret = ((rb->buffer_size - rb->element_size)               ) / rb->element_size;
    else if(head < tail)   ret = ((tail - head - rb->element_size)                   ) / rb->element_size;
    else                   ret = ((rb->buffer_size - head + tail - rb->element_size) ) / rb->element_size;

    return ret;
}

size_t ring_buffer_use_slot(ring_buffer_t * const rb)
{
    MEK_CRITICAL_SECTION_VAR;
    MEK_CRITICAL_SECTION_START();
    size_t ret = rb->buffer_size / rb->element_size - rb->element_size - ring_buffer_available_slot(rb);
    MEK_CRITICAL_SECTION_END();

    return ret;
}



//void ring_buffer_process(ring_buffer_t * const rb, void (*callback)(void *, uint8_t))
//{
//    MEK_PORT_CRIT_ENTRY();
//    size_t tail = rb->tail;
//    size_t head = rb->head;
//    MEK_PORT_CRIT_EXIT();
//
//    while(tail != head)
//    {
//        (*callback)(rb->buffer[tail]);
//        tail += rb->element_size;
//        if(tail == head)
//        {
//            tail = 0;
//        }
//
//        MEK_PORT_CRIT_ENTRY();
//        rb->tail = tail;
//        MEK_PORT_CRIT_EXIT();
//    }
//}





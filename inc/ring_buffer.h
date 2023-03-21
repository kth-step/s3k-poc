#pragma once
#include <stdbool.h>
#include <stdint.h>

struct ring_buffer {
	char **buffer;
	volatile uint64_t head, tail, size;
};

static inline void ring_buffer_init(struct ring_buffer *rb, char **buffer, int size)
{
	rb->buffer = buffer;
	rb->size = size;
	rb->head = 0;
	rb->tail = 0;
}

static inline bool ring_buffer_push(struct ring_buffer *rb, char *ptr)
{
	if (rb->tail - rb->head >= rb->size)
		return false;
	rb->buffer[rb->tail % rb->size] = ptr;
	__sync_synchronize();
	rb->tail++;
	return true;
}

static inline bool ring_buffer_pop(struct ring_buffer *rb, char **ptr)
{
	if (rb->tail == rb->head)
		return false;
	*ptr = rb->buffer[rb->head % rb->size];
	__sync_synchronize();
	rb->head++;
	return true;
}

/****************************************************************************
 * halo -- Open HAL for embedded systems                                    *
 *                                                                          *
 * MIT License:                                                             *
 * Copyright (c) 2017 SimonQian                                             *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 ****************************************************************************/

#ifndef __BUFFER_H_INCLUDED__
#define __BUFFER_H_INCLUDED__

// queue
struct queue_node_t
{
	uint32_t addr;
	struct queue_node_t *next;
};
struct queue_t
{
	struct queue_node_t *head;
	struct queue_node_t *tail;
};

void queue_init(struct queue_t *q);
void queue_append(struct queue_t *q, struct queue_node_t *n);
void queue_remove(struct queue_t *q, struct queue_node_t *n);
void queue_enqueue(struct queue_t *q, struct queue_node_t *n);
struct queue_node_t* queue_dequeue(struct queue_t *q);

struct buffer_t
{
	uint8_t *buffer;
	uint32_t size;
};

struct transaction_buffer_t
{
	struct buffer_t buffer;
	uint32_t position;
};

// fifo
struct fifo_t
{
	struct buffer_t buffer;
	uint32_t head;
	uint32_t tail;
};
halo_err_t fifo_init(struct fifo_t *fifo);
uint32_t fifo_push8(struct fifo_t *fifo, uint8_t data);
uint8_t fifo_pop8(struct fifo_t *fifo);
uint32_t fifo_push(struct fifo_t *fifo, uint32_t size, uint8_t *data);
uint32_t fifo_pop(struct fifo_t *fifo, uint32_t size, uint8_t *data);
uint32_t fifo_peek(struct fifo_t *fifo, uint32_t size, uint8_t *data);
uint32_t fifo_get_wbuf(struct fifo_t *fifo, uint8_t **data);
uint32_t fifo_get_rbuf(struct fifo_t *fifo, uint8_t **data);
uint32_t fifo_get_data_length(struct fifo_t *fifo);
uint32_t fifo_get_avail_length(struct fifo_t *fifo);

// multi_buffer
struct multibuf_t
{
	uint32_t size;
	uint8_t **buffer_list;
	uint16_t count;

	uint16_t head;
	uint16_t tail;
	uint16_t length;
};

halo_err_t multibuf_init(struct multibuf_t *mbuffer);
uint8_t* multibuf_get_empty(struct multibuf_t *mbuffer);
halo_err_t multibuf_push(struct multibuf_t *mbuffer);
uint8_t* multibuf_get_payload(struct multibuf_t *mbuffer);
halo_err_t multibuf_pop(struct multibuf_t *mbuffer);

// buffer_manager
void bufmgr_init(uint8_t *buf, uint32_t size);

void* bufmgr_malloc_aligned_do(uint32_t size, uint32_t align);
void bufmgr_free_do(void *ptr);
#define bufmgr_malloc(s) 			bufmgr_malloc_aligned(s, 4)
#define bufmgr_malloc_aligned(s, a)	bufmgr_malloc_aligned_do(s, a)
#define bufmgr_free(p)				bufmgr_free_do(p)

// pool
struct pool_t
{
	uint32_t *flags;
	void *buffer;
	uint32_t size;
	uint32_t num;
};

#define POOL_DEFINE(name, type, num)			\
	struct pool_##name##_t\
	{\
		struct pool_t pool;\
		uint32_t mskarr[((num) + 31) >> 5];\
		type buffer[(num)];\
	} name

#define POOL_INIT(p, type, n)			\
	do {\
		(p)->pool.flags = (p)->mskarr;\
		(p)->pool.buffer = (p)->buffer;\
		(p)->pool.size = sizeof(type);\
		(p)->pool.num = (n);\
		pool_init((struct pool_t *)(p));\
	} while (0)

#define POOL_ALLOC(p, type)				\
    (type *)pool_alloc((struct pool_t *)(p))

#define POOL_FREE(p, buf)					\
	pool_free((struct pool_t *)(p), (buf))

void pool_init(struct pool_t *pool);
void* pool_alloc(struct pool_t *pool);
void pool_free(struct pool_t *pool, void *buffer);

#endif	// __BUFFER_H_INCLUDED__

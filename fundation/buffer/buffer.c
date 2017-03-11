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

#include "halo.h"

// queue
void queue_init(struct queue_t *q)
{
	q->head = q->tail = NULL;
}

void queue_append(struct queue_t *q, struct queue_node_t *n)
{
	n->next = NULL;
	if (NULL == q->tail)
	{
		q->head = q->tail = n;
	}
	else
	{
		q->tail->next = n;
		q->tail = n;
	}
}

void queue_remove(struct queue_t *q, struct queue_node_t *n)
{
	struct queue_node_t *head = q->head;

	if (head == n)
	{
		q->head = head->next;
		if (NULL == q->head)
		{
			q->tail = NULL;
		}
	}
	else if (head != NULL)
	{
		while (head->next != NULL)
		{
			if (head->next == n)
			{
				head->next = head->next->next;
				if (NULL == head->next)
				{
					q->tail = head;
				}
				break;
			}
			head = head->next;
		}
	}
}

void queue_enqueue(struct queue_t *q, struct queue_node_t *n)
{
	struct queue_node_t *tmp = q->head;

	n->next = NULL;
	if (NULL == tmp)
	{
		q->head = q->tail = n;
	}
	else if (tmp->addr >= n->addr)
	{
		n->next = tmp;
		q->head = n;
	}
	else
	{
		while (tmp->next != NULL)
		{
			if (tmp->next->addr >= n->addr)
			{
				n->next = tmp->next;
				tmp->next = n;
				break;
			}
			tmp = tmp->next;
		}
		// insert last
		tmp->next = n;
	}
}

struct queue_node_t* queue_dequeue(struct queue_t *q)
{
	struct queue_node_t *head = q->head;
	if (q->head != NULL)
	{
		q->head = q->head->next;
		if (NULL == q->head)
		{
			q->tail = NULL;
		}
	}
	return head;
}

//#define fifo_get_next_index(pos, size)	(((pos) + 1) % (size))
static uint32_t fifo_get_next_index(uint32_t pos, uint32_t size)
{
	if (++pos >= size)
	{
		pos = 0;
	}
	return pos;
}

halo_err_t fifo_init(struct fifo_t *fifo)
{
	fifo->head = fifo->tail = 0;
	return HALOERR_NONE;
}

uint32_t fifo_get_data_length(struct fifo_t *fifo)
{
	if (fifo->head >= fifo->tail)
	{
		return fifo->head - fifo->tail;
	}
	else
	{
		return fifo->buffer.size - (fifo->tail - fifo->head);
	}
}

uint32_t fifo_get_avail_length(struct fifo_t *fifo)
{
	uint32_t len;

	len = fifo->buffer.size - fifo_get_data_length(fifo);
	if (len > 0)
	{
		len--;
	}
	return len;
}

uint32_t fifo_push8(struct fifo_t *fifo, uint8_t data)
{
	if (fifo_get_avail_length(fifo) < 1)
	{
		return 0;
	}

	fifo->buffer.buffer[fifo->head] = data;
	fifo->head = fifo_get_next_index(fifo->head, fifo->buffer.size);
	return 1;
}

uint8_t fifo_pop8(struct fifo_t *fifo)
{
	uint8_t data;

	if (fifo_get_data_length(fifo) <= 0)
	{
		return 0;
	}

	data = fifo->buffer.buffer[fifo->tail];
	fifo->tail = fifo_get_next_index(fifo->tail, fifo->buffer.size);
	return data;
}

uint32_t fifo_push(struct fifo_t *fifo, uint32_t size, uint8_t *data)
{
	uint32_t tmp32;

	if (size > fifo_get_avail_length(fifo))
	{
		return 0;
	}

	tmp32 = fifo->buffer.size - fifo->head;
	if (size > tmp32)
	{
		if (data)
		{
			memcpy(&fifo->buffer.buffer[fifo->head], &data[0], tmp32);
			memcpy(&fifo->buffer.buffer[0], &data[tmp32], size - tmp32);
		}
		fifo->head = size - tmp32;
	}
	else
	{
		if (data)
		{
			memcpy(&fifo->buffer.buffer[fifo->head], data, size);
		}
		fifo->head += size;
		if (fifo->head == fifo->buffer.size)
		{
			fifo->head = 0;
		}
	}
	return size;
}

uint32_t fifo_get_rbuf(struct fifo_t *fifo, uint8_t **data)
{
	uint32_t tmp32, avail_len = fifo_get_data_length(fifo);

	if (data)
	{
		*data = &fifo->buffer.buffer[fifo->tail];
	}
	tmp32 = fifo->buffer.size - fifo->tail;
	return min(tmp32, avail_len);
}

uint32_t fifo_get_wbuf(struct fifo_t *fifo, uint8_t **data)
{
	uint32_t tmp32, avail_len = fifo_get_avail_length(fifo);

	if (data)
	{
		*data = &fifo->buffer.buffer[fifo->head];
	}
	tmp32 = fifo->buffer.size - fifo->head;
	return min(tmp32, avail_len);
}

uint32_t fifo_peek(struct fifo_t *fifo, uint32_t size, uint8_t *data)
{
	uint32_t tmp32;
	uint32_t avail_len = fifo_get_data_length(fifo);

	if (size > avail_len)
	{
		size = avail_len;
	}

	tmp32 = fifo->buffer.size - fifo->tail;
	if (data)
	{
		if (size > tmp32)
		{
			memcpy(&data[0], &fifo->buffer.buffer[fifo->tail], tmp32);
			memcpy(&data[tmp32], &fifo->buffer.buffer[0], size - tmp32);
		}
		else
		{
			memcpy(data, &fifo->buffer.buffer[fifo->tail], size);
		}
	}
	return size;
}

uint32_t fifo_pop(struct fifo_t *fifo, uint32_t size, uint8_t *data)
{
	uint32_t tmp32;
	uint32_t ret = fifo_peek(fifo, size, data);

	if (!ret)
	{
		return 0;
	}

	tmp32 = fifo->buffer.size - fifo->tail;
	if (ret > tmp32)
	{
		fifo->tail = ret - tmp32;
	}
	else
	{
		fifo->tail += ret;
		if (fifo->tail == fifo->buffer.size)
		{
			fifo->tail = 0;
		}
	}
	return ret;
}

// multibuf
halo_err_t multibuf_init(struct multibuf_t *mbuffer)
{
	mbuffer->tail = mbuffer->head = mbuffer->length = 0;
	return HALOERR_NONE;
}

uint8_t* multibuf_get_empty(struct multibuf_t *mbuffer)
{
	if (mbuffer->count <= mbuffer->length)
	{
		return NULL;
	}

	return mbuffer->buffer_list[mbuffer->head];
}

halo_err_t multibuf_push(struct multibuf_t *mbuffer)
{
	if (mbuffer->count <= mbuffer->length)
	{
		return HALOERR_FAIL;
	}

	mbuffer->head = (uint16_t)fifo_get_next_index(mbuffer->head, mbuffer->count);
	mbuffer->length++;
	return HALOERR_NONE;
}

uint8_t* multibuf_get_payload(struct multibuf_t *mbuffer)
{
	if (!mbuffer->length)
	{
		return NULL;
	}

	return mbuffer->buffer_list[mbuffer->tail];
}

halo_err_t multibuf_pop(struct multibuf_t *mbuffer)
{
	if (!mbuffer->length)
	{
		return HALOERR_FAIL;
	}

	mbuffer->tail = (uint16_t)fifo_get_next_index(mbuffer->tail, mbuffer->count);
	mbuffer->length--;
	return HALOERR_NONE;
}

// bufmgr
#define BUFMGR_DEFAULT_ALIGN				4
#define BUFMGR_BUF_CHECK_EN					0
#define BUFMGR_POINT_CHECK_EN				0

#if	BUFMGR_BUF_CHECK_EN
#define BUFMGR_MAX_SIZE						(64 * 1024)
#define BUFMGR_CHECK0						0x78563412
#define BUFMGR_CHECK1						0xf1debc9a
#define BUFMGR_INIT_BUF(a)					\
	do{\
		(a)->temp[0] = BUFMGR_CHECK0, (a)->temp[1] = BUFMGR_CHECK1;\
	}while(0)
#define BUFMGR_CHECK_BUF(a)					\
	(((a)->temp[0] == BUFMGR_CHECK0) && ((a)->temp[1] == BUFMGR_CHECK1))
#endif		// BUFMGR_BUF_CHECK_EN

struct bufmgr_mcb_t
{
	struct buffer_t buffer;
	struct sllist list;
#if BUFMGR_POINT_CHECK_EN
	void *p;
#endif
#if BUFMGR_BUF_CHECK_EN
	uint32_t temp[2];
#endif
};
struct bufmgr_t
{
	// private
	struct bufmgr_mcb_t freed_list;
	struct bufmgr_mcb_t allocated_list;

#if BUFMGR_BUF_CHECK_EN
	uint32_t err_conut;
#endif
};
#define MCB_SIZE sizeof(struct bufmgr_mcb_t)
static struct bufmgr_t bufmgr;

#if BUFMGR_BUF_CHECK_EN
static void bufmgr_error(void)
{
	if (bufmgr.err_conut < 0xffffffff)
	{
		bufmgr.err_conut++;
	}
}

static void bufmgr_check(struct bufmgr_mcb_t *mcb)
{
	if (!BUFMGR_CHECK_BUF(mcb))
	{
		bufmgr_error();
		BUFMGR_INIT_BUF(mcb);
	}
}
#endif

static void bufmgr_remove_mcb(struct bufmgr_mcb_t *list, struct bufmgr_mcb_t *mcb)
{
	struct bufmgr_mcb_t *active_mcb, *next_mcb, *prev_mcb;

	active_mcb = sllist_get_container(list->list.next, struct bufmgr_mcb_t, list);
	prev_mcb = list;
	while(active_mcb != NULL)
	{
		if(active_mcb == mcb)
		{
			next_mcb = sllist_get_container(active_mcb->list.next, struct bufmgr_mcb_t, list);
			if(next_mcb != NULL)
			{
				sllist_insert(prev_mcb->list, next_mcb->list);
			}
			else
			{
				sllist_init_node(prev_mcb->list);
			}
			sllist_init_node(active_mcb->list);
			return;
		}
		prev_mcb = active_mcb;
		active_mcb = sllist_get_container(active_mcb->list.next, struct bufmgr_mcb_t, list);
	}
}

static void bufmgr_insert_mcb(struct bufmgr_mcb_t *list, struct bufmgr_mcb_t *mcb)
{
	struct bufmgr_mcb_t *active_mcb, *prev_mcb;

#if BUFMGR_BUF_CHECK_EN
	BUFMGR_INIT_BUF(mcb);
#endif

	active_mcb = sllist_get_container(list->list.next, struct bufmgr_mcb_t, list);
	prev_mcb = list;

	while (active_mcb != NULL)
	{
		if(active_mcb->buffer.size >= mcb->buffer.size)
		{
			sllist_insert(prev_mcb->list, mcb->list);
			sllist_insert(mcb->list, active_mcb->list);
			return;
		}
		prev_mcb = active_mcb;
		active_mcb = sllist_get_container(active_mcb->list.next, struct bufmgr_mcb_t, list);
	}
	if(active_mcb == NULL)
	{
		sllist_insert(prev_mcb->list, mcb->list);
		sllist_init_node(mcb->list);
	}
}

static void bufmgr_merge_mcb(struct bufmgr_mcb_t *list, struct bufmgr_mcb_t *mcb)
{
	struct bufmgr_mcb_t *active_mcb, *prev_mcb = NULL, *next_mcb = NULL;

	active_mcb = sllist_get_container(list->list.next, struct bufmgr_mcb_t, list);

	while (active_mcb != NULL)
	{
		if(((uint32_t)mcb->buffer.buffer + mcb->buffer.size) == (uint32_t)active_mcb)
		{
			next_mcb = active_mcb;
		}
		if(((uint32_t)active_mcb->buffer.buffer + active_mcb->buffer.size) == (uint32_t)mcb)
		{
			prev_mcb = active_mcb;
		}
		active_mcb = sllist_get_container(active_mcb->list.next, struct bufmgr_mcb_t, list);
	}
	if((next_mcb == NULL) && (prev_mcb == NULL))
	{
		bufmgr_insert_mcb(list, mcb);
	}
	else if((next_mcb != NULL) && (prev_mcb == NULL))
	{
		bufmgr_remove_mcb(list, next_mcb);
		mcb->buffer.size += MCB_SIZE + next_mcb->buffer.size;
#if BUFMGR_BUF_CHECK_EN
		if (mcb->buffer.size > BUFMGR_MAX_SIZE)
		{
			bufmgr_error();
		}
#endif
		bufmgr_insert_mcb(list, mcb);
	}
	else if((next_mcb == NULL) && (prev_mcb != NULL))
	{
		bufmgr_remove_mcb(list, prev_mcb);
		prev_mcb->buffer.size += MCB_SIZE + mcb->buffer.size;
		bufmgr_insert_mcb(list, prev_mcb);
	}
	else if((next_mcb != NULL) && (prev_mcb != NULL))
	{
		bufmgr_remove_mcb(list, prev_mcb);
		bufmgr_remove_mcb(list, next_mcb);
		prev_mcb->buffer.size += MCB_SIZE * 2 + mcb->buffer.size + next_mcb->buffer.size;
		bufmgr_insert_mcb(list, prev_mcb);
	}
}

void bufmgr_init(uint8_t *buf, uint32_t size)
{
	struct bufmgr_mcb_t *mcb = (struct bufmgr_mcb_t *)buf;

	bufmgr.freed_list.buffer.buffer = NULL;
	bufmgr.freed_list.buffer.size = 0;
	sllist_init_node(bufmgr.freed_list.list);

	bufmgr.allocated_list.buffer.buffer = NULL;
	bufmgr.allocated_list.buffer.size = 0;
	sllist_init_node(bufmgr.allocated_list.list);

	mcb->buffer.buffer = (void *)(((uint32_t)buf + MCB_SIZE + 3) & 0xfffffffc);
	mcb->buffer.size = (uint32_t)buf + size - (uint32_t)mcb->buffer.buffer;
	sllist_init_node(mcb->list);
	sllist_insert(bufmgr.freed_list.list, mcb->list);

#if BUFMGR_BUF_CHECK_EN
	bufmgr.err_conut = 0;
#endif
}

#ifdef VSFCFG_BUFMGR_LOG
void* bufmgr_malloc_aligned_do(uint32_t size, uint32_t align,
		const char *format, ...)
#else
void* bufmgr_malloc_aligned(uint32_t size, uint32_t align)
#endif
{
	struct bufmgr_mcb_t *mcb= sllist_get_container(
				bufmgr.freed_list.list.next, struct bufmgr_mcb_t, list);
#ifdef VSFCFG_BUFMGR_LOG
	va_list ap;
	uint32_t size_out;
	va_start(ap, format);
	size_out = vsnprintf((char *)bufmgr_log_buf, BUFMGR_LOG_BUF_LENGTH, format, ap);
	va_end(ap);
#endif

	if (size == 0)
	{
		return NULL;
	}
	if (size & 0x3)
	{
		size &= 0xfffffffc;
		size += 4;
	}
	if (align < BUFMGR_DEFAULT_ALIGN)
	{
		align = BUFMGR_DEFAULT_ALIGN;
	}
	while (mcb != NULL)
	{
		uint32_t offset = (uint32_t)mcb->buffer.buffer % align;
		offset = offset ? (align - offset) : 0;

		if (mcb->buffer.size >= size + offset)
		{
			struct bufmgr_mcb_t *mcb_align;
			if (offset >= MCB_SIZE + BUFMGR_DEFAULT_ALIGN)
			{
				mcb_align = (struct bufmgr_mcb_t *)\
									(mcb->buffer.buffer + offset - MCB_SIZE);
				mcb_align->buffer.buffer = (uint8_t *)mcb_align + MCB_SIZE;
				mcb_align->buffer.size = mcb->buffer.size - offset;

				bufmgr_remove_mcb(&bufmgr.freed_list, mcb);
				mcb->buffer.size = offset - MCB_SIZE;
				bufmgr_insert_mcb(&bufmgr.freed_list, mcb);

				offset = 0; // as return ptr offset
			}
			else
			{
				mcb_align = mcb;
				bufmgr_remove_mcb(&bufmgr.freed_list, mcb_align);
			}

			if (mcb_align->buffer.size > (size + offset + MCB_SIZE))
			{
				struct bufmgr_mcb_t *mcb_tail = (struct bufmgr_mcb_t *)\
									(mcb_align->buffer.buffer + offset + size);
				mcb_tail->buffer.buffer = (uint8_t *)mcb_tail + MCB_SIZE;
				mcb_tail->buffer.size = mcb_align->buffer.size - offset - size -
										MCB_SIZE;
				bufmgr_insert_mcb(&bufmgr.freed_list, mcb_tail);

				mcb_align->buffer.size = offset + size;
			}

			bufmgr_insert_mcb(&bufmgr.allocated_list, mcb_align);
			
#ifdef VSFCFG_BUFMGR_LOG
			if (size_out > 1)
			{
				bufmgr_log_buf[size_out] = '\0';
				debug("MalcOK 0x%x:%d %s", (uint32_t)(mcb_align->buffer.buffer + offset), mcb_align->buffer.size, bufmgr_log_buf);
			}
			else
			{
				debug("MalcOK 0x%x:%d", (uint32_t)(mcb_align->buffer.buffer + offset), mcb_align->buffer.size);
			}
#endif
			
#if BUFMGR_POINT_CHECK_EN
			mcb_align->p = mcb_align->buffer.buffer + offset;
			return mcb_align->p;
#else
			return mcb_align->buffer.buffer + offset;
#endif

		}
		mcb = sllist_get_container(mcb->list.next, struct bufmgr_mcb_t,
									list);
	}

#ifdef VSFCFG_BUFMGR_LOG
	if (size_out > 1)
	{
		bufmgr_log_buf[size_out] = '\0';
		debug("MalcFL %s", bufmgr_log_buf);
	}
	else
	{
		debug("MalcFL UNKNOWN");
	}
#endif
	
#if BUFMGR_BUF_CHECK_EN
	bufmgr_error();
#endif
	return NULL;
}

#ifdef VSFCFG_BUFMGR_LOG
void bufmgr_free_do(void *ptr, const char *format, ...)
#else
void bufmgr_free(void *ptr)
#endif
{
	struct bufmgr_mcb_t *mcb = sllist_get_container(
			bufmgr.allocated_list.list.next,struct bufmgr_mcb_t, list);											 
#ifdef VSFCFG_BUFMGR_LOG
	va_list ap;
	uint32_t size_out;
	va_start(ap, format);
	size_out = vsnprintf((char *)bufmgr_log_buf, BUFMGR_LOG_BUF_LENGTH, format, ap);
	va_end(ap);
#endif
	
	
	while (mcb != NULL)
	{
		if (((uint32_t)mcb->buffer.buffer <= (uint32_t)ptr) &&
			((uint32_t)mcb->buffer.buffer + mcb->buffer.size > (uint32_t)ptr))
		{
#if BUFMGR_BUF_CHECK_EN
			bufmgr_check(mcb);
#endif
#if BUFMGR_POINT_CHECK_EN
			if (mcb->p != ptr)
			{
				bufmgr_error();
			}
#endif
			bufmgr_remove_mcb(&bufmgr.allocated_list, mcb);
			bufmgr_merge_mcb(&bufmgr.freed_list, mcb);
			
#ifdef VSFCFG_BUFMGR_LOG
			if (size_out > 1)
			{
				bufmgr_log_buf[size_out] = '\0';
				debug("FreeOK 0x%x %s", (uint32_t)ptr, bufmgr_log_buf);
			}
			else
			{
				debug("FreeOK 0x%x", (uint32_t)ptr);
			}
#endif

			return;
		}
		mcb = sllist_get_container(mcb->list.next, struct bufmgr_mcb_t,
									list);
	}

#ifdef VSFCFG_BUFMGR_LOG
	if (size_out > 1)
	{
		bufmgr_log_buf[size_out] = '\0';
		debug("FreeFL 0x%x %s", (uint32_t)ptr, bufmgr_log_buf);
	}
	else
	{
		debug("FreeFL 0x%x ", (uint32_t)ptr);
	}
#endif
	
#if BUFMGR_BUF_CHECK_EN
	bufmgr_error();
#endif
}

// pool
void pool_init(struct pool_t *pool)
{
	memset(pool->flags, 0, (pool->num + 31) >> 3);
}

void* pool_alloc(struct pool_t *pool)
{
	uint32_t index = mskarr_ffz(pool->flags, (pool->num + 31) >> 5);

	if (index >= pool->num)
	{
		return NULL;
	}
	mskarr_set(pool->flags, index);
	return (uint8_t *)pool->buffer + index * pool->size;
}

void pool_free(struct pool_t *pool, void *buffer)
{
	uint32_t index = ((uint8_t *)buffer - (uint8_t *)pool->buffer) / pool->size;

	if (index < pool->num)
	{
		mskarr_clr(pool->flags, index);
	}
}

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

#ifndef __STREAM_H_INCLUDED__
#define __STREAM_H_INCLUDED__

#include "../buffer/buffer.h"

struct stream_t;
struct stream_op_t
{
	void (*init)(struct stream_t *stream);
	void (*fini)(struct stream_t *stream);
	// for read/write, if buffer->buffer is NULL,
	// 		then do dummy read/write of buffer->size
	uint32_t (*write)(struct stream_t *stream, struct buffer_t *buffer);
	uint32_t (*read)(struct stream_t *stream, struct buffer_t *buffer);
	uint32_t (*get_data_length)(struct stream_t *stream);
	uint32_t (*get_avail_length)(struct stream_t *stream);
	// get pointer to consequent buffer for read/write
	uint32_t (*get_wbuf)(struct stream_t *stream, uint8_t **ptr);
	uint32_t (*get_rbuf)(struct stream_t *stream, uint8_t **ptr);
};

struct stream_cb_t
{
	void *param;
	void (*on_inout)(void *param);
	void (*on_connect)(void *param);
	void (*on_disconnect)(void *param);
};

struct stream_t
{
	struct stream_op_t const *op;

	// callback_tx is notification for tx end if any change in rx end
	struct stream_cb_t callback_tx;
	// callback_rx is notification for rx end if any change in tx end
	struct stream_cb_t callback_rx;
	bool tx_ready;
	bool rx_ready;
	bool overflow;
};

#define STREAM_INIT(s)			stream_init((struct stream_t *)(s))
#define STREAM_FINI(s)			stream_fini((struct stream_t *)(s))
#define STREAM_WRITE(s, b)		stream_write((struct stream_t *)(s), (b))
#define STREAM_READ(s, b)		stream_read((struct stream_t *)(s), (b))
#define STREAM_GET_DATA_SIZE(s)	stream_get_data_size((struct stream_t *)(s))
#define STREAM_GET_FREE_SIZE(s)	stream_get_free_size((struct stream_t *)(s))
#define STREAM_GET_WBUF(s, p)	stream_get_wbuf((struct stream_t *)(s), (p))
#define STREAM_GET_RBUF(s, p)	stream_get_rbuf((struct stream_t *)(s), (p))
#define STREAM_CONNECT_RX(s)	stream_connect_rx((struct stream_t *)(s))
#define STREAM_CONNECT_TX(s)	stream_connect_tx((struct stream_t *)(s))
#define STREAM_DISCONNECT_RX(s)	stream_disconnect_rx((struct stream_t *)(s))
#define STREAM_DISCONNECT_TX(s)	stream_disconnect_tx((struct stream_t *)(s))

// fifo stream, user_mem is fifo_t: available in interrupt
struct fifostream_t
{
	struct stream_t stream;
	struct fifo_t mem;
};
// multibuf stream, user_mem is multibuf_stream_t: unavailable in interrupt
struct mbufstream_mem_t
{
	struct multibuf_t multibuf;
	// private
	uint32_t rpos, wpos;
};
struct mbufstream_t
{
	struct stream_t stream;
	struct mbufstream_mem_t mem;
};
// buffer stream, user_mem is buffer_stream_t: unavailable in interrupt
struct bufstream_mem_t
{
	struct buffer_t buffer;
	bool read;
	// private
	uint32_t pos;
};
struct bufstream_t
{
	struct stream_t stream;
	struct bufstream_mem_t mem;
};

halo_err_t stream_init(struct stream_t *stream);
halo_err_t stream_fini(struct stream_t *stream);
uint32_t stream_write(struct stream_t *stream, struct buffer_t *buffer);
uint32_t stream_read(struct stream_t *stream, struct buffer_t *buffer);
uint32_t stream_get_data_size(struct stream_t *stream);
uint32_t stream_get_free_size(struct stream_t *stream);
uint32_t stream_get_wbuf(struct stream_t *stream, uint8_t **ptr);
uint32_t stream_get_rbuf(struct stream_t *stream, uint8_t **ptr);
void stream_connect_rx(struct stream_t *stream);
void stream_connect_tx(struct stream_t *stream);
void stream_disconnect_rx(struct stream_t *stream);
void stream_disconnect_tx(struct stream_t *stream);

extern const struct stream_op_t fifostream_op;
extern const struct stream_op_t mbufstream_op;
extern const struct stream_op_t bufstream_op;

#endif	// __STREAM_H_INCLUDED__

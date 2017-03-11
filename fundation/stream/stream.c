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

uint32_t stream_read(struct stream_t *stream, struct buffer_t *buffer)
{
	uint32_t count = stream->op->read(stream, buffer);

	if (stream->tx_ready && (stream->callback_tx.on_inout != NULL) && count)
	{
		stream->callback_tx.on_inout(stream->callback_tx.param);
	}
	return count;
}

uint32_t stream_write(struct stream_t *stream, struct buffer_t *buffer)
{
	uint32_t count = stream->op->write(stream, buffer);

	if (count < buffer->size)
	{
		stream->overflow = true;
	}
	if (stream->rx_ready && (stream->callback_rx.on_inout != NULL) && count)
	{
		stream->callback_rx.on_inout(stream->callback_rx.param);
	}
	return count;
}

uint32_t stream_get_data_size(struct stream_t *stream)
{
	return stream->op->get_data_length(stream);
}

uint32_t stream_get_free_size(struct stream_t *stream)
{
	return stream->op->get_avail_length(stream);
}

uint32_t stream_get_wbuf(struct stream_t *stream, uint8_t **ptr)
{
	return stream->op->get_wbuf(stream, ptr);
}

uint32_t stream_get_rbuf(struct stream_t *stream, uint8_t **ptr)
{
	return stream->op->get_rbuf(stream, ptr);
}

void stream_connect_rx(struct stream_t *stream)
{
	if (!stream->rx_ready)
	{
		if (stream->callback_tx.on_connect != NULL)
		{
			stream->callback_tx.on_connect(stream->callback_tx.param);
		}
		if ((stream->tx_ready) && (stream->callback_rx.on_connect != NULL))
		{
			stream->callback_rx.on_connect(stream->callback_rx.param);
		}
		stream->rx_ready = true;
	}
}

void stream_connect_tx(struct stream_t *stream)
{
	if (!stream->tx_ready)
	{
		if (stream->callback_rx.on_connect != NULL)
		{
			stream->callback_rx.on_connect(stream->callback_rx.param);
		}
		if ((stream->rx_ready) && (stream->callback_tx.on_connect != NULL))
		{
			stream->callback_tx.on_connect(stream->callback_tx.param);
		}
		stream->tx_ready = true;
	}
}

void stream_disconnect_rx(struct stream_t *stream)
{
	if (stream->rx_ready && (stream->callback_tx.on_disconnect != NULL))
	{
		stream->callback_tx.on_disconnect(stream->callback_tx.param);
	}
	stream->rx_ready = false;
}

void stream_disconnect_tx(struct stream_t *stream)
{
	if (stream->tx_ready && (stream->callback_rx.on_disconnect != NULL))
	{
		stream->callback_rx.on_disconnect(stream->callback_rx.param);
	}
	stream->tx_ready = false;
}

halo_err_t stream_init(struct stream_t *stream)
{
	stream->overflow = false;
	stream->tx_ready = false;
	stream->rx_ready = false;
	if (stream->op->init != NULL)
	{
		stream->op->init(stream);
	}
	return HALOERR_NONE;
}

halo_err_t stream_fini(struct stream_t *stream)
{
	if (stream->tx_ready)
	{
		stream_disconnect_tx(stream);
	}
	if (stream->rx_ready)
	{
		stream_disconnect_rx(stream);
	}
	if (stream->op->fini != NULL)
	{
		stream->op->fini(stream);
	}
	return HALOERR_NONE;
}

// fifo stream
static void fifo_stream_init(struct stream_t *stream)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	fifo_init(&fifostream->mem);
}

static uint32_t fifo_stream_get_data_length(struct stream_t *stream)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	return fifo_get_data_length(&fifostream->mem);
}

static uint32_t fifo_stream_get_avail_length(struct stream_t *stream)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	return fifo_get_avail_length(&fifostream->mem);
}

static uint32_t fifo_stream_get_wbuf(struct stream_t *stream, uint8_t **ptr)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	return fifo_get_wbuf(&fifostream->mem, ptr);
}

static uint32_t fifo_stream_get_rbuf(struct stream_t *stream, uint8_t **ptr)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	return fifo_get_rbuf(&fifostream->mem, ptr);
}

static uint32_t
fifo_stream_write(struct stream_t *stream, struct buffer_t *buffer)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	return fifo_push(&fifostream->mem, buffer->size, buffer->buffer);
}

static uint32_t
fifo_stream_read(struct stream_t *stream, struct buffer_t *buffer)
{
	struct fifostream_t *fifostream = (struct fifostream_t *)stream;
	return fifo_pop(&fifostream->mem, buffer->size, buffer->buffer);
}

// multibuf stream
static void multibuf_stream_init(struct stream_t *stream)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;

	mbufstream->mem.rpos = mbufstream->mem.wpos = 0;
	multibuf_init(&mbufstream->mem.multibuf);
}

static uint32_t multibuf_stream_get_data_length(struct stream_t *stream)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;

	return  (mbufstream->mem.multibuf.length * mbufstream->mem.multibuf.size)
				+ mbufstream->mem.wpos - mbufstream->mem.rpos;
}

static uint32_t multibuf_stream_get_avail_length(struct stream_t *stream)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;

	return (mbufstream->mem.multibuf.count * mbufstream->mem.multibuf.size) -
								multibuf_stream_get_data_length(stream);
}

static uint32_t
multibuf_stream_get_wbuf(struct stream_t *stream, uint8_t **ptr)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;
	uint8_t *buf = multibuf_get_empty(&mbufstream->mem.multibuf);
	uint32_t avail_len = STREAM_GET_FREE_SIZE(&mbufstream->mem.multibuf);
	uint32_t avail_buf = mbufstream->mem.multibuf.size - mbufstream->mem.wpos;

	if (ptr)
		*ptr = &buf[mbufstream->mem.wpos];
	return min(avail_len, avail_buf);
}

static uint32_t
multibuf_stream_get_rbuf(struct stream_t *stream, uint8_t **ptr)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;
	uint8_t *buf = multibuf_get_payload(&mbufstream->mem.multibuf);
	uint32_t avail_len = STREAM_GET_DATA_SIZE(&mbufstream->mem.multibuf);
	uint32_t avail_buf = mbufstream->mem.multibuf.size - mbufstream->mem.rpos;

	if (ptr)
		*ptr = &buf[mbufstream->mem.rpos];
	return min(avail_len, avail_buf);
}

static uint32_t
multibuf_stream_write(struct stream_t *stream, struct buffer_t *buffer)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;
	uint8_t *buf = multibuf_get_empty(&mbufstream->mem.multibuf);
	uint32_t wsize = 0, cur_size, remain_size = buffer->size;

	while ((buf != NULL) && (remain_size > 0))
	{
		cur_size = mbufstream->mem.multibuf.size - mbufstream->mem.wpos;
		cur_size = min(cur_size, remain_size);
		if (buffer->buffer)
			memcpy(&buf[mbufstream->mem.wpos], &buffer->buffer[wsize], cur_size);
		wsize += cur_size;
		remain_size -= cur_size;

		mbufstream->mem.wpos += cur_size;
		if (mbufstream->mem.wpos >= mbufstream->mem.multibuf.size)
		{
			multibuf_push(&mbufstream->mem.multibuf);
			buf = multibuf_get_empty(&mbufstream->mem.multibuf);
			mbufstream->mem.wpos = 0;
		}
	}
	return wsize;
}

static uint32_t
multibuf_stream_read(struct stream_t *stream, struct buffer_t *buffer)
{
	struct mbufstream_t *mbufstream = (struct mbufstream_t *)stream;
	uint8_t *buf = multibuf_get_payload(&mbufstream->mem.multibuf);
	uint32_t rsize = 0, cur_size, remain_size = buffer->size;

	while ((buf != NULL) && (remain_size > 0))
	{
		cur_size = mbufstream->mem.multibuf.size - mbufstream->mem.rpos;
		cur_size = min(cur_size, remain_size);
		if (buffer->buffer)
			memcpy(&buffer->buffer[rsize], &buf[mbufstream->mem.rpos], cur_size);
		rsize += cur_size;
		remain_size -= cur_size;

		mbufstream->mem.rpos += cur_size;
		if (mbufstream->mem.rpos >= mbufstream->mem.multibuf.size)
		{
			multibuf_pop(&mbufstream->mem.multibuf);
			buf = multibuf_get_payload(&mbufstream->mem.multibuf);
			mbufstream->mem.rpos = 0;
		}
	}
	return rsize;
}

// buffer stream
static void buffer_stream_init(struct stream_t *stream)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	bufstream->mem.pos = 0;
}

static uint32_t buffer_stream_get_data_length(struct stream_t *stream)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	return !bufstream->mem.read ? bufstream->mem.pos :
						bufstream->mem.buffer.size - bufstream->mem.pos;
}

static uint32_t buffer_stream_get_avail_length(struct stream_t *stream)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	return bufstream->mem.read ? bufstream->mem.pos :
						bufstream->mem.buffer.size - bufstream->mem.pos;
}

static uint32_t
buffer_stream_get_wbuf(struct stream_t *stream, uint8_t **ptr)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	uint8_t *p = NULL;
	uint32_t size = 0;

	if (!bufstream->mem.read)
	{
		p = bufstream->mem.buffer.buffer + bufstream->mem.pos;
		size = buffer_stream_get_avail_length(stream);
	}

	if (ptr)
		*ptr = p;
	return size;
}

static uint32_t
buffer_stream_get_rbuf(struct stream_t *stream, uint8_t **ptr)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	uint8_t *p = NULL;
	uint32_t size = 0;

	if (bufstream->mem.read)
	{
		p = bufstream->mem.buffer.buffer + bufstream->mem.pos;
		size = buffer_stream_get_data_length(stream);
	}

	if (ptr)
		*ptr = p;
	return size;
}

static uint32_t
buffer_stream_write(struct stream_t *stream, struct buffer_t *buffer)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	uint32_t wsize = 0;

	if (bufstream->mem.read)
	{
		bufstream->mem.buffer = *buffer;
		bufstream->mem.pos = 0;
		wsize = buffer->size;
	}
	else
	{
		uint32_t avail_len = buffer_stream_get_avail_length(stream);
		wsize = min(avail_len, buffer->size);
		if (buffer->buffer)
			memcpy(bufstream->mem.buffer.buffer + bufstream->mem.pos,
					buffer->buffer, wsize);
		bufstream->mem.pos += wsize;
	}
	return wsize;
}

static uint32_t
buffer_stream_read(struct stream_t *stream, struct buffer_t *buffer)
{
	struct bufstream_t *bufstream = (struct bufstream_t *)stream;
	uint32_t rsize = 0;

	if (bufstream->mem.read)
	{
		uint32_t data_len = buffer_stream_get_data_length(stream);
		rsize = min(data_len, buffer->size);
		if (buffer->buffer)
			memcpy(buffer->buffer,
					bufstream->mem.buffer.buffer + bufstream->mem.pos, rsize);
		bufstream->mem.pos += rsize;
	}
	else
	{
		bufstream->mem.buffer = *buffer;
		bufstream->mem.pos = 0;
		// just to notify the rx end
		rsize = 1;
	}
	return rsize;
}

const struct stream_op_t fifostream_op =
{
	.init = fifo_stream_init,
	.fini = fifo_stream_init,
	.write = fifo_stream_write,
	.read = fifo_stream_read,
	.get_data_length = fifo_stream_get_data_length,
	.get_avail_length = fifo_stream_get_avail_length,
	.get_wbuf = fifo_stream_get_wbuf,
	.get_rbuf = fifo_stream_get_rbuf,
};

const struct stream_op_t mbufstream_op =
{
	.init = multibuf_stream_init,
	.fini = multibuf_stream_init,
	.write = multibuf_stream_write,
	.read = multibuf_stream_read,
	.get_data_length = multibuf_stream_get_data_length,
	.get_avail_length = multibuf_stream_get_avail_length,
	.get_wbuf = multibuf_stream_get_wbuf,
	.get_rbuf = multibuf_stream_get_rbuf,
};

const struct stream_op_t bufstream_op =
{
	.init = buffer_stream_init,
	.fini = buffer_stream_init,
	.write = buffer_stream_write,
	.read = buffer_stream_read,
	.get_data_length = buffer_stream_get_data_length,
	.get_avail_length = buffer_stream_get_avail_length,
	.get_wbuf = buffer_stream_get_wbuf,
	.get_rbuf = buffer_stream_get_rbuf,
};

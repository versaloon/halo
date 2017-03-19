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

#ifndef __HALO_H_INCLUDED__
#define __HALO_H_INCLUDED__

#include "compiler.h"
#include "halo_const.h"
#include "halo_basetype.h"

#include "halo_type.h"
#include "halo_cfg.h"

// fundation
#include "fundation/bittool/bittool.h"
#include "fundation/buffer/buffer.h"
#include "fundation/list/list.h"
#include "fundation/stream/stream.h"

// hal lv0 api
// core
halo_err_t halo_core_init(struct halo_sysinfo_t *info);
halo_err_t halo_core_fini(void);
halo_err_t halo_core_reset(void);
halo_err_t halo_core_sleep(enum halo_sleep_mode_t mode);
// uid
uint32_t halo_uid_get(uint8_t *buffer, uint32_t size);
// pendsv
halo_err_t halo_pendsv_config(void (*on_pendsv)(void *), void *param);
halo_err_t halo_pendsv_trigger(void);
// tickclk
halo_err_t halo_tickclk_init(void);
halo_err_t halo_tickclk_fini(void);
halo_err_t halo_tickclk_start(void);
halo_err_t halo_tickclk_stop(void);
halo_err_t halo_tickclk_config_cb(void (*callback)(void*), void *param);
void halo_tickclk_poll();
// gpio
halo_err_t halo_gpio_init(uint8_t index);
halo_err_t halo_gpio_fini(uint8_t index);
halo_err_t halo_gpio_config_pin(uint8_t index, uint8_t pin_idx, enum halo_gpio_mode_t mode);
halo_err_t halo_gpio_config(uint8_t index, uint32_t pin_mask, uint32_t io, uint32_t pull_en_mask, uint32_t input_pull_mask);
halo_err_t halo_gpio_in(uint8_t index, uint32_t pin_mask, uint32_t *value);
halo_err_t halo_gpio_out(uint8_t index, uint32_t pin_mask, uint32_t value);
halo_err_t halo_gpio_set(uint8_t index, uint32_t pin_mask);
halo_err_t halo_gpio_clear(uint8_t index, uint32_t pin_mask);
uint32_t halo_gpio_get(uint8_t index, uint32_t pin_mask);
// spi

#endif    // __HALO_H_INCLUDED__

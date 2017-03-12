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

#include "halo_const.h"
#include "halo_basetype.h"

#include "halo_type.h"
#include "halo_cfg.h"

// fundation
#include "fundation/bittool/bittool.h"
#include "fundation/buffer/buffer.h"
#include "fundation/list/list.h"
#include "fundation/stream/stream.h"

// hal
struct halo_core_t
{
	halo_err_t (*init)(void *p);
	halo_err_t (*fini)(void *p);
	halo_err_t (*reset)(void *p);
	uint32_t (*get_stack)(void);
	halo_err_t (*set_stack)(uint32_t sp);
	void (*sleep)(uint32_t mode);
	halo_err_t (*pendsv_config)(void (*on_pendsv)(void *), void *param);
	halo_err_t (*pendsv_trigger)(void);
}

struct halo_gpio_pin_t
{
	uint8_t port;
	uint8_t pin;
};

#if IFS_GPIO_EN

#define CORE_GPIO_INFLOAT(m)			__CONNECT(m, _GPIO_INFLOAT)
#define CORE_GPIO_INPU(m)				__CONNECT(m, _GPIO_INPU)
#define CORE_GPIO_INPD(m)				__CONNECT(m, _GPIO_INPD)
#define CORE_GPIO_OUTPP(m)				__CONNECT(m, _GPIO_OUTPP)
#define CORE_GPIO_OUTOD(m)				__CONNECT(m, _GPIO_OUTOD)
struct halo_gpio_t
{
#if IFS_CONST_EN
	struct
	{
		uint32_t INFLOAT;
		uint32_t INPU;
		uint32_t INPD;
		uint32_t OUTPP;
		uint32_t OUTOD;
	} constants;
#endif
	halo_err_t (*init)(uint8_t index);
	halo_err_t (*fini)(uint8_t index);
	halo_err_t (*config_pin)(uint8_t index, uint8_t pin_idx, uint32_t mode);
	halo_err_t (*config)(uint8_t index, uint32_t pin_mask, uint32_t io, uint32_t pull_en_mask, uint32_t input_pull_mask);
	halo_err_t (*set)(uint8_t index, uint32_t pin_mask);
	halo_err_t (*clear)(uint8_t index, uint32_t pin_mask);
	halo_err_t (*out)(uint8_t index, uint32_t pin_mask, uint32_t value);
	halo_err_t (*in)(uint8_t index, uint32_t pin_mask, uint32_t *value);
	uint32_t (*get)(uint8_t index, uint32_t pin_mask);
};

#define HALO_GPIO_INIT(m)				__CONNECT(m, _gpio_init)
#define HALO_GPIO_FINI(m)				__CONNECT(m, _gpio_fini)
#define HALO_GPIO_CONFIG_PIN(m)			__CONNECT(m, _gpio_config_pin)
#define HALO_GPIO_CONFIG(m)				__CONNECT(m, _gpio_config)
#define HALO_GPIO_IN(m)					__CONNECT(m, _gpio_in)
#define HALO_GPIO_OUT(m)				__CONNECT(m, _gpio_out)
#define HALO_GPIO_SET(m)				__CONNECT(m, _gpio_set)
#define HALO_GPIO_CLEAR(m)				__CONNECT(m, _gpio_clear)
#define HALO_GPIO_GET(m)				__CONNECT(m, _gpio_get)

halo_err_t HALO_GPIO_INIT(__TARGET_CHIP__)(uint8_t index);
halo_err_t HALO_GPIO_FINI(__TARGET_CHIP__)(uint8_t index);
halo_err_t HALO_GPIO_CONFIG_PIN(__TARGET_CHIP__)(uint8_t index, uint8_t pin_idx, uint32_t mode);
halo_err_t HALO_GPIO_CONFIG(__TARGET_CHIP__)(uint8_t index, uint32_t pin_mask, uint32_t io, uint32_t pull_en_mask, uint32_t input_pull_mask);
halo_err_t HALO_GPIO_IN(__TARGET_CHIP__)(uint8_t index, uint32_t pin_mask, uint32_t *value);
halo_err_t HALO_GPIO_OUT(__TARGET_CHIP__)(uint8_t index, uint32_t pin_mask, uint32_t value);
halo_err_t HALO_GPIO_SET(__TARGET_CHIP__)(uint8_t index, uint32_t pin_mask);
halo_err_t HALO_GPIO_CLEAR(__TARGET_CHIP__)(uint8_t index, uint32_t pin_mask);
uint32_t HALO_GPIO_GET(__TARGET_CHIP__)(uint8_t index, uint32_t pin_mask);

#define GPIO_INFLOAT					HALO_GPIO_INFLOAT(__TARGET_CHIP__)
#define GPIO_INPU						HALO_GPIO_INPU(__TARGET_CHIP__)
#define GPIO_INPD						HALO_GPIO_INPD(__TARGET_CHIP__)
#define GPIO_OUTPP						HALO_GPIO_OUTPP(__TARGET_CHIP__)
#define GPIO_OUTOD						HALO_GPIO_OUTOD(__TARGET_CHIP__)

#define halo_gpio_init					HALO_GPIO_INIT(__TARGET_CHIP__)
#define halo_gpio_fini					HALO_GPIO_FINI(__TARGET_CHIP__)
#define halo_gpio_config_pin			HALO_GPIO_CONFIG_PIN(__TARGET_CHIP__)
#define halo_gpio_config				HALO_GPIO_CONFIG(__TARGET_CHIP__)
#define halo_gpio_in					HALO_GPIO_IN(__TARGET_CHIP__)
#define halo_gpio_out					HALO_GPIO_OUT(__TARGET_CHIP__)
#define halo_gpio_set					HALO_GPIO_SET(__TARGET_CHIP__)
#define halo_gpio_clear					HALO_GPIO_CLEAR(__TARGET_CHIP__)
#define halo_gpio_get					HALO_GPIO_GET(__TARGET_CHIP__)

#endif

#endif    // __HALO_H_INCLUDED__

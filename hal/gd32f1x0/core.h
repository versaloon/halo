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

#ifndef __HALO_CORE_H_INCLUDED__
#define __HALO_CORE_H_INCLUDED__

#define GD32F1X0_CLK_HSI				(1UL << 0)
#define GD32F1X0_CLK_HSE				(1UL << 1)
#define GD32F1X0_CLK_PLL				(1UL << 2)

struct halo_afio_pin_t
{
	uint8_t port;
	uint8_t pin;
	int8_t af;
};
halo_err_t halo_afio_config(const struct halo_afio_pin_t *pin, uint32_t mode);

enum gd32f1x0_clksrc_t
{
	GD32F1X0_CLKSRC_HSI = 0,
	GD32F1X0_CLKSRC_HSE = 1,
	GD32F1X0_CLKSRC_PLL = 2
};
enum gd32f1x0_pllsrc_t
{
	GD32F1X0_PLLSRC_HSID2,
	GD32F1X0_PLLSRC_HSE,
	GD32F1X0_PLLSRC_HSED2,
};
struct halo_sysinfo_t
{
	uint8_t priority_group;
	uint32_t vector_table;
	
	uint32_t clk_enable;
	
	enum gd32f1x0_clksrc_t clksrc;
	enum gd32f1x0_pllsrc_t pllsrc;
	
	uint32_t hse_freq_hz;
	uint32_t hsi_freq_hz;
	uint32_t pll_freq_hz;
	uint32_t ahb_freq_hz;
	uint32_t apb_freq_hz;
	
	uint32_t sys_freq_hz;
};

#endif	// __GD32F1X0_CORE_H_INCLUDED__

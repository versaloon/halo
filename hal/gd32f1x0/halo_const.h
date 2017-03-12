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

#ifndef __GD32F1X0_CONST_H_INCLUDED__
#define __GD32F1X0_CONST_H_INCLUDED__

#include "gd32f1x0.h"
#include "core.h"

#define gd32f1x0_SLEEP_WFI				0
#define gd32f1x0_SLEEP_PWRDOWN			1

#define gd32f1x0_GPIO_INFLOAT			0x00
#define gd32f1x0_GPIO_INPU				0x08
#define gd32f1x0_GPIO_INPD				0x10
#define gd32f1x0_GPIO_OUTPP				0x01
#define gd32f1x0_GPIO_OUTOD				0x05

#define gd32f1x0_GPIO_OD				0x04
#define gd32f1x0_GPIO_AF				0x02
#define gd32f1x0_GPIO_AN				0x03

#endif	// __GD32F1X0_CONST_H_INCLUDED__

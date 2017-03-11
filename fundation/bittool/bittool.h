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

#ifndef __BITTOOL_H_INCLUDED__
#define __BITTOOL_H_INCLUDED__

uint8_t BIT_REVERSE_U8(uint8_t);
uint16_t BIT_REVERSE_U16(uint16_t);
uint32_t BIT_REVERSE_U32(uint32_t);
uint64_t BIT_REVERSE_U64(uint64_t);

// GET_UXX_XXXXXXXX and SET_UXX_XXXXXXXX are align independent
uint16_t GET_U16_MSBFIRST(uint8_t *p);
uint32_t GET_U24_MSBFIRST(uint8_t *p);
uint32_t GET_U32_MSBFIRST(uint8_t *p);
uint64_t GET_U64_MSBFIRST(uint8_t *p);
uint16_t GET_U16_LSBFIRST(uint8_t *p);
uint32_t GET_U24_LSBFIRST(uint8_t *p);
uint32_t GET_U32_LSBFIRST(uint8_t *p);
uint64_t GET_U64_LSBFIRST(uint8_t *p);

void SET_U16_MSBFIRST(uint8_t *p, uint16_t v16);
void SET_U24_MSBFIRST(uint8_t *p, uint32_t v32);
void SET_U32_MSBFIRST(uint8_t *p, uint32_t v32);
void SET_U64_MSBFIRST(uint8_t *p, uint64_t v64);
void SET_U16_LSBFIRST(uint8_t *p, uint16_t v16);
void SET_U24_LSBFIRST(uint8_t *p, uint32_t v32);
void SET_U32_LSBFIRST(uint8_t *p, uint32_t v32);
void SET_U64_LSBFIRST(uint8_t *p, uint64_t v64);

uint16_t SWAP_U16(uint16_t);
uint32_t SWAP_U24(uint32_t);
uint32_t SWAP_U32(uint32_t);
uint64_t SWAP_U64(uint64_t);

int msb(uint32_t);
int ffz(uint32_t);

// mask array
void mskarr_set(uint32_t *arr, int bit);
void mskarr_clr(uint32_t *arr, int bit);
int mskarr_ffz(uint32_t *arr, int arrlen);

#endif // __BITTOOL_H_INCLUDED__

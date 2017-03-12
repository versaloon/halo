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

#if IFS_GPIO_EN

#define GD32F1X0_GPIO_NUM					6

#define RCC_APB2Periph_GPIOA			((uint32_t)0x00000004)

vsf_err_t gd32f1x0_gpio_init(uint8_t index)
{
	RCC->AHBCCR |= RCC_AHBCCR_PAEN << index;
	return VSFERR_NONE;
}

vsf_err_t gd32f1x0_gpio_fini(uint8_t index)
{
	RCC->AHBRCR |= RCC_AHBRCR_PARST << index;
	RCC->AHBCCR &= ~(RCC_AHBCCR_PAEN << index);
	return VSFERR_NONE;
}

vsf_err_t gd32f1x0_gpio_config_pin(uint8_t index, uint8_t pin_idx, uint32_t mode)
{
	GPIO_TypeDef *gpio;
	uint8_t offset = pin_idx << 1;
	
	gpio = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)index << 10));
	
	gpio->CTLR = (gpio->CTLR & ~(0x3 << offset)) | ((mode & 0x3) << offset);
	
	gpio->OMODE &= ~(GPIO_OMODE_OM0 << pin_idx);
	gpio->OMODE |= ((mode >> 2) & 0x1) << offset;
	
	gpio->PUPD &= ~(GPIO_PUPD_PUPD0 << offset);
	gpio->PUPD |= ((mode >> 3) & 0x3) << offset;
	
	gpio->OSPD &= ~(GPIO_OSPD_OSPD0 << offset);
	gpio->OSPD |= GPIO_OSPD_OSPD0 << offset;
//	gpio->OSPD |= ((mode >> 5) & 0x3) << offset;
	
	return VSFERR_NONE;
}

vsf_err_t gd32f1x0_gpio_config(uint8_t index, uint32_t pin_mask, uint32_t io, 
							uint32_t pull_en_mask, uint32_t input_pull_mask)
{
	return VSFERR_NONE;
}

vsf_err_t gd32f1x0_gpio_set(uint8_t index, uint32_t pin_mask)
{
	GPIO_TypeDef *gpio;
	
	gpio = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)index << 10));
	gpio->BOR = pin_mask & 0xffff;
	return VSFERR_NONE;
}

vsf_err_t gd32f1x0_gpio_clear(uint8_t index, uint32_t pin_mask)
{
	GPIO_TypeDef *gpio;
	
	gpio = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)index << 10));
	gpio->BCR = pin_mask;
	return VSFERR_NONE;
}

vsf_err_t gd32f1x0_gpio_out(uint8_t index, uint32_t pin_mask, uint32_t value)
{
	GPIO_TypeDef *gpio;
	
	gpio = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)index << 10));
	gpio->BOR = ((pin_mask & ~value) << 16) | (pin_mask & value);
	return VSFERR_NONE;
}

uint32_t gd32f1x0_gpio_get(uint8_t index, uint32_t pin_mask)
{
	GPIO_TypeDef *gpio;
	
	gpio = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)index << 10));
	return gpio->DIR & pin_mask;
}

vsf_err_t gd32f1x0_gpio_in(uint8_t index, uint32_t pin_mask, uint32_t *value)
{
	*value = gd32f1x0_gpio_get(index, pin_mask);
	return VSFERR_NONE;
}

#endif


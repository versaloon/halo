#通用HAL(待命名)

通用HAL用来解决嵌入式MCU开发过程中，高层代码对底层硬件的依赖问题。通过实现通用的硬件抽象层（HAL），来实现对底层硬件的屏蔽。

1. **快速开发**： 应用程序开发用户，不需要熟悉目标MCU的各个外设寄存器，直接调用HAL中的通用接口。
2. **快速移植**：硬件BSP定义了和硬件相关的各个设置，应用程序直接更新这个设置，就可以实现应用的移植。

--------------------------------

[TOC]

##简介

随着现在ARM等32位MCU的流行，系统资源和性能得到了非常大的丰富和提高。通用HAL的设计，就是为了实现在不同的MCU上，提供同一套硬件访问接口，使得高层的代码可以硬件无关，提高开发和维护的效率。并且，代码都是构架无关的，高层应用可以使用各种不同的构架以及实时操作系统（RTOS）。以下的示例代码，我试用vsfhal层的实现，这里也只是示例，并不代表最终的实现。

###简单的基于函数的访问接口

``` c
vsfhal_gpio_init(USBD_PULLUP_PORT);
vsfhal_gpio_clear(USBD_PULLUP_PORT, 1 << USBD_PULLUP_PIN);
vsfhal_gpio_config_pin(USBD_PULLUP_PORT, USBD_PULLUP_PIN, GPIO_OUTPP);
```

###对外设编号，并且通过编号来访问

一般，访问外设的接口的第一个参数，为这个外设的序号，序号和实际MCU上的外设资源的对应关系，由该MCU驱动开发人员确定，并且提供相应的文档。对于一个特定的硬件，板级支持包（BSP）就包含了使用的所有外设的序号定义。以下实现方式只是推荐，并非强制要求。

hw_cfg_XXXX_Board.h:
``` c
#define OSC_HZ							((uint32_t)8000000)
#define USB_PULLUP_PORT					0
#define USB_PULLUP_PIN					15
```

对应的.c文件里：
``` c
struct bsp_hwcfg_t
{
	struct
	{
		struct interface_gpio_pin_t pullup;
	} usbd;
	// other hardware configurations
} static bsp_hwcfg =
{
	.usbd.pullup.port = USB_PULLUP_PORT,
	.usbd.pullup.pin = USB_PULLUP_PIN,
};
```

应用代码中，通过bsp模块指定的序号和参数，来对相应的外设访问。
``` c
vsfhal_gpio_init(bsp_hwcfg.usbd.pullup.port);
vsfhal_gpio_clear(bsp_hwcfg.usbd.pullup.port, 1 << bsp_hwcfg.usbd.pullup.pin);
vsfhal_gpio_config_pin(bsp_hwcfg.usbd.pullup.port, bsp_hwcfg.usbd.pullup.pin, GPIO_OUTPP);
```
由于硬件访问接口是通用的，而对于特定的硬件实现，通过硬件配置文件定义好了需要使用的外设接口和参数。这样，应用代码就不需要针对不同的硬件而修改，需要修改的只是硬件配置文件，从而应用代码可以屏蔽硬件实现。

另外，外设序号也决定了外设的GPIO引脚映射，这里也举个实际驱动的例子。GD32的IIC引脚可以在多个GPIO引脚上配置，不同的配置组合，对应一个IIC的外设序号。
``` c
struct i2c_param_t
{
	I2C_TypeDef *reg;
	struct gd32f1x0_afio_pin_t sda[3];
	struct gd32f1x0_afio_pin_t scl[3];
} static const gd32f1x0_i2c_param[IIC_NUM] = 
{
	{
		I2C1,
		{
			{0, 10, 4},		// PA10, AF4
			{1, 7, 1},		// PB7, AF1
			{1, 9, 1},		// PB9, AF1
		},
		{
			{0, 9, 4},		// PA9, AF4
			{1, 6, 1},		// PB6, AF1
			{1, 8, 1},		// PB8, AF1
		},
	},
	{
		I2C2,
		{
			{0, 1, 4},		// PA1, AF4
			{1, 11, 1},		// PB11, AF1
			{6, 7, -1},		// PF7, default
		},
		{
			{0, 0, 4},		// PA0, AF4
			{1, 10, 1},		// PB10, AF1
			{6, 6, -1},		// PF6, default
		},
	},
};

// 2-bit iic_idx, 3-bit sda_idx, 3-bit scl_idx
vsf_err_t gd32f1x0_i2c_init(uint8_t index)
{
	uint8_t iic_idx = (index & 0x03) >> 0;
	uint8_t sda_idx = (index & 0x1C) >> 2;
	uint8_t scl_idx = (index & 0xE0) >> 5;
	I2C_TypeDef *reg = gd32f1x0_i2c_param[iic_idx].reg;

	gd32f1x0_afio_config(&gd32f1x0_i2c_param[iic_idx].sda[sda_idx], gd32f1x0_GPIO_AF | gd32f1x0_GPIO_OD);
	gd32f1x0_afio_config(&gd32f1x0_i2c_param[iic_idx].scl[scl_idx], gd32f1x0_GPIO_AF | gd32f1x0_GPIO_OD);
	
	RCC->APB1CCR |= RCC_APB1CCR_I2C1EN << iic_idx;
	reg->CTLR1 = 0;
	return VSFERR_NONE;
}
```
在GD32的IIC驱动里，定义IIC的外设序号包含了IIC的引脚映射信息。所以，用户简单设置一个序号，就可以确定使用不同的IIC引脚了。

###接口都是非阻塞的，提供轮询和中断回调2中方式

由于系统构架的不同，系统可能运行在轮询模式下，也可能在中断模式下（前后台系统）。对于这2种系统，接口上会有一些区别。比如，串口应用在轮询模式下，需要轮询收发完成标志；而在中断回调模式下，只需要在收发完成后，调用相应的回调接口（这种模式类似于硬件的中断）。这2种模式，在实际应用中，都可能会碰到，我以USBD的实现来举例。

一般，如果MCU自带USBD外设，那么完全可以使用中断模式，当然，使用轮询也可以。但是，如果MCU不带USBD外设，需要使用GPIO来模拟的话，这个就需要严格的时序确定性，甚至最新的ARM芯片的tail-chaining功能，都会使得时序不确定。这个时候，就需要使用轮询模式，整个系统只有一个中断用于模拟USBD，其他部分都工作在轮询模式下。

这里简单列举一下VSF中的代码。
对于系统1ms定时部分，一般如下实现：
``` c
static void app_tickclk_callback_int(void *param)
{
	vsftimer_callback_int();
}
......
vsfhal_tickclk_config_cb(app_tickclk_callback_int, NULL);
```
调用tickclk模块，并且配置回调函数的接口和参数，并且在回调函数里，调用定时器模块的回调接口以处理系统的各个定时器。以上代码一般是运行在中断模式，callback在中断里运行，如果在轮询模式下的话，增加如下代码（并且通过配置宏，关闭VSF的中断部分）：
``` c
void usrapp_poll(struct usrapp_t *app)
{
	vsfhal_tickclk_poll();
}
```
usrapp_poll是VSF构架配置为轮询模式时候的轮询代码。这里vsfhal_tickclk_poll就是轮询tickclk模块，如果1ms溢出，再调用之前设置的callback，callback不是在中断下运行。
``` c
void vsfhal_tickclk_poll()
{
	if (SysTick->CTRL & CM3_SYSTICK_COUNTFLAG)
		SysTick_Handler();
}
```
这里，SysTick_Handler就是中断接口，在轮询模式下，poll里的代码，会在定时器溢出的时候，调用SysTick_Handler。

###一些构架无关的基础模块

除了硬件控制接口外，还可以提供一些通用的构架无关的基础模块，比如fifo模块、stream模块等等。

####vsfstream -- 流控

vsfstream定义了一套不依赖内存实现的流控的接口，也就是说，可以在不同形式的内存结构上实现自动流控，免去用户代码自己实现流控。目前实现了有3种流，并且，用于也可以扩展其他满足特殊需求的流控，对于高层应用来说，调用接口和控制方式完全一样：

1. vsf_fifostream_t: 基于FIFO内存结构的流控。
2. vsf_mbufstream_t: 基于多缓冲内存结构的流控，一般用于流设备和块设备的转换，比如视频码流和视频帧块的互转。
3. vsf_bufstream_t: 基于普通内存块结构的流控。

##接口规范

TBD

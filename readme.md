#通用HAL -- halo

halo用来解决嵌入式MCU开发过程中，高层代码对底层硬件的依赖问题。通过实现通用的硬件抽象层（HAL），来实现对底层硬件的屏蔽。并且，halo实现了一些基本的系统无关的基础构件。halo可以作为其他实时操作系统（RTOS）或者状态机构架的硬件抽象层和基础框架层。

1. **快速开发**：应用程序开发用户，不需要熟悉目标MCU的各个外设寄存器，直接调用HAL中的通用接口。
2. **快速移植**：硬件BSP定义了和硬件相关的各个设置，应用程序直接更新这个设置，就可以实现应用的移植。
3. **GUI配置**：可以通过GUI来配置halo，并且，可以集成用户定义模块的配置功能。
4. ~~**兼容MISRA**：兼容最新的MISRA代码标准~~

--------------------------------

[TOC]

##简介

嵌入式MCU应用的开发，是在当前硬件限制条件下，采用最高效的开发方式。随着现在ARM等32位MCU的流行，系统资源和性能得到了非常大的丰富和提高。halo的设计，就是为了实现在不同的MCU上，提供同一套硬件访问接口，使得高层的代码可以硬件无关，提高开发和维护的效率。并且，代码都是构架无关的，高层应用可以使用各种不同的构架以及实时操作系统（RTOS）。

以下的示例代码，我使用vsfhal层的实现，这里也只是示例，并不代表最终的实现。

###多种外设访问方式

halo种实现了3种外设访问的方式，具体选用什么方式，由用户根据自己的应用需求，开发维护复杂度来自行选择。不同的访问方式各有特点，如果用户同时使用动态和静态的接口，用户需要控制资源不冲突（比如，应用中，模块A动态分配得到定时器0，模块B指定使用定时器0，这样对于定时器0的使用，就会有冲突）。

####基于函数访问外设（用于访问芯片自带的外设资源）：
``` c
vsfhal_gpio_init(USBD_PULLUP_PORT);
vsfhal_gpio_clear(USBD_PULLUP_PORT, 1 << USBD_PULLUP_PIN);
vsfhal_gpio_config_pin(USBD_PULLUP_PORT, USBD_PULLUP_PIN, GPIO_OUTPP);
```
这种方式适用于直接调用函数形式的驱动接口，并且第一个参数是对应的外设的序号。对于一些外设，序号还包括了外设引脚的复用定义，底层驱动会自动执行引脚复用的初始化，不需要高层介入。外设的序号，是通过图形化配置界面，自动生成的。

####基于接口访问外设（可以是芯片自带的，也可以是通过其他方式实现的）：
``` c
struct vsfusbd_device_t
{
	......
	struct interface_usbd_t *drv;
	......
};

static void vsfusbd_transact_out(struct vsfusbd_device_t *device,
								struct vsfusbd_transact_t *transact)
{
	uint8_t ep = transact->ep;
	struct interface_usbd_t *drv = device->drv;

	if (transact->idle)
	{
		uint16_t ep_size = drv->ep.get_OUT_epsize(ep);
		uint16_t pkg_size = min(ep_size, transact->data_size);
		uint32_t free_size = stream_get_free_size(transact->stream);

		transact->idle = free_size < pkg_size;
		if (!transact->idle)
		{
			drv->ep.enable_OUT(ep);
		}
	}
}
```
上述代码为usbd协议栈中的代码，usbd可以是芯片自带的外设，也可以是使用GPIO模拟的，甚至可以是使用一些通用接口（比如SPI）驱动扩展芯片实现的。通过接口访问外设，可以屏蔽接口的不同实现方式，对高层代码的影响。

基于接口的外设访问，在BSP层中，定义相应的外设的名字（操作函数等组成的数据结构），比如：IIC_EEPROM，表示链接EEPROM的IIC接口。调用的时候，使用IIC_EEPROM.Init(...)。这种方式，相当于是为第一种方式，加上了面向对象的封装。

####动态外设分配
这里就举几种适合的应用情况：
1. 定时器资源自动分配。高层代码只需要申请一个定时器资源，并且传入需要的定时器能力，驱动自动匹配适合的定时器。这种适合多个系统类似功能资源的访问管理。
2. 加密/Hash资源分配。有些芯片带CRC，当应用申请CRC的时候，得到硬件的接口。如果芯片不带CRC，得到的就是软件实现的CRC接口。对于高层应用，不需要关心CRC是硬件还是软件，当然，实际使用的时候，性能上会有差别。
3. 可动态重构资源分配。有些芯片会带有一定的硬件动态重构特性，高层可以申请需要重构的外设，底层自动重构。

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

通过编号，就可以访问对应的芯片外设。BSP里存放当前硬件使用的所有外设对应的编号，这样，就可以简单实现硬件配置文件，是的对于不同的工程，应用代码通用。芯片移植也只需要新建对应的工程，并且修改硬件配置文件，以符合实际硬件。

###图形化配置以及代码自动生成
图形化配置可以用来实现芯片和外部模块的功能配置，自动生成BSP硬件配置文件，以及模块的配置文件。图形化组件由xml定义，包含各种不同MCU组件、各种用户模块组件。

图形化配置的时候，用户可以自己加入各个组件，比如MCU组件（可以是特定的MCU芯片，也可以是虚拟的MCU）。然后，在各个组件之间建立关系，并且配置相应的参数。比如，把MCU组件的一路SPI连接到Dataflash组件，这种可以自动生成相应的硬件配置信息。当然，组件还可以包括软件组件，比如一个MP3编码组件，具备一个输入流和一个输出流，输入流链接到音频采样外设，输出流接到USB的Audio接口，然后自动生成对应的代码。

###接口都是非阻塞的，并且可以在轮询和中断方式中使用

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

###2种应用开发方式

用户可以自己选择，使用动态库的开发方式，应用开发会更加简单，也是推荐的方式。当然，如果用户需要更好的优化，以及一些定制化的功能，可以选择使用源代码编译开发。

####直接使用源代码开发

halo的各个功能都提供源代码，用户可以阅读各个模块的说明文档，自己配置模块的参数，直接从源代码编译出应用。

####使用动态库开发
这里对应用开发做了更好的隔离，应用由3个工程组成：应用功能（以及中间件）、BSP、底层的芯片驱动。3个工程可以独立编译，应用代码不依赖特定硬件的BSP和底层芯片驱动。底层芯片驱动可以直接使用编译好的lib库。

按照CMSIS的推广经验，一开始CMSIS提供的都是源代码，由用户自己加入到工程编译，结果，80%的反馈问题，都是和编译有关的。后来CMSIS使用库的方式，用户就基本上不会碰到编译的问题了。CMSIS的这个经验非常值得借鉴。

###一些构架无关的基础构件

除了硬件控制接口外，还可以提供一些通用的构架无关的基础模块，比如fifo模块、stream模块等等。

####vsfstream -- 流控

vsfstream定义了一套不依赖内存实现的流控的接口，也就是说，可以在不同形式的内存结构上实现自动流控，免去用户代码自己实现流控。目前实现了有3种流（流实现为面向对象，3种特定的流位基本流类型的子类型），并且，用于也可以扩展其他满足特殊需求的流控，对于高层应用来说，调用接口和控制方式完全一样：

1. vsf_fifostream_t: 基于FIFO内存结构的流控。
2. vsf_mbufstream_t: 基于多缓冲内存结构的流控，一般用于流设备和块设备的转换，比如视频码流和视频帧块的互转。
3. vsf_bufstream_t: 基于普通内存块结构的流控。

TBD

##接口规范

###core
``` c
struct interface_core_t
{
	vsf_err_t (*init)(void *p);
	vsf_err_t (*fini)(void *p);
	vsf_err_t (*reset)(void *p);
	uint32_t (*get_stack)(void);
	vsf_err_t (*set_stack)(uint32_t sp);
	void (*sleep)(uint32_t mode);
	vsf_err_t (*pendsv_config)(void (*on_pendsv)(void *), void *param);
	vsf_err_t (*pendsv_trigger)(void);
};
```

###Tick
``` c
struct interface_tickclk_t
{
	vsf_err_t (*init)(void);
	vsf_err_t (*fini)(void);
	void (*poll)(void);
	vsf_err_t (*start)(void);
	vsf_err_t (*stop)(void);
	uint32_t (*get_count)(void);
	vsf_err_t (*config_cb)(void (*)(void*), void*);
};
```

###GPIO
``` c
struct interface_gpio_t
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
	vsf_err_t (*init)(uint8_t index);
	vsf_err_t (*fini)(uint8_t index);
	vsf_err_t (*config_pin)(uint8_t index, uint8_t pin_idx, uint32_t mode);
	vsf_err_t (*config)(uint8_t index, uint32_t pin_mask, uint32_t io, uint32_t pull_en_mask, uint32_t input_pull_mask);
	vsf_err_t (*set)(uint8_t index, uint32_t pin_mask);
	vsf_err_t (*clear)(uint8_t index, uint32_t pin_mask);
	vsf_err_t (*out)(uint8_t index, uint32_t pin_mask, uint32_t value);
	vsf_err_t (*in)(uint8_t index, uint32_t pin_mask, uint32_t *value);
	uint32_t (*get)(uint8_t index, uint32_t pin_mask);
};
```

###SPI
``` c
struct interface_spi_t
{
#if IFS_CONST_EN
	struct
	{
		uint32_t MASTER;
		uint32_t SLAVE;
		uint32_t MODE0;
		uint32_t MODE1;
		uint32_t MODE2;
		uint32_t MODE3;
		uint32_t MSB_FIRST;
		uint32_t LSB_FIRST;
	} constants;
#endif
	vsf_err_t (*init)(uint8_t index);
	vsf_err_t (*fini)(uint8_t index);
	vsf_err_t (*get_ability)(uint8_t index, struct spi_ability_t *ability);
	vsf_err_t (*enable)(uint8_t index);
	vsf_err_t (*disable)(uint8_t index);
	vsf_err_t (*config)(uint8_t index, uint32_t kHz, uint32_t mode);
	vsf_err_t (*config_cb)(uint8_t index, uint32_t int_priority, void *p, void (*onready)(void *));

	vsf_err_t (*select)(uint8_t index, uint8_t cs);
	vsf_err_t (*deselect)(uint8_t index, uint8_t cs);

	vsf_err_t (*start)(uint8_t index, uint8_t *out, uint8_t *in, uint32_t len);
	uint32_t (*stop)(uint8_t index);
};
```

###IIC
``` c
struct interface_i2c_t
{
	vsf_err_t (*init)(uint8_t index);
	vsf_err_t (*fini)(uint8_t index);
	vsf_err_t (*config)(uint8_t index, uint16_t kHz);
	vsf_err_t (*config_cb)(uint8_t index, void *param, void (*cb)(void*, vsf_err_t));
	vsf_err_t (*xfer)(uint8_t index, uint16_t addr, struct interface_i2c_msg_t *msg, uint8_t msglen);
};
```

###Flash

###Timer

###Eint
``` c
struct interface_eint_t
{
#if IFS_CONST_EN
	struct
	{
		uint32_t ONFALL;
		uint32_t ONRISE;
		uint32_t ONLOW;
		uint32_t ONHIGH;
	} constants;
#endif
	vsf_err_t (*init)(uint32_t index);
	vsf_err_t (*fini)(uint32_t index);
	vsf_err_t (*config)(uint32_t index, uint32_t type, uint32_t int_priority, void *param, void (*callback)(void *param));
	vsf_err_t (*enable)(uint32_t index);
	vsf_err_t (*disable)(uint32_t index);
};
```

###USART
``` c
struct interface_usart_t
{
#if IFS_CONST_EN
	struct
	{
		uint32_t STOPBITS_1;
		uint32_t STOPBITS_1P5;
		uint32_t STOPBITS_2;
		uint32_t PARITY_NONE;
		uint32_t PARITY_ODD;
		uint32_t PARITY_EVEN;
	} constants;
#endif
	vsf_err_t (*init)(uint8_t index);
	vsf_err_t (*fini)(uint8_t index);
	vsf_err_t (*config)(uint8_t index, uint32_t baudrate, uint32_t mode);
	vsf_err_t (*config_cb)(uint8_t index, uint32_t int_priority, void *p, void (*ontx)(void *), void (*onrx)(void *, uint16_t));
	uint16_t (*tx_bytes)(uint8_t index, uint8_t *data, uint16_t size);
	uint16_t (*tx_get_free_size)(uint8_t index);
	uint16_t (*rx_bytes)(uint8_t index, uint8_t *data, uint16_t size);
	uint16_t (*rx_get_data_size)(uint8_t index);
};
```

###ADC
``` c
struct interface_adc_t
{
	vsf_err_t (*init)(uint8_t index);
	vsf_err_t (*fini)(uint8_t index);
	vsf_err_t (*config)(uint8_t index, uint32_t clock_hz, uint32_t mode);
	vsf_err_t (*config_channel)(uint8_t index, uint8_t channel, uint8_t cycles);
	uint32_t (*get_max_value)(uint8_t index);
	vsf_err_t (*calibrate)(uint8_t index, uint8_t channel);
	vsf_err_t (*start)(uint8_t index, uint8_t channel,
				 		void (callback)(void *, uint16_t), void *param);
};
```

###USBD

###USBDIO(GPIO emulated USBD)
``` c
struct interface_usbdio_t
{
	vsf_err_t (*init)(void (*onrx)(enum usbdio_evt_t evt, uint8_t *buf, uint16_t len));
	vsf_err_t (*fini)(void);
	vsf_err_t (*tx)(uint8_t *buf, uint16_t len);
};
```

###HCD(Host Controller driver, eg: OHCI, EHCI)
``` c
struct interface_hcd_t
{
	vsf_err_t (*init)(uint32_t index, vsf_err_t (*hcd_irq)(void *), void *param);
	vsf_err_t (*fini)(uint32_t index);
	void* (*regbase)(uint32_t index);
};
```

###PWM

###EBI

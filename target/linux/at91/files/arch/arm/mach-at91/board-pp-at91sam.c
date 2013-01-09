/*
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2008 Atmel
 *  Copyright (C) 2009 Raritan Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/mmc_spi.h>
#include <linux/clk.h>
#include <linux/mtd/physmap.h>
#include <linux/can/platform/mcp251x.h>

#include <mach/hardware.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/at91_shdwc.h>

#include "generic.h"
#include <mach/at91_features.h>
#if 0
#include <linux/pp.h>
#endif

#define HW_GPIO(gpio) (PIN_BASE + 0x20*gpio.bank + gpio.pin)

static const pp_hw_entry_t* get_hw_entry(void) {
    unsigned int i = 0;
    unsigned char hwid = 0x2; /* pp_get_opt_hardware_id(); */
    while (pp_at91_feature_table[i].hwid != 0xff) {
       if (pp_at91_feature_table[i].hwid == hwid) {
           return &pp_at91_feature_table[i];
       }
       i++;
    }
    printk("Could not determine hardware interface for hwid: %02x\n", hwid);
    return NULL;
}

static int have_hw_feature(unsigned int feature)
{
    const pp_hw_entry_t * hwe = get_hw_entry();
    return (hwe && (hwe->feature & feature) == feature) ? 1 : 0;
}

static inline int cascade_use_usart2(void) {
    return have_hw_feature(HW_FEAT_CASCADING_USART2);
}

static inline int cascade_use_usart3(void) {
    return have_hw_feature(HW_FEAT_CASCADING_USART3);
}

static inline int enable_mmc_slot_a(void) {
    return have_hw_feature(HW_FEAT_MMC_SLOT_A);
}

static inline int enable_mmc_slot_b(void) {
    return have_hw_feature(HW_FEAT_MMC_SLOT_B);
}

static inline int have_spi_flash(void) {
    return have_hw_feature(HW_FEAT_SPI_FLASH);
}

static inline int have_nor_flash(void) {
    return have_hw_feature(HW_FEAT_NOR_FLASH);
}

static int have_phy_rst_gpio(u8 *pin, u8 *neg_polarity) {
    const pp_hw_entry_t * hwe = get_hw_entry();
    if (hwe && (hwe->feature & HW_FEAT_PHY_RST_GPIO)) {
	if (pin) *pin = HW_GPIO(hwe->phy_rst_gpio);
	if (neg_polarity) *neg_polarity = 1; /* FIXME: determine from features */
	return 1;
    } 
    return 0;
}

static int have_rmii(void) {
    return have_hw_feature(HW_FEAT_RMII);
}

static void __init ek_map_io(void)
{
	/* Initialize processor: 12.000 MHz crystal */
	at91sam9260_initialize(12000000);

	/* DGBU on ttyS0, debug console. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}

static void __init ek_init_irq(void)
{
	at91sam9260_init_interrupts(NULL);
}

/*
 * NOR flash
 */

static struct physmap_flash_data nor_flash_data = {
	.width = 2,
};

static struct resource nor_flash_resource = {
	.flags = IORESOURCE_MEM,
	// start/end are filled in below
};

static struct platform_device nor_flash_device = {
	.name = "physmap-flash",
	.id = 0,
	.dev.platform_data = &nor_flash_data,
	.num_resources = 1,
	.resource = &nor_flash_resource,
};

static struct flash_platform_data spi_flash_platform_data = {
    .name = "phys"
};

static int __init
parse_opt_flash_start(char *arg)
{
	nor_flash_resource.start = memparse(arg, NULL);
	return 0;
}
early_param("pp_flash_start", parse_opt_flash_start);

static int __init
parse_opt_flash_end(char *arg)
{
	nor_flash_resource.end = memparse(arg, NULL);
	return 0;
}
early_param("pp_flash_end", parse_opt_flash_end);

/*
 * USB Host port
 */
static struct at91_usbh_data __initdata ek_usbh_data = {
	.ports		= 2,
};

/*
 * USB Device port
 */
static struct at91_udc_data __initdata ek_udc_data = {
	.vbus_pin	= AT91_PIN_PB21,
	.pullup_pin	= 0,		/* pull-up driven by UDC */
};


/*
 * SPI devices.
 */
#define MAX_SPI_DEVICES	5
static struct spi_board_info spi_device_flash = {
    /* SPI Flash on stamp, SPI0 */
    .modalias       = "s25sl12801",
    .bus_num        = 0,
    .chip_select    = 0,
    .max_speed_hz   = 12000000, /* 12 MHz */
    .platform_data  = &spi_flash_platform_data
};

#if defined(CONFIG_MMC_SPI) || defined(CONFIG_MMC_SPI_MODULE)
static struct mmc_spi_platform_data at91_mmc_spi_pdata = {
};

static struct spi_board_info spi_device_mmc = {
    .modalias = "mmc_spi",
    .max_speed_hz = 12000000,
    .bus_num = 0,
    .chip_select = 1,
    .platform_data = &at91_mmc_spi_pdata,
};
#endif

#if 1 /* def PP_CONFIG_ADE78XX */
static int have_ade78xx(void) {
#if 0
    unsigned char ctrl_hw_id = pp_get_opt_hardware_id();
    unsigned char base_board_hw_id = pp_get_opt_ext_hardware_id();
    return (ctrl_hw_id == 0x02 && base_board_hw_id == 0x03);
#endif
    return 1;
}

static struct spi_board_info spi_device_ade78xx = {
    .modalias		= "ade7854",
    .bus_num		= 1,
    .chip_select	= 0,
    .max_speed_hz	= 2500000 /* 2.5 MHz */
};
#endif

#ifdef PP_CONFIG_ADE7953
static int have_ade7953(void) {
    unsigned char ctrl_hw_id = pp_get_opt_hardware_id();
    unsigned char base_board_hw_id = pp_get_opt_ext_hardware_id();
    return (ctrl_hw_id == 0x02 && base_board_hw_id == 0x00);
}

static struct spi_board_info spi_device_ade7953 = {
    .modalias		= "ade7953",
    .bus_num		= 1,
    .chip_select	= 0,
    .max_speed_hz	= 5000000 /* 5 MHz */
};
#endif

static struct spi_board_info ek_spi_devices[MAX_SPI_DEVICES];
static unsigned int ek_spi_device_count = 0;
static void pp_at91_spi_add_device(const struct spi_board_info* spi_device)
{
    memcpy(&ek_spi_devices[ek_spi_device_count++],
           spi_device,
           sizeof(struct spi_board_info));
}

/*
 * SDIO devices
 */
static struct at91_mmc_data __initdata ek_mmc_data_a = {
	.wire4			= 1,
	.slot_b			= 0,
//	.det_pin		= ... not connected
#if 1
	.reset_pin		= AT91_PIN_PA5,
	.reset_active_low	= true,
#endif
//	.wp_pin			= ... not connected
//	.vcc_pin		= ... not connected
};

static struct at91_mmc_data __initdata ek_mmc_data_b = {
	.wire4			= 1,
	.slot_b			= 1,
//	.det_pin		= ... not connected
#if 1
	.reset_pin		= AT91_PIN_PA2,
	.reset_active_low	= true,
#endif
//	.wp_pin			= ... not connected
	.vcc_pin		= AT91_PIN_PC11,
#if 0
	.vcc_active_low		= true,
#endif
};

/*
 * MACB Ethernet device
 */
static struct at91_eth_data __initdata ek_macb_data = {
	.phy_irq_pin	= AT91_PIN_PC12,
#if 0
	.use_pa10_pa11	= 1,
#endif
};

/*
 * I2C Devices
 */
static struct i2c_board_info __initdata ek_i2c_devices[] = {
	{ I2C_BOARD_INFO("s35390a", 0x30) },	// Seiko S35390A RTC
};

static void __init ek_board_init(void)
{
	/* NOR flash */
	if (have_nor_flash()) {
	    platform_device_register(&nor_flash_device);
	}

	/* USART0 on ttyS1, modem port. (Rx, Tx, CTS, RTS, DTR, DSR, DCD, RI) */
	at91_register_uart(AT91SAM9260_ID_US0, 1, ATMEL_UART_CTS | ATMEL_UART_RTS
			   | ATMEL_UART_DTR | ATMEL_UART_DSR | ATMEL_UART_DCD
			   | ATMEL_UART_RI);

#define ATMEL_UART_RS485 0
	/* USART1 on ttyS2, internal RS485. (Rx, Tx, RTS) */
	at91_register_uart(AT91SAM9260_ID_US1, 2, ATMEL_UART_RTS | ATMEL_UART_RS485);

	if (cascade_use_usart3()) { /* cascading uart on usart3 introduced with V6 board */
	    /* USART3 on ttyS3, extension port (Rx, Tx) */
	    at91_register_uart(AT91SAM9260_ID_US3, 3, ATMEL_UART_RTS | ATMEL_UART_RS485);
	}
	if (cascade_use_usart2()) {
	    /* USART2 on ttyS3, cascade RS485. (Rx, Tx, RTS) */
	    at91_register_uart(AT91SAM9260_ID_US2, 3, ATMEL_UART_RTS | ATMEL_UART_RS485);
	}

	/* USART4 on ttyS4, Baytech/KVM/BLS port (Rx, Tx) */
	at91_register_uart(AT91SAM9260_ID_US4, 4, 0);

	/* Serial */
	at91_add_device_serial();
	/* USB Host */
	at91_add_device_usbh(&ek_usbh_data);
	/* USB Device */
	at91_add_device_udc(&ek_udc_data);
	/* SPI */
	printk("spi\n");
	if (have_spi_flash()) {
	    pp_at91_spi_add_device(&spi_device_flash);
	}
	printk("spi done\n");
#if defined(CONFIG_MMC_SPI) || defined(CONFIG_MMC_SPI_MODULE)
	pp_at91_spi_add_device(&spi_device_mmc);
#endif
#if 1 /* def PP_CONFIG_ADE78XX */
	if (have_ade78xx()) {
	    pp_at91_spi_add_device(&spi_device_ade78xx);
	}
#endif
#ifdef PP_CONFIG_ADE7953
	if (have_ade7953()) {
	    pp_at91_spi_add_device(&spi_device_ade7953);
	}
#endif
	if (ek_spi_device_count > 0) {
	    at91_add_device_spi(ek_spi_devices, ek_spi_device_count);
	}
	/* MMC */
	if (enable_mmc_slot_a()) {
	printk("slota\n");
	    at91_add_device_mmc(0, &ek_mmc_data_a);
	printk("slota done\n");
	}
	if (enable_mmc_slot_b()) {
	printk("slotb\n");
	    at91_add_device_mmc(0, &ek_mmc_data_b);
	printk("slotb done\n");
	}
	/* Ethernet */
	u8 pin, neg_polarity;
	if (have_phy_rst_gpio(&pin, &neg_polarity)) {
#if 0
	    ek_macb_data.phy_rst_pin = pin;
	    ek_macb_data.phy_rst_neg = neg_polarity;
#endif
	}
	ek_macb_data.is_rmii = have_rmii();
	at91_add_device_eth(&ek_macb_data);
	/* I2C -- bus and RTC chip */
	at91_add_device_i2c(ek_i2c_devices, ARRAY_SIZE(ek_i2c_devices));
}

// FIXME: Register our own mach type
MACHINE_START(SBC35_A9G20, "Raritan AT91SAM")
	/* Maintainer: Raritan */
	.boot_params	= AT91_SDRAM_BASE + 0x100,
	.timer		= &at91sam926x_timer,
	.map_io		= ek_map_io,
	.init_irq	= ek_init_irq,
	.init_machine	= ek_board_init,
MACHINE_END

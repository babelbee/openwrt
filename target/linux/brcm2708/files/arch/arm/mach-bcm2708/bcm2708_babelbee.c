#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/i2c/pcf857x.h>
#include <linux/gpio.h>
#include <linux/can/platform/mcp251x.h>
#include <linux/spi/spi_gpio.h>
#include <linux/leds.h>
#include <mach/gpio.h>

extern int bcm_register_device(struct platform_device *pdev);

void babelbee_gpio_init(void);


static struct platform_device babelbee_gpio_device = {
	.name = "babelbee-gpio",
	.id = -1,
};


static int
babelbee_pcf857x_setup(struct i2c_client *client, int gpio, unsigned int ngpio, void *context)
{
	int i;
	printk("Babelbee: pcf857x_setup(%p,%d,%d,%p)\n",client,gpio,ngpio,context);
	for (i = 0 ; i < 8 ; i++) {
		int gpio=BCM2708_NR_GPIOS+7-i;
		char name[16],*namep;
		sprintf(name,"rel%d%s",i/2,i&1 ? "off":"on");
		namep=kstrdup(name, GFP_KERNEL);
		gpio_request(gpio, namep);
		gpio_direction_output(gpio, 1);
		gpio_set_value_cansleep(gpio, 0);
		gpio_export(gpio, 0);
		gpio_export_link(&babelbee_gpio_device.dev, namep, gpio);
	}

        printk("Babelbee: HACK: enforcing late GPIO init\n");
	babelbee_gpio_init(); 

	return 0;
}


static struct i2c_board_info __initdata babelbee_i2c_devices1[] = {
	{
		I2C_BOARD_INFO("pcf8563", 0x51)
	},
};

static struct pcf857x_platform_data pcf_data = {
	.gpio_base      = BCM2708_NR_GPIOS,
	.setup          = babelbee_pcf857x_setup,
};

static struct i2c_board_info __initdata babelbee_i2c_devices2[] = {
	{
		I2C_BOARD_INFO("ds2482", 0x18)
	},
	{
		I2C_BOARD_INFO("pcf8574", 0x20),
		.platform_data = &pcf_data
	},
};

static struct mcp251x_platform_data mcp251x_info0 = {
	.oscillator_frequency = 20000000,
	.clock_flags = 4,
};

static struct mcp251x_platform_data mcp251x_info1 = {
	.oscillator_frequency = 20000000,
	.clock_flags = 4,
};

static struct spi_gpio_platform_data babelbee_spi_gpio_pdata = {
	.sck	    = 42,
	.mosi	   = 41,
	.miso	   = 40,
	.num_chipselect = 1,
};

static struct platform_device babelbee_spi_gpio = {
	.name	   = "spi_gpio",
	.id	     = 3,
	.dev	    = {
		.platform_data  = &babelbee_spi_gpio_pdata,
	},
};

static struct spi_board_info babelbee_spi_devices[] = {
	{	
		.modalias = "mcp2515",
		.platform_data = &mcp251x_info0,
		.bus_num	= 0,
		.chip_select = 1,
		.irq = gpio_to_irq(20),
		.max_speed_hz = 2*1000*1000,
	},
        {
 	        .modalias       = "ade7854",
	        .bus_num        = 3,
	        .chip_select    = 0,
	        .mode           = SPI_MODE_3,
	        .max_speed_hz   = 5000000, /* 5 MHz */
	        .controller_data = (void *)43,
        },
	{	
		.modalias = "mcp2515",
		.platform_data = &mcp251x_info1,
		.bus_num	= 0,
		.chip_select = 0,
		.irq = gpio_to_irq(19),
		.max_speed_hz = 2*1000*1000,
	},
};

static struct gpio_led babelbee_leds[] = {
	[0] = {
		.name = "blue",
		.gpio = 23,
		.active_low = 1,
	},
	[1] = {
		.name = "green",
		.gpio = 13,
		.active_low = 1,
	},
	[2] = {
		.name = "red",
		.gpio = 12,
		.active_low = 1,
	},
};

static struct gpio_led_platform_data babelbee_led_pdata = {
	.num_leds = ARRAY_SIZE(babelbee_leds),
	.leds = babelbee_leds,
};

static struct platform_device babelbee_led_device = {
	.name = "leds-gpio",
	.id = 1,
	.dev = {
		.platform_data = &babelbee_led_pdata,
		},
};

void
babelbee_gpio_init(void)
{
        printk("Babelbee: ADE7880 reset\n");
	/* ade reset */
	gpio_request(44, "ade_reset");
	gpio_direction_output(44, 0);
	gpio_request(18, "can_reset");
	gpio_direction_output(18, 0);

	gpio_request(21, "button1");
	gpio_direction_input(21);
	gpio_export(21, 0);
	gpio_export_link(&babelbee_gpio_device.dev, "button1", 21);
	gpio_request(22, "button2");
	gpio_direction_input(22);
	gpio_export(22, 0);
	gpio_export_link(&babelbee_gpio_device.dev, "button2", 22);
	printk("Babelbee: registered buttons\n");
	gpio_set_value_cansleep(44, 1);
	gpio_set_value_cansleep(18, 1);
}

void
babelbee_init(void)
{
        printk("Babelbee: registering devices\n");

	bcm_register_device(&babelbee_spi_gpio);
	bcm_register_device(&babelbee_gpio_device);
	i2c_register_board_info(0, babelbee_i2c_devices1, ARRAY_SIZE(babelbee_i2c_devices1));
	i2c_register_board_info(1, babelbee_i2c_devices2, ARRAY_SIZE(babelbee_i2c_devices2));
	spi_register_board_info(babelbee_spi_devices, ARRAY_SIZE(babelbee_spi_devices));
	platform_device_register(&babelbee_led_device);
	//	babelbee_gpio_init();
}


struct at91_gpio {
	int bank;
	int pin;
};

#define HW_FEAT_CASCADING_USART2 (1<<0)
#define HW_FEAT_CASCADING_USART3 (1<<1)
#define HW_FEAT_MMC_SLOT_A (1<<2)
#define HW_FEAT_MMC_SLOT_B (1<<3)
#define HW_FEAT_SPI_FLASH (1<<4)
#define HW_FEAT_NOR_FLASH (1<<5)
#define HW_FEAT_PHY_RST_GPIO (1<<6)
#define HW_FEAT_RMII (1<<7)

#define PP_CONFIG_ADE78XX 1

typedef struct {
	unsigned char hwid;
	unsigned int feature;
	struct at91_gpio phy_rst_gpio;
} pp_hw_entry_t;

static pp_hw_entry_t pp_at91_feature_table[]={
	{0x02,HW_FEAT_SPI_FLASH|HW_FEAT_RMII|HW_FEAT_MMC_SLOT_A},
	{0xff,},
};


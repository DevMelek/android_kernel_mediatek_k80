
/*----------------------------------------------------------------
* Author : Rubén Espínola (ruben1863@github.com)
* Contact : rubenes2003@gmail.com
* Supported device: Tecno POP 2F
* Reversed for Melek Saidani
* Copyright 2019 © Rubén Espínola
 *---------------------------------------------------------------*/

#include <mt-plat/mt_gpio.h>
#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------



#define FRAME_WIDTH                                         (960)
#define FRAME_HEIGHT                                        (480)


#define REGFLAG_DELAY               (0XFE)
#define REGFLAG_END_OF_TABLE        (0x00)


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V3(para_tbl,size,force_update)         lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define read_reg_v2(cmd, buffer, buffer_size)	            lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define write_regs(addr, pdata, byte_nums)	                lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define wrtie_cmd(cmd)	lcm_util.dsi_write_cmd(cmd)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[128];
};


static struct LCM_setting_table lcm_initialization_setting[] =
{
	{0xFF, 5, {0x77, 0x01, 0x00, 0x00, 0x10}},
	{0xC0, 2, {0xF7, 0x01}},
	{0xC1, 2, {0x12, 0x02}},
	{0xC2, 2, {0x07, 0x02}},
	{0xC6, 1, {0x21}},
	{0xCC, 1, {0x30}},
	{0xB0, 10, {0x00, 0x02, 0x0E, 0x18, 0x1F, 0x0F, 0x19, 0x0C, 0x08, 0x2D, 0x0C, 0x19, 0x12, 0x1A, 0x2F, 0x18}},
	{0xB1, 10, {0x00, 0x03, 0x09, 0x0D, 0x12, 0x07, 0x01, 0x08, 0x08, 0x18, 0x06, 0x18, 0x19, 0x0F, 0x2F, 0x18}},
	{0xFF, 5, {0x77, 0x01, 0x00, 0x00, 0x11}},
	{0xB0, 1, {0x42}},
	{0xB2, 1, {0x87}},
	{0xB3, 1, {0x80}},
	{0xB5, 1, {0x47}},
	{0xB7, 1, {0x85}},
	{0xB8, 1, {0x10}},
	{0xB9, 1, {0x10}},
	{0xC0, 1, {0x80}},
	{0xC1, 1, {0x78}},
	{0xC2, 1, {0x78}},
	{0xD0, 1, {0x88}},
	{0xE0, 3, {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
	{0xE1, 14, {0x0A, 0x96, 0x0C, 0x96, 0x0B, 0x96, 0x0D, 0x96, 0x00, 0x44, 0x44}},
	{0xE2, 13, {0x33, 0x33, 0x44, 0x44, 0xCF, 0x96, 0xD1, 0x96, 0xD0, 0x96, 0xD2, 0x96}},
	{0xE3, 4, {0x00, 0x00, 0x33, 0x33}},
	{0xE4, 2, {0x44, 0x44}},
	{0xE5, 10, {0x0C, 0xD0, 0x86, 0x86, 0x0E, 0xD2, 0x86, 0x86, 0x10, 0xD4, 0x86, 0x86, 0x12, 0xD6, 0x86, 0x86}},
	{0xE6, 4, {0x00, 0x00, 0x33, 0x33}},
	{0xE7, 2, {0x44, 0x44}},
	{0xE8, 10, {0x0D, 0xD1, 0x86, 0x86, 0x0F, 0xD3, 0x86, 0x86, 0x11, 0xD5, 0x86, 0x86, 0x13, 0xD7, 0x86, 0x86}},
	{0xEB, 7, {0x00, 0x01, 0x4E, 0x4E, 0xEE, 0x44}},
	{0xED, 10, {0xFF, 0xF1, 0x04, 0x56, 0x72, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x27, 0x65, 0x40, 0x1F, 0xFF}},
	{0xFF, 5, {0x77, 0x01, 0x00, 0x00, 0x11}},
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0X00, {}}
};
	
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = 
{
    { 0x28, 0x01, {0x00}},
    { REGFLAG_DELAY, 20, {0x00}},
    { 0x10, 0x01, {0x00}},
    { REGFLAG_DELAY, 120, {0x00}},
    { REGFLAG_END_OF_TABLE, 0x00, {0x00}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {
        
        unsigned cmd;
        cmd = table[i].cmd;
        
        switch (cmd) {
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
                
            case REGFLAG_END_OF_TABLE :
                break;
                
            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
    
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));	

	params->physical_width = 61;
	params->physical_height = 123;
	params->physical_width_um = 61880;
	params->type = 2;
	params->width = 480;
	params->height = 960;
	params->physical_height_um = 123750;
//	params->dsi.lane_swap_en = 6;
	params->dsi.vertical_active_line = 960;
	params->dsi.data_format.color_order = 0;
	params->dsi.data_format.trans_seq = 0;
	params->dsi.data_format.padding = 0;
	params->dsi.horizontal_backporch = 35;
	params->dsi.vertical_sync_active = 8;
	params->dsi.horizontal_active_pixel = 480;
	params->dsi.vertical_backporch = 20;
	params->dsi.vertical_frontporch = 20;
	params->dsi.PLL_CLOCK = 208;
	params->dsi.horizontal_sync_active = 10;
	params->dsi.ssc_range = 4;
	params->lcm_if = 1;
	params->lcm_cmd_if = 1;
	params->dsi.mode = 1;
	params->dsi.LANE_NUM = 2;
	params->dsi.data_format.format = 2;
	params->dsi.PS = 2;
	params->dsi.horizontal_frontporch = 30;
	params->dsi.ssc_disable = 1;
	params->dsi.HS_TRAIL = 15;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 10;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = -100;
	params->dsi.noncont_clock = 1;
	params->dsi.noncont_clock_period = 2;
}

static void lcm_init(void)
{
	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
	MDELAY(5);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO);
	MDELAY(10);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
	MDELAY(120);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO);
}

static void lcm_resume(void)
{
	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
	MDELAY(5);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO);
	MDELAY(10);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
	MDELAY(10);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static unsigned int lcm_compare_id(void)
{
	return 1;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER st7701s_fwvga_dsi_vdo_coe_ivo_b1_lcm_drv = 
{
    .name           = "st7701s_fwvga_dsi_vdo_coe_ivo_b1",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,   
    .compare_id     = lcm_compare_id,

};

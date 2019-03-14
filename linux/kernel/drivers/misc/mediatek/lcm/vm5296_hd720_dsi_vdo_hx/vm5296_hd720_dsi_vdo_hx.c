#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0XFFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

//#define LCM_DEBUG
#if defined(BUILD_LK)
	#if defined(BUILD_LK)
	#define LCM_LOG(fmt, args...)    printf(fmt, ##args)
	#else
	#define LCM_LOG(fmt, args...)    printk(fmt, ##args)	
	#endif
#else
#define LCM_LOG(fmt, args...)	 printk(fmt, ##args)	
#endif

#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg					 						lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	{0xB9, 3, {0xFF,0x52,0x96}},

	{0xD9, 1, {0x84}},

	{0xB1, 11, {0x02,0x02,0x72,0x92,0x01,0x32,0x33,0x51,0x51,0x57,0x4D}},

	{0xD2, 1, {0x88}},

	{0xB2, 16, {0x00,0x04,0x80,0xB4,0x01,0x06,0x5A,0x11,0x10,0x00,0x00,0x1F,0x70,0x03,0xC4,0x38}},

	{0xB4, 48, {0x00,0xFF,0xA0,0x12,0xA0,0x12,0x01,0xA6,0x01,0xA6,0x00,0x00,0x07,0x0D,0x00,0x1C,0x04,0x05,0x0B,0x11,0x22,0x04,0x00,0x00,0xAF,0x00,0xA0,0x12,0xA0,0x12,0x01,0xA6,0x01,0xA6,0x00,0x00,0x07,0x0D,0x00,0x1C,0x04,0x05,0x0B,0x04,0x00,0x00,0xAF,0x85}},

	{0xB6, 2, {0x8A,0x8A}},

	{0xBA, 4, {0x63,0x23,0x68,0x75}},

	{0xCC, 1, {0x0A}},

	{0xC0, 6, {0x7D,0x39,0x04,0x0F,0xE0,0x23}},

	{0xC7, 6, {0x00,0x08,0x00,0x00,0x00,0x22}},

	{0xD1, 19, {0x56,0x4B,0x07,0x9A,0x54,0x6B,0x27,0x98,0x04,0x04,0x08,0x04,0x04,0x08,0x80,0x7F,0x00,0x00,0x00}},

	{0xC1, 44, {0x01,0x00,0x00,0x06,0x0B,0x10,0x14,0x18,0x1B,0x1F,0x2A,0x32,0x3F,0x49,0x53,0x5B,0x63,0x6B,0x6C,0x74,0x7D,0x88,0x94,0xA1,0xB1,0xBD,0xCC,0xCF,0xD4,0xD8,0xDE,0xE5,0xEB,0xF7,0xFF,0x24,0x17,0x46,0xC6,0x8B,0x4B,0x8C,0xB3,0x40}},

	{0xBD, 1, {0x01}},

	{0xC1, 43, {0x00,0x00,0x06,0x0B,0x10,0x14,0x18,0x1B,0x1F,0x2A,0x32,0x3F,0x49,0x53,0x5B,0x63,0x6B,0x6C,0x74,0x7D,0x88,0x94,0xA1,0xB1,0xBD,0xCC,0xCF,0xD4,0xD8,0xDE,0xE5,0xEB,0xF7,0xFF,0x24,0x17,0x46,0xC6,0x8B,0x4B,0x8C,0xB3,0x40}},

	{0xBD, 1, {0x02}},

	{0xC1, 43, {0x00,0x00,0x06,0x0B,0x10,0x14,0x18,0x1B,0x1F,0x2A,0x32,0x3F,0x49,0x53,0x5B,0x63,0x6B,0x6C,0x74,0x7D,0x88,0x94,0xA1,0xB1,0xBD,0xCC,0xCF,0xD4,0xD8,0xDE,0xE5,0xEB,0xF7,0xFF,0x24,0x17,0x46,0xC6,0x8B,0x4B,0x8C,0xB3,0x40}},

	{0xBD, 1, {0x03}},

	{0xC1, 43, {0x00,0x00,0x06,0x0B,0x10,0x14,0x18,0x1B,0x1F,0x2A,0x32,0x3F,0x49,0x53,0x5B,0x63,0x6B,0x6C,0x74,0x7D,0x88,0x94,0xA1,0xB1,0xBD,0xCC,0xCF,0xD4,0xD8,0xDE,0xE5,0xEB,0xF7,0xFF,0x24,0x17,0x46,0xC6,0x8B,0x4B,0x8C,0xB3,0x40}},

	{0xBD, 1, {0x00}},

	{0xD3, 39, {0x00,0x08,0x03,0x03,0x00,0x00,0x04,0x04,0x32,0x10,0x00,0x00,0x00,0x32,0x13,0xC0,0x00,0x00,0x32,0x10,0x08,0x00,0x00,0x30,0x00,0x01,0x01,0x03,0x00,0x01,0x01,0x03,0x01,0x40,0x00,0x00,0x00,0x00,0x10}},
	{REGFLAG_DELAY, 5, {}},

	{0xD5, 36, {0x20,0x20,0x19,0x19,0x18,0x18,0x19,0x19,0x13,0x12,0x11,0x10,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x2F,0x2F,0x30,0x30,0x31,0x31,0x35,0x35,0x18,0x18}},
	{REGFLAG_DELAY, 5, {}},

	{0xD6, 36, {0x20,0x20,0x18,0x18,0x19,0x19,0x19,0x19,0x10,0x11,0x12,0x13,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x2F,0x2F,0x30,0x30,0x31,0x31,0x35,0x35,0x98,0x98}},
	{REGFLAG_DELAY, 5, {}},

	{0xD8, 20, {0x83,0xA0,0x00,0x2A,0x80,0x83,0xA0,0x00,0x2A,0x80,0x82,0xA0,0x00,0x2A,0x80,0x82,0xA0,0x00,0x2A,0x80}},

	{0xBD, 1, {0x01}},

	{0xD8, 10, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0xBD, 1, {0x02}},

	{0xD8, 10, {0xC3,0xF0,0x00,0x3F,0xC0,0xC3,0xF0,0x00,0x3F,0xC0}},

	{0xBD, 1, {0x00}},

	{0xDD, 1, {0x03}},

	{0xC6, 2, {0xFF,0xFE}},		

	{0x11, 0, {0x11}},
	{REGFLAG_DELAY, 120, {}},

	{0x29, 0, {0x29}},
	{REGFLAG_DELAY, 20, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}                                                         

};
/*
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 200, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 200, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned int cmd;
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
	
		params->type   = LCM_TYPE_DSI;
		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		params->dsi.mode   = SYNC_PULSE_VDO_MODE;

	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE; //LCM_TWO_LANE;

		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;		
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;	
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;		
		params->dsi.data_format.format	  = LCM_DSI_FORMAT_RGB888;
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;



		params->dsi.vertical_sync_active				= 2;
		params->dsi.vertical_backporch					= 8; //2--->8
		params->dsi.vertical_frontporch					= 8; //2--->8
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.horizontal_sync_active				= 84; //4--->14
		params->dsi.horizontal_backporch				= 84; //12--->24
		params->dsi.horizontal_frontporch				= 84; //14--->100
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 227;

		
}

static void lcm_init(void)
{

    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}

static void lcm_suspend(void)
{
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);//Must > 5ms
    SET_RESET_PIN(1);
    MDELAY(50);//Must > 50ms

		push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_resume(void)
{
	lcm_init();
	
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}  

static unsigned int lcm_compare_id(void)
{
#if 1
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];  
 	
	//Do reset here
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(50);       
  
	/*array[0]=0x00063902;
	array[1]=0x52aa55f0;
	array[2]=0x00000108;
	dsi_set_cmdq(array, 3, 1);
	MDELAY(10);*/

	array[0] = 0x00023700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04, buffer, 3);//if read 0x04,should get 0x008000,that is both OK.
	//read_reg_v2(0xc5, buffer,2);

	id = buffer[0]<<8 |buffer[1]; 
	      
#if defined(BUILD_LK)
	  printf("[vm5296]%s,  buffer[0]=%x,buffer[1]=%x,id = 0x%x \n", __func__,buffer[0],buffer[1], id);
#else
    printk("[vm5296]%s,  buffer[0]=%x,buffer[1]=%x,id = 0x%x \n", __func__,buffer[0],buffer[1], id);
#endif

    if(id == 0x5296)
    	return 1;
    else
    	return 0;
#else
	return 1;
#endif    	
}

LCM_DRIVER vm5296_hd720_dsi_vdo_hx_lcm_drv = 
{
  .name		= "vm5296_hd720_dsi_vdo_hx",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
    };

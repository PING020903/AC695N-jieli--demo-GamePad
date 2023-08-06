#ifndef CONFIG_BOARD_AC6953D_USB_H
#define CONFIG_BOARD_AC6953D_USB_H

#ifdef CONFIG_BOARD_AC6953D_USB

#define CONFIG_SDFILE_ENABLE
#define CONFIG_FLASH_SIZE       (1024 * 1024)

//*********************************************************************************//
//                                 配置开始                                        //
//*********************************************************************************//
#define ENABLE_THIS_MOUDLE					1
#define DISABLE_THIS_MOUDLE					0

#define ENABLE								1
#define DISABLE								0

#define NO_CONFIG_PORT						(-1)

//*********************************************************************************//
//                                 UART配置                                        //
//*********************************************************************************//
#define TCFG_UART0_ENABLE					ENABLE_THIS_MOUDLE
#define TCFG_UART0_RX_PORT					NO_CONFIG_PORT
#define TCFG_UART0_TX_PORT  				IO_PORTA_05
#define TCFG_UART0_BAUDRATE  				1000000

//*********************************************************************************//
//                                  SD 配置                                        //
//*********************************************************************************//
#define TCFG_SD0_ENABLE						DISABLE_THIS_MOUDLE
#define TCFG_SD0_PORTS						'B'
#define TCFG_SD0_DAT_MODE					1
#define TCFG_SD0_DET_MODE					0
#define TCFG_SD0_CLK						30000000

#define TCFG_SD1_ENABLE						DISABLE_THIS_MOUDLE
#define TCFG_SD1_PORTS						'A'
#define TCFG_SD1_DAT_MODE					1
#define TCFG_SD1_DET_MODE					0
#define TCFG_SD1_CLK						3000000

//*********************************************************************************//
//                                 USB 配置                                        //
//*********************************************************************************//
#define TCFG_PC_ENABLE						    ENABLE_THIS_MOUDLE//PC模块使能
#define TCFG_UDISK_ENABLE						DISABLE_THIS_MOUDLE//U盘模块使能
#define TCFG_MOUSE_ENABLE                       DISABLE_THIS_MOUDLE //鼠标dongle

#if TCFG_UDISK_ENABLE
#define MOUNT_RETRY                         10
#define MOUNT_RESET                         20
#define MOUNT_TIMEOUT                       50
#endif

#if (TCFG_PC_ENABLE&TCFG_UDISK_ENABLE)
#define TCFG_OTG_MODE                       (OTG_HOST_MODE|OTG_SLAVE_MODE)
#define TCFG_OTG_SLAVE_CNT                  20
#define TCFG_OTG_HOST_CNT                   20
#elif TCFG_PC_ENABLE
#define TCFG_OTG_MODE                       (OTG_SLAVE_MODE)
#define TCFG_OTG_SLAVE_CNT                  20
#define TCFG_OTG_HOST_CNT                   0
#elif   TCFG_UDISK_ENABLE
#define TCFG_OTG_MODE                       (OTG_HOST_MODE)
#define TCFG_OTG_HOST_CNT                   20
#define TCFG_OTG_SLAVE_CNT                  0
#else
#define TCFG_OTG_MODE                       0
#define TCFG_OTG_SLAVE_CNT                  0
#define TCFG_OTG_HOST_CNT                   0
#endif

#define TCFG_OTG_DET_INTERVAL               20



//*********************************************************************************//
//                                 key配置                                       //
//*********************************************************************************//
#define  KEY_NUM_MAX                        10
#define  KEY_NUM                            3

//iokey配置
#define TCFG_IOKEY_ENABLE					DISABLE_THIS_MOUDLE

#define TCFG_IOKEY_POWER_ENABLE				DISABLE_THIS_MOUDLE
#define TCFG_IOKEY_POWER_CONNECT_WAY		ONE_PORT_TO_LOW  //按键一端接低电平一端接IO
#define TCFG_IOKEY_POWER_ONE_PORT			IO_PORTB_01
#define TCFG_IOKEY_POWER_IN_PORT			NO_CONFIG_PORT
#define TCFG_IOKEY_POWER_OUT_PORT			NO_CONFIG_PORT

#define TCFG_IOKEY_PREV_ENABLE				DISABLE_THIS_MOUDLE
#define TCFG_IOKEY_PREV_CONNECT_WAY			ONE_PORT_TO_LOW  //按键一端接低电平一端接IO
#define TCFG_IOKEY_PREV_ONE_PORT			IO_PORTB_00
#define TCFG_IOKEY_PREV_IN_PORT				NO_CONFIG_PORT
#define TCFG_IOKEY_PREV_OUT_PORT			NO_CONFIG_PORT

#define TCFG_IOKEY_NEXT_ENABLE				DISABLE_THIS_MOUDLE
#define TCFG_IOKEY_NEXT_CONNECT_WAY 		ONE_PORT_TO_LOW  //按键一端接低电平一端接IO
#define TCFG_IOKEY_NEXT_ONE_PORT			IO_PORTB_02
#define TCFG_IOKEY_NEXT_IN_PORT				NO_CONFIG_PORT
#define TCFG_IOKEY_NEXT_OUT_PORT			NO_CONFIG_PORT

//ADKEY配置
#define TCFG_ADKEY_ENABLE                   ENABLE_THIS_MOUDLE
#define TCFG_ADKEY_PORT                     IO_PORTB_01
#define TCFG_ADKEY_AD_CHANNEL               AD_CH_PB1
#define TCFG_ADKEY_EXTERN_UP_ENABLE         ENABLE_THIS_MOUDLE

#if TCFG_ADKEY_EXTERN_UP_ENABLE     //是否使用外部上拉
#define R_UP    39                  //3.3K，外部上拉阻值在此自行设置
#else
#define R_UP    100                 //10K，内部上拉默认10K
#endif

//必须从小到大填电阻，没有则同VDDIO,填0x3ffL
#define TCFG_ADKEY_AD0      (0)                                 //0R
#define TCFG_ADKEY_AD1      (0x3ffL * 6   / (6   + R_UP))     //1.5k
#define TCFG_ADKEY_AD2      (0x3ffL * 10    / (10   + R_UP))     //2.4k
#define TCFG_ADKEY_AD3      (0x3ffL * 33   / (33   + R_UP))     //7.3k
#define TCFG_ADKEY_AD4      (0x3ffL * 150  / (150  + R_UP))     //15k
#define TCFG_ADKEY_AD5      (0x3ffL * 240  / (240  + R_UP))     //24k
#define TCFG_ADKEY_AD6      (0x3ffL * 330  / (330  + R_UP))     //33k
#define TCFG_ADKEY_AD7      (0x3ffL * 510  / (510  + R_UP))     //51k
#define TCFG_ADKEY_AD8      (0x3ffL * 1000 / (1000 + R_UP))     //100k
#define TCFG_ADKEY_AD9      (0x3ffL * 2200 / (2200 + R_UP))     //220k
#define TCFG_ADKEY_VDDIO    (0x3ffL)

#if 0
#define TCFG_ADKEY_VOLTAGE0 300//((TCFG_ADKEY_AD0 + TCFG_ADKEY_AD1) / 2)
#define TCFG_ADKEY_VOLTAGE1 400//((TCFG_ADKEY_AD1 + TCFG_ADKEY_AD2) / 2)
#define TCFG_ADKEY_VOLTAGE2 480//((TCFG_ADKEY_AD2 + TCFG_ADKEY_AD3) / 2)
#else
#define TCFG_ADKEY_VOLTAGE0 ((TCFG_ADKEY_AD0 + TCFG_ADKEY_AD1) / 2)
#define TCFG_ADKEY_VOLTAGE1 ((TCFG_ADKEY_AD1 + TCFG_ADKEY_AD2) / 2)
#define TCFG_ADKEY_VOLTAGE2 ((TCFG_ADKEY_AD2 + TCFG_ADKEY_AD3) / 2)
#endif
#define TCFG_ADKEY_VOLTAGE3 ((TCFG_ADKEY_AD3 + TCFG_ADKEY_AD4) / 2)
#define TCFG_ADKEY_VOLTAGE4 ((TCFG_ADKEY_AD4 + TCFG_ADKEY_AD5) / 2)
#define TCFG_ADKEY_VOLTAGE5 ((TCFG_ADKEY_AD5 + TCFG_ADKEY_AD6) / 2)
#define TCFG_ADKEY_VOLTAGE6 ((TCFG_ADKEY_AD6 + TCFG_ADKEY_AD7) / 2)
#define TCFG_ADKEY_VOLTAGE7 ((TCFG_ADKEY_AD7 + TCFG_ADKEY_AD8) / 2)
#define TCFG_ADKEY_VOLTAGE8 ((TCFG_ADKEY_AD8 + TCFG_ADKEY_AD9) / 2)
#define TCFG_ADKEY_VOLTAGE9 ((TCFG_ADKEY_AD9 + TCFG_ADKEY_VDDIO) / 2)

#define TCFG_ADKEY_VALUE0                   0
#define TCFG_ADKEY_VALUE1                   1
#define TCFG_ADKEY_VALUE2                   2
#define TCFG_ADKEY_VALUE3                   NO_KEY
#define TCFG_ADKEY_VALUE4                   NO_KEY
#define TCFG_ADKEY_VALUE5                   NO_KEY
#define TCFG_ADKEY_VALUE6                   NO_KEY
#define TCFG_ADKEY_VALUE7                   NO_KEY
#define TCFG_ADKEY_VALUE8                   NO_KEY
#define TCFG_ADKEY_VALUE9                   NO_KEY

//*********************************************************************************//
//                                 Audio配置                                       //
//*********************************************************************************//
#define TCFG_AUDIO_ADC_ENABLE				ENABLE_THIS_MOUDLE
//MIC只有一个声道，固定选择右声道
#define TCFG_AUDIO_ADC_MIC_CHA				LADC_CH_MIC_R
#define TCFG_AUDIO_ADC_LINE_CHA				LADC_LINE0_MASK
/*MIC LDO电流档位设置：
    0:1.25ua    1:2.5ua    2:3.75ua    3:5ua*/
#define TCFG_AUDIO_ADC_LD0_SEL				2

#define TCFG_AUDIO_DAC_ENABLE				ENABLE_THIS_MOUDLE
#define TCFG_AUDIO_DAC_LDO_SEL				1
/*
DACVDD电压设置(要根据具体的硬件接法来确定):
    DACVDD_LDO_1_20V        DACVDD_LDO_1_30V        DACVDD_LDO_2_35V        DACVDD_LDO_2_50V
    DACVDD_LDO_2_65V        DACVDD_LDO_2_80V        DACVDD_LDO_2_95V        DACVDD_LDO_3_10V*/
#define TCFG_AUDIO_DAC_LDO_VOLT			    DACVDD_LDO_2_90V//DACVDD_LDO_3_10V
/*预留接口，未使用*/
#define TCFG_AUDIO_DAC_PA_PORT				NO_CONFIG_PORT

#define TCFG_AUDIO_ENABLE   (TCFG_AUDIO_ADC_ENABLE|TCFG_AUDIO_DAC_ENABLE)
/*
DAC硬件上的连接方式,可选的配置：
    DAC_OUTPUT_MONO_L               左声道
    DAC_OUTPUT_MONO_R               右声道
    DAC_OUTPUT_LR                   立体声
    DAC_OUTPUT_MONO_LR_DIFF         单声道差分输出
    DAC_OUTPUT_DUAL_LR_DIFF         双差分输出
*/
#define TCFG_AUDIO_DAC_CONNECT_MODE         DAC_OUTPUT_DUAL_LR_DIFF

/*解码后音频的输出方式
    AUDIO_OUTPUT_ORIG_CH            按原始声道输出
    AUDIO_OUTPUT_STEREO             按立体声
    AUDIO_OUTPUT_L_CH               只输出原始声道的左声道
    AUDIO_OUTPUT_R_CH               只输出原始声道的右声道
    AUDIO_OUTPUT_MONO_LR_CH         输出左右合成的单声道
 */
#define AUDIO_OUTPUT_MODE                   AUDIO_OUTPUT_MONO_LR_CH
//*********************************************************************************//
//                                  充电仓配置                                     //
//*********************************************************************************//
#define TCFG_CHARGESTORE_ENABLE				DISABLE_THIS_MOUDLE
#define TCFG_TEST_BOX_ENABLE			    1
#define TCFG_CHARGESTORE_PORT				IO_PORTB_05
#define TCFG_CHARGESTORE_UART_ID			IRQ_UART1_IDX

//*********************************************************************************//
//                                  充电参数配置                                   //
//*********************************************************************************//
#define TCFG_CHARGE_ENABLE					DISABLE_THIS_MOUDLE
#define TCFG_CHARGE_POWERON_ENABLE			DISABLE
#define TCFG_CHARGE_OFF_POWERON_NE			DISABLE
#define TCFG_CHARGE_FULL_V					CHARGE_FULL_V_4202
#define TCFG_CHARGE_FULL_MA					CHARGE_FULL_mA_10
#define TCFG_CHARGE_MA						CHARGE_mA_60

//*********************************************************************************//
//                                  LED 配置                                       //
//*********************************************************************************//
#define TCFG_PWMLED_ENABLE					DISABLE_THIS_MOUDLE
#define TCFG_PWMLED_PORT					IO_PORTB_06
#define TCFG_REDLED_LIGHT					1   //1 ~ 10, value越大, (红灯)亮度越高
#define TCFG_BLUELED_LIGHT					1   //1 ~ 10, value越大, (蓝灯)亮度越高
#define TCFG_SINGLE_SLOW_FLASH_FREQ			3	//1 ~ 8, value越大, LED单独慢闪速度越慢, value * 0.5s闪烁一次
#define TCFG_SINGLE_FAST_FLASH_FREQ			4	//1 ~ 4, value越大, LED单独快闪速度越慢, value * 100ms闪烁一次
#define TCFG_DOUBLE_SLOW_FLASH_FREQ			3	//1 ~ 8, value越大, LED交替慢闪速度越慢, value * 0.5s闪烁一次
#define TCFG_DOUBLE_FAST_FLASH_FREQ			4	//1 ~ 4, value越大, LED交替快闪速度越慢, value * 100ms闪烁一次

//*********************************************************************************//
//                                  时钟配置                                       //
//*********************************************************************************//
#define TCFG_CLOCK_SYS_SRC					SYS_CLOCK_INPUT_PLL_BT_OSC   //系统时钟源选择
#define TCFG_CLOCK_SYS_HZ					24000000                     //系统时钟设置
#define TCFG_CLOCK_OSC_HZ					24000000                     //外界晶振频率设置
#define TCFG_CLOCK_MODE                     CLOCK_MODE_ADAPTIVE

//*********************************************************************************//
//                                  低功耗配置                                     //
//*********************************************************************************//
#define TCFG_LOWPOWER_POWER_SEL				PWR_LDO15
#define TCFG_LOWPOWER_BTOSC_DISABLE			0
#define TCFG_LOWPOWER_LOWPOWER_SEL			0//SLEEP_EN
#define TCFG_LOWPOWER_VDDIOM_LEVEL			VDDIOM_VOL_32V
#define TCFG_LOWPOWER_VDDIOW_LEVEL			VDDIOW_VOL_28V

//*********************************************************************************//
//                                  EQ配置                                         //
//*********************************************************************************//
//EQ配置，使用在线EQ时，EQ文件和EQ模式无效。有EQ文件时，默认不用EQ模式切换功能
#define TCFG_EQ_ENABLE                            0
#define TCFG_EQ_ONLINE_ENABLE                     0     //支持在线EQ调试
#define TCFG_BT_MUSIC_EQ_ENABLE                   0     //支持蓝牙音乐EQ
#define TCFG_PHONE_EQ_ENABLE                      0     //支持通话近端EQ

#define TCFG_HW_SOFT_EQ_ENABLE                    0     //支持使用软硬件混合EQ


// ONLINE CCONFIG
#define TCFG_ONLINE_ENABLE                        (TCFG_EQ_ONLINE_ENABLE)
#define TCFG_ONLINE_TX_PORT						  IO_PORT_DP
#define TCFG_ONLINE_RX_PORT						  IO_PORT_DM


//*********************************************************************************//
//                                  g-sensor配置                                   //
//*********************************************************************************//
#define TCFG_GSENSOR_ENABLE                       0     //gSensor使能
#define TCFG_DA230_EN                             0

//*********************************************************************************//
//                                  系统配置                                         //
//*********************************************************************************//
#define TCFG_AUTO_SHUT_DOWN_TIME		          0   //没有蓝牙连接自动关机时间
#define TCFG_SYS_LVD_EN						      0   //电量检测使能
#define TCFG_POWER_ON_NEED_KEY				      0	  //是否需要按按键开机配置

//*********************************************************************************//
//                                  蓝牙配置                                       //
//*********************************************************************************//
#define TCFG_USER_TWS_ENABLE                      0   //tws功能使能
#define TCFG_USER_BLE_ENABLE                      0   //BLE功能使能

#define USER_SUPPORT_PROFILE_SPP    0
#define USER_SUPPORT_PROFILE_HFP    1
#define USER_SUPPORT_PROFILE_A2DP   1
#define USER_SUPPORT_PROFILE_AVCTP  1
#define USER_SUPPORT_PROFILE_HID    1
#define USER_SUPPORT_PROFILE_PNP    1
#define USER_SUPPORT_PROFILE_PBAP   0


#if(TCFG_USER_TWS_ENABLE || TCFG_USER_BLE_ENABLE)
#define TCFG_BD_NUM						          1   //连接设备个数配置
#define TCFG_AUTO_STOP_PAGE_SCAN_TIME             0   //配置一拖二第一台连接后自动关闭PAGE SCAN的时间(单位分钟)
#else
#define TCFG_BD_NUM						          1   //连接设备个数配置
#define TCFG_AUTO_STOP_PAGE_SCAN_TIME             0 //配置一拖二第一台连接后自动关闭PAGE SCAN的时间(单位分钟)
#endif

#define BT_INBAND_RINGTONE                        0   //是否播放手机自带来电铃声
#define BT_PHONE_NUMBER                           0   //是否播放来电报号
#define BT_SUPPORT_DISPLAY_BAT                    0   //是否使能电量检测
#define BT_SUPPORT_MUSIC_VOL_SYNC                 0   //是否使能音量同步


//*********************************************************************************//
//                                 配置结束                                        //
//*********************************************************************************//

#endif //CONFIG_BOARD_DEMO_V1_0
#endif //CONFIG_BOARD_DEMO_H

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * 系统打印总开关
 */


#ifdef CONFIG_RELEASE_ENABLE
#define LIB_DEBUG    1
#else
#define LIB_DEBUG    1
#endif
#define CONFIG_DEBUG_LIB(x)         (x & LIB_DEBUG)

#define CONFIG_DEBUG_ENABLE

#ifndef CONFIG_DEBUG_ENABLE
//#define CONFIG_DEBUG_LITE_ENABLE  //轻量级打印开关, 默认关闭
#endif


#include "board_config.h"

#include "usb_common_def.h"

#include "btcontroller_mode.h"

#include "user_cfg_id.h"

#ifdef CONFIG_SDFILE_ENABLE

#define SDFILE_DEV				"sdfile"
#define SDFILE_MOUNT_PATH     	"mnt/sdfile"

#if (USE_SDFILE_NEW)
#define SDFILE_APP_ROOT_PATH       	SDFILE_MOUNT_PATH"/app/"  //app分区
#define SDFILE_RES_ROOT_PATH       	SDFILE_MOUNT_PATH"/res/"  //资源文件分区
#else
#define SDFILE_RES_ROOT_PATH       	SDFILE_MOUNT_PATH"/C/"
#endif

#endif


//*********************************************************************************//
//                                 测试模式配置                                    //
//*********************************************************************************//
#if (CONFIG_BT_MODE != BT_NORMAL)

#undef TCFG_BD_NUM
#define TCFG_BD_NUM						          1

#undef TCFG_USER_TWS_ENABLE
#define TCFG_USER_TWS_ENABLE                      0     //tws功能使能

#undef  TCFG_USER_BLE_ENABLE
#define TCFG_USER_BLE_ENABLE                      1     //BLE功能使能

#undef TCFG_AUTO_SHUT_DOWN_TIME
#define TCFG_AUTO_SHUT_DOWN_TIME		          0

#undef TCFG_SYS_LVD_EN
#define TCFG_SYS_LVD_EN						      0

#undef TCFG_LOWPOWER_LOWPOWER_SEL
#define TCFG_LOWPOWER_LOWPOWER_SEL                0

#undef TCFG_AUDIO_DAC_LDO_VOLT
#define TCFG_AUDIO_DAC_LDO_VOLT				DACVDD_LDO_2_65V

#undef TCFG_LOWPOWER_POWER_SEL
#define TCFG_LOWPOWER_POWER_SEL				PWR_LDO15

#undef  TCFG_PWMLED_ENABLE
#define TCFG_PWMLED_ENABLE					DISABLE_THIS_MOUDLE

#undef TCFG_ADKEY_ENABLE
#define TCFG_ADKEY_ENABLE                   DISABLE_THIS_MOUDLE

#undef TCFG_IOKEY_ENABLE
#define TCFG_IOKEY_ENABLE					DISABLE_THIS_MOUDLE

#undef TCFG_TEST_BOX_ENABLE
#define TCFG_TEST_BOX_ENABLE			    0

#undef TCFG_AUTO_SHUT_DOWN_TIME
#define TCFG_AUTO_SHUT_DOWN_TIME		          0

#undef TCFG_POWER_ON_NEED_KEY
#define TCFG_POWER_ON_NEED_KEY				      0

// #undef TCFG_UART0_ENABLE
// #define TCFG_UART0_ENABLE					DISABLE_THIS_MOUDLE

#endif //(CONFIG_BT_MODE != BT_NORMAL)


#if TCFG_USER_TWS_ENABLE

//*********************************************************************************//
//                                 对耳配置方式配置                                    //
//*********************************************************************************//
#define CONFIG_TWS_CONNECT_SIBLING_TIMEOUT    4    /* 开机或超时断开后对耳互连超时时间，单位s */
#define CONFIG_TWS_REMOVE_PAIR_ENABLE              /* 不连手机的情况下双击按键删除配对信息 */
#define CONFIG_TWS_POWEROFF_SAME_TIME         1    /*按键关机时两个耳机同时关机*/

#define ONE_KEY_CTL_DIFF_FUNC                 1    /*通过左右耳实现一个按键控制两个功能*/
#define CONFIG_TWS_SCO_ONLY_MASTER			  0	   /*通话的时候只有主机出声音*/

/* 配对方式选择 */
#define CONFIG_TWS_PAIR_BY_CLICK            0      /* 按键发起配对 */
#define CONFIG_TWS_PAIR_BY_AUTO             1      /* 开机自动配对 */
#define CONFIG_TWS_PAIR_BY_FAST_CONN        2      /* 开机快速连接,连接速度比自动配对快,不支持取消配对操作 */
#define CONFIG_TWS_PAIR_MODE                CONFIG_TWS_PAIR_BY_CLICK//CONFIG_TWS_PAIR_BY_AUTO


/* 声道确定方式选择 */
#define CONFIG_TWS_MASTER_AS_LEFT             0 //主机作为左耳
#define CONFIG_TWS_AS_LEFT_CHANNEL            1 //固定左耳
#define CONFIG_TWS_AS_RIGHT_CHANNEL           2 //固定右耳
#define CONFIG_TWS_LEFT_START_PAIR            3 //双击发起配对的耳机做左耳
#define CONFIG_TWS_RIGHT_START_PAIR           4 //双击发起配对的耳机做右耳
#define CONFIG_TWS_EXTERN_UP_AS_LEFT          5 //外部有上拉电阻作为左耳
#define CONFIG_TWS_EXTERN_DOWN_AS_LEFT        6 //外部有下拉电阻作为左耳
#define CONFIG_TWS_SECECT_BY_CHARGESTORE      7 //充电仓决定左右耳
#define CONFIG_TWS_CHANNEL_SELECT             CONFIG_TWS_LEFT_START_PAIR //配对方式选择

#define CONFIG_TWS_CHANNEL_CHECK_IO           IO_PORTA_07					//上下拉电阻检测引脚


#if CONFIG_TWS_PAIR_MODE != CONFIG_TWS_PAIR_BY_CLICK
#if (CONFIG_TWS_CHANNEL_SELECT == CONFIG_TWS_LEFT_START_PAIR) ||\
    (CONFIG_TWS_CHANNEL_SELECT == CONFIG_TWS_RIGHT_START_PAIR)
#undef CONFIG_TWS_CHANNEL_SELECT
#define CONFIG_TWS_CHANNEL_SELECT             CONFIG_TWS_MASTER_AS_LEFT
#endif

#if CONFIG_TWS_PAIR_MODE == CONFIG_TWS_PAIR_BY_AUTO
#define CONFIG_TWS_AUTO_PAIR_WITHOUT_UNPAIR     /* 不取消配对也可以配对新的耳机 */
#endif

#if CONFIG_TWS_PAIR_MODE == CONFIG_TWS_PAIR_BY_FAST_CONN
#undef CONFIG_TWS_REMOVE_PAIR_ENABLE
#endif

#endif




#if TCFG_CHARGESTORE_ENABLE
#undef CONFIG_TWS_CHANNEL_SELECT
#define CONFIG_TWS_CHANNEL_SELECT             CONFIG_TWS_SECECT_BY_CHARGESTORE	//充电仓区分左右
#endif //TCFG_CHARGESTORE_ENABLE

#if TCFG_TEST_BOX_ENABLE && (!TCFG_CHARGESTORE_ENABLE)
#define CONFIG_TWS_SECECT_CHARGESTORE_PRIO    1 //测试盒配置左右耳优先
#else
#define CONFIG_TWS_SECECT_CHARGESTORE_PRIO    0
#endif //TCFG_TEST_BOX_ENABLE

//*********************************************************************************//
//                                 对耳电量显示方式                                //
//*********************************************************************************//

#if BT_SUPPORT_DISPLAY_BAT
#define CONFIG_DISPLAY_TWS_BAT_LOWER          1 //对耳手机端电量显示，显示低电量耳机的电量
#define CONFIG_DISPLAY_TWS_BAT_HIGHER         2 //对耳手机端电量显示，显示高电量耳机的电量
#define CONFIG_DISPLAY_TWS_BAT_LEFT           3 //对耳手机端电量显示，显示左耳的电量
#define CONFIG_DISPLAY_TWS_BAT_RIGHT          4 //对耳手机端电量显示，显示右耳的电量

#define CONFIG_DISPLAY_TWS_BAT_TYPE           CONFIG_DISPLAY_TWS_BAT_LOWER
#endif //BT_SUPPORT_DISPLAY_BAT

#define CONFIG_DISPLAY_DETAIL_BAT             0 //BLE广播显示具体的电量
#define CONFIG_NO_DISPLAY_BUTTON_ICON         1 //BLE广播不显示按键界面,智能充电仓置1

#endif //TCFG_USER_TWS_ENABLE


#define CONFIG_BT_RX_BUFF_SIZE  (12 * 1024)
#define CONFIG_BT_TX_BUFF_SIZE  (3 * 1024)

#define CONFIG_TWS_BULK_POOL_SIZE  (0 * 1024)


#if (CONFIG_BT_MODE != BT_NORMAL)
////bqb 如果测试3M tx buf 最好加大一点
#undef  CONFIG_BT_TX_BUFF_SIZE
#define CONFIG_BT_TX_BUFF_SIZE  (6 * 1024)

#endif
//*********************************************************************************//
//                                 电源切换配置                                    //
//*********************************************************************************//

#define PHONE_CALL_USE_LDO15	CONFIG_PHONE_CALL_USE_LDO15

//*********************************************************************************//
//                                 时钟切换配置                                    //
//*********************************************************************************//

#define BT_NORMAL_HZ	            CONFIG_BT_NORMAL_HZ
#define BT_CONNECT_HZ               CONFIG_BT_CONNECT_HZ

#define BT_A2DP_HZ	        	    CONFIG_BT_A2DP_HZ

#define BT_CALL_HZ		            CONFIG_BT_CALL_HZ
#define BT_CALL_ADVANCE_HZ          CONFIG_BT_CALL_ADVANCE_HZ
#define BT_CALL_16k_HZ	            CONFIG_BT_CALL_16k_HZ
#define BT_CALL_16k_ADVANCE_HZ      CONFIG_BT_CALL_16k_ADVANCE_HZ


#define MUSIC_DEC_FASTEST_CLOCK		CONFIG_MUSIC_DEC_FASTEST_CLOCK
#define MUSIC_DEC_FAST_CLOCK		CONFIG_MUSIC_DEC_FAST_CLOCK
#define MUSIC_DEC_CLOCK			    CONFIG_MUSIC_DEC_CLOCK
// #define MUSIC_IDLE_CLOCK		    CONFIG_MUSIC_IDLE_CLOCK
#define MUSIC_FSCAN_CLOCK		    CONFIG_MUSIC_FSCAN_CLOCK
#define LINEIN_CLOCK				CONFIG_LINEIN_CLOCK
#define FM_CLOCK			    	CONFIG_FM_CLOCK
#define FM_EMITTER_CLOCK	    	CONFIG_FM_EMITTER_CLOCK
#define PC_CLOCK					CONFIG_PC_CLOCK
#define RECODRD_CLOCK				CONFIG_RECORD_CLOCK
#define SPDIF_CLOCK			    	CONFIG_SPDIF_CLOCK


#ifdef CONFIG_FPGA_ENABLE

#undef TCFG_CLOCK_OSC_HZ
#define TCFG_CLOCK_OSC_HZ		12000000

#endif



#define  CONFIG_CLOCK_BY_TYPE        0//  是否根据解码类型，配置对应的时钟

#ifdef CONFIG_CPU_BR25
#define CONFIG_MAX_CLK                 (192*1000000L)
#else
#define CONFIG_MAX_CLK                 (240*1000000L)
#endif
#define SUPPORT_SYS_CLK0               (24*1000000L)
#define SUPPORT_SYS_CLK1               (32*1000000L)
#define SUPPORT_SYS_CLK2               (48*1000000L)
#define SUPPORT_SYS_CLK3               (64*1000000L)
#define SUPPORT_SYS_CLK4               (96*1000000L)
#define SUPPORT_SYS_CLK5               (100*1000000L)
#define SUPPORT_SYS_CLK6               (120*1000000L)
#define SUPPORT_SYS_CLK7               (160*1000000L)
#define SUPPORT_SYS_CLK8               (192*1000000L)
#define SUPPORT_SYS_CLK9               (CONFIG_MAX_CLK)

#if TCFG_EQ_ONLINE_ENABLE
#define CONFIG_EQ_ONLINE_CLK         (192*1000000L)
#define CONFIG_EQ_EN_CLK             (0)
#define CONFIG_DRC_EN_CLK            (0)
#else
#define CONFIG_EQ_ONLINE_CLK         (0)
#define CONFIG_EQ_EN_CLK             (32*1000000L)
#define CONFIG_DRC_EN_CLK            (CONFIG_EQ_EN_CLK + 32*1000000L)
#endif

#define  CONFIG_UDISK_CLK                (8*1000000L)


#if (TCFG_EQ_ENABLE == 0)
#undef  CONFIG_EQ_EN_CLK
#define CONFIG_EQ_EN_CLK             (0)

#undef  CONFIG_EQ_ONLINE_CLK
#define CONFIG_EQ_ONLINE_CLK         (0)
#endif

#if (TCFG_DRC_ENABLE == 0)
#undef  CONFIG_DRC_EN_CLK
#define CONFIG_DRC_EN_CLK            (0)
#endif

#if (TCFG_UDISK_ENABLE == 0)
#undef   CONFIG_UDISK_CLK
#define  CONFIG_UDISK_CLK            (0)
#endif
#if (TCFG_USER_EMITTER_ENABLE == 0)
#define  CONFIG_SBC_ENC_CLK      (0)
#else
#define  CONFIG_SBC_ENC_CLK      (16*1000000L)
#endif

#if (defined(TCFG_MIXERCH_REC_EN) && (TCFG_MIXERCH_REC_EN))
#define CONFIG_MIXERCH_RECORD_CLK				(48*1000000L)
#else
#define CONFIG_MIXERCH_RECORD_CLK				(0)
#endif


#define CONFIG_LOCAL_TWS_SYNC_CLK			(16*1000000L)

#if (defined(TCFG_PCM_ENC2TWS_ENABLE) && (TCFG_PCM_ENC2TWS_ENABLE))
#if (defined(TCFG_PCM2TWS_SBC_ENABLE) && (TCFG_PCM2TWS_SBC_ENABLE))
#define CONFIG_LOCAL_TWS_PCM_ENC_CLK		(24*1000000L)
#define CONFIG_LOCAL_TWS_PCM_DEC_CLK		(16*1000000L + CONFIG_LOCAL_TWS_SYNC_CLK)
#else /*TCFG_PCM2TWS_SBC_ENABLE*/
#define CONFIG_LOCAL_TWS_PCM_ENC_CLK		(48*1000000L)
#define CONFIG_LOCAL_TWS_PCM_DEC_CLK		(48*1000000L + CONFIG_LOCAL_TWS_SYNC_CLK)
#endif /*TCFG_PCM2TWS_SBC_ENABLE*/
#else /*TCFG_PCM_ENC2TWS_ENABLE*/
#define CONFIG_LOCAL_TWS_PCM_ENC_CLK		(0)
#define CONFIG_LOCAL_TWS_PCM_DEC_CLK		(0)
#endif /*TCFG_PCM_ENC2TWS_ENABLE*/

#define CONFIG_LOCAL_TWS_PCM_CLK			(CONFIG_LOCAL_TWS_PCM_ENC_CLK+CONFIG_LOCAL_TWS_PCM_DEC_CLK)

#if (defined(TCFG_DEC2TWS_ENABLE) && (TCFG_DEC2TWS_ENABLE))
#define CONFIG_LOCAL_TWS_PICK_ENC_CLK		(8*1000000L)
#define CONFIG_LOCAL_TWS_PICK_DEC_CLK		(32*1000000L + CONFIG_LOCAL_TWS_SYNC_CLK)
#else /*TCFG_DEC2TWS_ENABLE*/
#define CONFIG_LOCAL_TWS_PICK_ENC_CLK		(0)
#define CONFIG_LOCAL_TWS_PICK_DEC_CLK		(0)
#endif /*TCFG_DEC2TWS_ENABLE*/

#define CONFIG_LOCAL_TWS_PICK_CLK			(CONFIG_LOCAL_TWS_PICK_ENC_CLK+CONFIG_LOCAL_TWS_PICK_DEC_CLK)

/*新增加时钟数，请累加在宏定义中*/
#define CONFIG_MODE_MUSIC_ADD_CLK      	(CONFIG_EQ_ONLINE_CLK + CONFIG_EQ_EN_CLK + CONFIG_DRC_EN_CLK + CONFIG_UDISK_CLK + CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_BT_ADD_CLK      	(CONFIG_EQ_ONLINE_CLK + CONFIG_EQ_EN_CLK + CONFIG_DRC_EN_CLK + CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_FM_EMITTER_ADD_CLK  (CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_RECORD_ADD_CLK      (CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_SPDIF_ADD_CLK      	(CONFIG_MIXERCH_RECORD_CLK)

#if (defined(TCFG_PCM_ENC2TWS_ENABLE) && (TCFG_PCM_ENC2TWS_ENABLE))
#define CONFIG_MODE_LINEIN_ADD_CLK      (CONFIG_EQ_ONLINE_CLK + CONFIG_EQ_EN_CLK + CONFIG_DRC_EN_CLK + CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_FM_ADD_CLK      	(CONFIG_EQ_ONLINE_CLK + CONFIG_EQ_EN_CLK + CONFIG_DRC_EN_CLK + CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_PC_ADD_CLK      	(CONFIG_EQ_ONLINE_CLK + CONFIG_EQ_EN_CLK + CONFIG_DRC_EN_CLK + CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#else
#define CONFIG_MODE_LINEIN_ADD_CLK      (CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_FM_ADD_CLK      	(CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#define CONFIG_MODE_PC_ADD_CLK      	(CONFIG_SBC_ENC_CLK + CONFIG_MIXERCH_RECORD_CLK)
#endif

#define CONFIG_MODE_LOCAL_ADD_CLK      	(CONFIG_MODE_MUSIC_ADD_CLK)


#define CONFIG_AUDIO_DEC_CLOCK_MP3     MIN(((48*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK + CONFIG_LOCAL_TWS_PICK_ENC_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_WAV     MIN(((32*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK + 24*1000000L + CONFIG_LOCAL_TWS_PCM_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_G729    MIN(((32*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#if (defined(TCFG_REVERB_ENABLE) && (TCFG_REVERB_ENABLE))
#define CONFIG_AUDIO_DEC_CLOCK_PCM     MIN(((96*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#else
#define CONFIG_AUDIO_DEC_CLOCK_PCM     MIN(((32*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#endif
#define CONFIG_AUDIO_DEC_CLOCK_MTY     MIN(((32*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)

#define CONFIG_AUDIO_DEC_CLOCK_WMA     MIN(((48*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK + CONFIG_LOCAL_TWS_PICK_ENC_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_AMR     MIN(((32*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_FLAC    MIN(((64*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_APE     MIN(((96*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_DTS     MIN(((120*1000000L)+ CONFIG_MODE_MUSIC_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_M4A     MIN(((48*1000000L) + CONFIG_MODE_MUSIC_ADD_CLK + 24*1000000L), CONFIG_MAX_CLK)

#define CONFIG_AUDIO_DEC_CLOCK_AAC     MIN(((48*1000000L) + CONFIG_MODE_BT_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_SBC     MIN(((48*1000000L) + CONFIG_MODE_BT_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_MSBC    MIN(((48*1000000L) + CONFIG_MODE_BT_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_DEC_CLOCK_CVSD    MIN(((48*1000000L) + CONFIG_MODE_BT_ADD_CLK), CONFIG_MAX_CLK)

#define CONFIG_AUDIO_DEC_CLOCK_UNKNOW  (32*1000000L)

#define CONFIG_AUDIO_CLOCK_LINEIN      MIN(((24*1000000L) + CONFIG_MODE_LINEIN_ADD_CLK + CONFIG_LOCAL_TWS_PCM_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_CLOCK_FM          MIN(((120*1000000L)+ CONFIG_MODE_FM_ADD_CLK + CONFIG_LOCAL_TWS_PCM_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_CLOCK_FM_EMITTER  MIN(((48*1000000L) + CONFIG_MODE_FM_EMITTER_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_CLOCK_PC          MIN(((96*1000000L) + CONFIG_MODE_PC_ADD_CLK + CONFIG_LOCAL_TWS_PCM_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_CLOCK_RECORD      MIN(((120*1000000L) + CONFIG_MODE_RECORD_ADD_CLK), CONFIG_MAX_CLK)
#define CONFIG_AUDIO_CLOCK_SPDIF       MIN(((192*1000000L) + CONFIG_MODE_SPDIF_ADD_CLK), CONFIG_MAX_CLK)

#define CONFIG_LOCAL_TWS_DEC_PCM_CLK		MIN(((8*1000000L) + CONFIG_MODE_LOCAL_ADD_CLK + CONFIG_LOCAL_TWS_PCM_DEC_CLK), CONFIG_MAX_CLK)
#define CONFIG_LOCAL_TWS_DEC_PICK_CLK		MIN(((8*1000000L) + CONFIG_MODE_LOCAL_ADD_CLK + CONFIG_LOCAL_TWS_PICK_DEC_CLK), CONFIG_MAX_CLK)


#define AUDIO_ANOTHER_CLK_TYPE_LINEIN      (AUDIO_CODING_STU_APP | 0)
#define AUDIO_ANOTHER_CLK_TYPE_FM          (AUDIO_CODING_STU_APP | 1)
#define AUDIO_ANOTHER_CLK_TYPE_FM_EMITTER  (AUDIO_CODING_STU_APP | 2)
#define AUDIO_ANOTHER_CLK_TYPE_PC          (AUDIO_CODING_STU_APP | 3)
#define AUDIO_ANOTHER_CLK_TYPE_RECORD      (AUDIO_CODING_STU_APP | 4)
#define AUDIO_ANOTHER_CLK_TYPE_SPDIF       (AUDIO_CODING_STU_APP | 5)

//*********************************************************************************//
//                                  低功耗配置                                     //
//*********************************************************************************//
#if TCFG_IRKEY_ENABLE
#undef  TCFG_LOWPOWER_LOWPOWER_SEL
#define TCFG_LOWPOWER_LOWPOWER_SEL			0                     //开红外不进入低功耗
#endif  /* #if TCFG_IRKEY_ENABLE */

//*********************************************************************************//
//                                 升级配置                                        //
//*********************************************************************************//
#if (defined(CONFIG_CPU_BR30))
//升级LED显示使能
//#define UPDATE_LED_REMIND
//升级提示音使能
//#define UPDATE_VOICE_REMIND
#endif

#if (defined(CONFIG_CPU_BR23) || defined(CONFIG_CPU_BR25))
//升级IO保持使能
//#define DEV_UPDATE_SUPPORT_JUMP           //目前只有br23\br25支持
#endif
#endif

//*********************************************************************************//
//                              usb app差异化配置                                  //
//*********************************************************************************//


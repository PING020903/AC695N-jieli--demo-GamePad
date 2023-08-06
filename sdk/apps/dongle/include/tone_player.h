#ifndef _TONE_PLAYER_
#define _TONE_PLAYER_

#include "app_config.h"
#include "audio_config.h"

#define DEFAULT_SINE_SAMPLE_RATE 16000
#define SINE_TOTAL_VOLUME        26843546//16106128//20132660 //26843546
#define TONE_STOP       0
#define TONE_START      1

#define CONFIG_USE_DEFAULT_SINE     1

#define TONE_DEFAULT_VOL   SYS_MAX_VOL

enum {
    IDEX_TONE_NUM_0,
    IDEX_TONE_NUM_1,
    IDEX_TONE_NUM_2,
    IDEX_TONE_NUM_3,
    IDEX_TONE_NUM_4,
    IDEX_TONE_NUM_5,
    IDEX_TONE_NUM_6,
    IDEX_TONE_NUM_7,
    IDEX_TONE_NUM_8,
    IDEX_TONE_NUM_9,
    IDEX_TONE_BT_MODE,
    IDEX_TONE_BT_CONN,
    IDEX_TONE_BT_DISCONN,
    IDEX_TONE_TWS_CONN,
    IDEX_TONE_TWS_DISCONN,
    IDEX_TONE_LOW_POWER,
    IDEX_TONE_POWER_OFF,
    IDEX_TONE_POWER_ON,
    IDEX_TONE_RING,
    IDEX_TONE_MAX_VOL,
    IDEX_TONE_NORMAL,
#if (defined(TCFG_APP_MUSIC_EN) && (TCFG_APP_MUSIC_EN))
    IDEX_TONE_MUSIC,
#endif
#if (defined(TCFG_APP_LINEIN_EN) && (TCFG_APP_LINEIN_EN))
    IDEX_TONE_LINEIN,
#endif
#if (defined(TCFG_APP_FM_EN) && (TCFG_APP_FM_EN))
    IDEX_TONE_FM,
#endif
#if (defined(TCFG_APP_PC_EN) && (TCFG_APP_PC_EN))
    IDEX_TONE_PC,
#endif
#if (defined(TCFG_APP_RTC_EN) && (TCFG_APP_RTC_EN))
    IDEX_TONE_RTC,
#endif
#if (defined(TCFG_APP_RECORD_EN) && (TCFG_APP_RECORD_EN))
    IDEX_TONE_RECORD,
#endif

    IDEX_TONE_NONE = 0xFF,
};

#define TONE_NUM_0      		SDFILE_RES_ROOT_PATH"tone/0.*"
#define TONE_NUM_1      		SDFILE_RES_ROOT_PATH"tone/1.*"
#define TONE_NUM_2				SDFILE_RES_ROOT_PATH"tone/2.*"
#define TONE_NUM_3				SDFILE_RES_ROOT_PATH"tone/3.*"
#define TONE_NUM_4				SDFILE_RES_ROOT_PATH"tone/4.*"
#define TONE_NUM_5				SDFILE_RES_ROOT_PATH"tone/5.*"
#define TONE_NUM_6				SDFILE_RES_ROOT_PATH"tone/6.*"
#define TONE_NUM_7				SDFILE_RES_ROOT_PATH"tone/7.*"
#define TONE_NUM_8				SDFILE_RES_ROOT_PATH"tone/8.*"
#define TONE_NUM_9				SDFILE_RES_ROOT_PATH"tone/9.*"
#define TONE_BT_MODE			SDFILE_RES_ROOT_PATH"tone/bt.*"
#define TONE_BT_CONN       		SDFILE_RES_ROOT_PATH"tone/bt_conn.*"
#define TONE_BT_DISCONN    		SDFILE_RES_ROOT_PATH"tone/bt_dconn.*"
#define TONE_TWS_CONN			SDFILE_RES_ROOT_PATH"tone/tws_conn.*"
#define TONE_TWS_DISCONN		SDFILE_RES_ROOT_PATH"tone/tws_dconn.*"
#define TONE_LOW_POWER			SDFILE_RES_ROOT_PATH"tone/low_power.*"
#define TONE_POWER_OFF			SDFILE_RES_ROOT_PATH"tone/power_off.*"
#define TONE_POWER_ON			SDFILE_RES_ROOT_PATH"tone/power_on.*"
#define TONE_RING				SDFILE_RES_ROOT_PATH"tone/ring.*"
#define TONE_MAX_VOL			SDFILE_RES_ROOT_PATH"tone/vol_max.*"
#define TONE_MUSIC				SDFILE_RES_ROOT_PATH"tone/music.*"
#define TONE_LINEIN				SDFILE_RES_ROOT_PATH"tone/linein.*"
#define TONE_PC 				SDFILE_RES_ROOT_PATH"tone/pc.*"
#define TONE_FM 				SDFILE_RES_ROOT_PATH"tone/fm.*"
#define TONE_RTC 				SDFILE_RES_ROOT_PATH"tone/rtc.*"
#define TONE_RECORD 			SDFILE_RES_ROOT_PATH"tone/record.*"
#define TONE_SPDIF 			    SDFILE_RES_ROOT_PATH"tone/spdif.*"

#ifdef CONFIG_CPU_BR18
#undef TONE_POWER_ON
#undef TONE_POWER_OFF
#undef TONE_BT_CONN
#undef TONE_BT_DISCONN

#define TONE_POWER_ON			SDFILE_RES_ROOT_PATH"power_on.mp3"
#define TONE_POWER_OFF			SDFILE_RES_ROOT_PATH"power_off.*"
#define TONE_BT_CONN       		SDFILE_RES_ROOT_PATH"bt_conn.mp3"
#define TONE_BT_DISCONN    		SDFILE_RES_ROOT_PATH"bt_dconn.mp3"
#endif


#define SINE_WTONE_NORAML           0
#define SINE_WTONE_TWS_CONNECT      1
#define SINE_WTONE_TWS_DISCONNECT   2
#define SINE_WTONE_LOW_POWER        4
#define SINE_WTONE_RING             5
#define SINE_WTONE_MAX_VOLUME       6

#define TONE_REPEAT_BEGIN(a)  (char *)((0x1 << 30) | (a & 0xffff))
#define TONE_REPEAT_END()     (char *)(0x2 << 30)

#define IS_REPEAT_BEGIN(a)    ((((u32)a >> 30) & 0x3) == 0x1 ? 1 : 0)
#define IS_REPEAT_END(a)      ((((u32)a >> 30) & 0x3) == 0x2 ? 1 : 0)
#define TONE_REPEAT_COUNT(a)  (((u32)a) & 0xffff)

#define DEFAULT_SINE_TONE(a)     (char *)(((u32)0x3 << 30) | (a))
#define IS_DEFAULT_SINE(a)       ((((u32)a >> 30) & 0x3) == 0x3 ? 1 : 0)
#define DEFAULT_SINE_ID(a)       ((u32)a & 0xffff)

#if CONFIG_USE_DEFAULT_SINE
#undef TONE_TWS_CONN
#define TONE_TWS_CONN           DEFAULT_SINE_TONE(SINE_WTONE_TWS_CONNECT)
#undef TONE_TWS_DISCONN
#define TONE_TWS_DISCONN        DEFAULT_SINE_TONE(SINE_WTONE_TWS_DISCONNECT)

#undef TONE_MAX_VOL
#define TONE_MAX_VOL            DEFAULT_SINE_TONE(SINE_WTONE_MAX_VOLUME)

#undef TONE_NORMAL
#define TONE_NORMAL            DEFAULT_SINE_TONE(SINE_WTONE_NORAML)
#if BT_PHONE_NUMBER

#else
#undef TONE_RING
#define TONE_RING               DEFAULT_SINE_TONE(SINE_WTONE_RING)
#endif

#undef TONE_LOW_POWER
#define TONE_LOW_POWER          DEFAULT_SINE_TONE(SINE_WTONE_LOW_POWER)
#endif

#define TONE_SIN_NORMAL			DEFAULT_SINE_TONE(SINE_WTONE_NORAML)

int tone_play_index(u8 index, u8 preemption);
int tone_play_index_with_callback(u8 index, u8 preemption, void (*user_evt_handler)(void *priv), void *priv);
int tone_file_list_play(const char **list, u8 preemption);
int tone_file_list_play_with_callback(const char **list, u8 preemption, void (*user_evt_handler)(void *priv), void *priv);
int tone_play_with_callback(const char *name, u8 preemption, void (*user_evt_handler)(void *priv), void *priv);
int tone_play_with_callback_stop(void);
int tone_play_stop(void);
int tone_sin_play(int time_ms, u8 wait);
int tone_get_status();
int tone_file_list_stop(u8 no_end);

int mode_tone_play(const char *name, void (*user_evt_handler)(void *priv), void *priv);
void mode_tone_stop(void);

int record_file_play(void);

#endif

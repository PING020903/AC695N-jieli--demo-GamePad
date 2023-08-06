/*********************************************************************************************
    *   Filename        : app_main.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : caibingquan@zh-jieli.com

    *   Last modifiled  : 2019-05-11 14:54

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "gpio.h"
#include "app_main.h"
#include "asm/charge.h"
#include "update.h"
#include "app_power_manage.h"
#include "app_charge.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

int app_task_switch(const char *name, int action, void *param);

/*任务列表 */
const struct task_info task_info_table[] = {
    {"app_core",            1,     512,   128  },
    {"sys_event",           7,     256,   0    },
    {"btctrler",            4,     512,   256  },
    {"btencry",             1,     512,   128  },
    {"btstack",             3,     512,   512  },
    {"systimer",		    7,	   128 + 128,   0		},
    {"audio_dec",           5,     512,   128  },
    {"audio_enc",           3,     256,   128  },
    {"usb_msd",             3,     1024,  512  },
    {"usb_audio",           4,     256,   256  },
    {"mic_src",             4,     256,   128  },
    {"aec",					2,	   512,   128	},
    {"update",				1,	   512,   0		},
    {"dw_update",		 	2,	   256,   128  },
    {0, 0},
};

APP_VAR app_var;

void app_var_init(void)
{
    memset((u8 *)&bt_user_priv_var, 0, sizeof(BT_USER_PRIV_VAR));
    app_var.play_poweron_tone = 1;

}

void app_main()
{
    struct intent it;

    init_intent(&it);
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;
#if TCFG_AUDIO_ENABLE
    extern int audio_dec_init();
    extern int audio_enc_init();
    audio_dec_init();
    audio_enc_init();
#endif/*TCFG_AUDIO_ENABLE*/


    start_app(&it);
}

int eSystemConfirmStopStatus(void)
{

    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    //1:Endless Sleep
    //0:100 ms wakeup
    /* if (get_charge_full_flag()) {
        log_i("Endless Sleep");
        power_set_soft_poweroff();
        return 1;
    }
       else  */
    {
        log_i("100 ms wakeup");
        return 0;
    }
}



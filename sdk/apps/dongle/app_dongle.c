/*********************************************************************************************
    *   Filename        : dongle.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : caibingquan@zh-jieli.com

    *   Last modifiled  : 2019-05-11 14:35

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "app_dongle.h"
#include "system/app_core.h"
#include "system/includes.h"
#include "server/server_core.h"
#include "app_config.h"
#include "app_action.h"
#include "os/os_api.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"
/* #include "clock_cfg.h" */

#if TCFG_PC_ENABLE
#include "usb/usb_config.h"
#include "usb/device/usb_stack.h"
#include "usb/device/hid.h"
#include "usb/device/msd.h"
#include "audio_config.h"
#endif

/* #include "uac_stream.h" */

#define LOG_TAG_CONST       DONGLE
#define LOG_TAG             "[DONGLE]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"
#include "my_put_test.h"


extern int btstack_demo_init();
extern int audio_demo_init();
extern void usbstack_init();
extern void usb_start();
extern void bt_pll_para(u32 osc, u32 sys, u8 low_power, u8 xosc);
extern u8 dongle_key_event_get(struct key_event *key);
extern s8 app_audio_get_volume(u8 state);

/************************************user***********************************/
extern void my_task_init(void);
/***************************************************************************/

_WEAK_
s8 app_audio_get_volume(u8 state)
{
    return 88;
}
_WEAK_
void usb_audio_demo_exit(void)
{

}
_WEAK_
int usb_audio_demo_init(void)
{
    return 0;
}

static hci_transport_config_uart_t config = {
    HCI_TRANSPORT_CONFIG_UART,
    115200,
    0,  // main baudrate
    0,  // flow control
    NULL,
};

#if (CONFIG_BT_MODE == BT_BQB)
extern void dut_le_hw_open(void);
extern void dut_hci_controller_init(const hci_transport_t *transport, const void *config);

static void ble_bqb_test_thread_init(void)
{
    wdt_close();

    dut_le_hw_open();

    dut_hci_controller_init((void *)hci_transport_uart_instance(), &config);
}
#endif

static void hci_interface_init(void)
{
#if (CONFIG_BT_MODE == BT_FCC)
    btctrler_task_init((void *)hci_transport_uart_instance(), &config);
#elif (CONFIG_BT_MODE == BT_BQB)
    btctrler_task_init((void *)NULL, NULL);

    ble_bqb_test_thread_init();
#else
    /* btctrler_task_init((void *)hci_transport_uart_instance(), &config); */
    btctrler_task_init((void *)hci_transport_usb_instance(), NULL);
#endif
}


static void app_start()
{
    log_info("=======================================");
    log_info("--------------dongle demo--------------");
    log_info("=======================================");

    /* clock_idle(BT_IDLE_CLOCK); */
    u32 sys_clk =  clk_get("sys");
    bt_pll_para(TCFG_CLOCK_OSC_HZ, sys_clk, 0, 0);

    /* audio_dev_init(); */

    //test_function();

#if 0
    log_info("App Start - USB");
    hci_interface_init();
#else
    //log_info("App Start - HCI");
    //btstack_demo_init();
    //usbstack_init();
    //usb_start();

#if TCFG_PC_ENABLE
    //otg mode
    usbstack_init();
    //no otg mode
    usb_start();
#endif

    /* 按键消息使能 */
    //sys_key_event_enable();
    /* hci_interface_init(); */

#endif

    /* extern void my_dac_test(void); */
    /* my_dac_test(); */
/*********************** user task **************************/
    my_task_init();
/************************************************************/
}

static int state_machine(struct application *app, enum app_state state, struct intent *it)
{
    switch (state) {
    case APP_STA_CREATE:
        break;
    case APP_STA_START:
        if (!it) {
            break;
        }
        switch (it->action) {
        case ACTION_DONGLE_MAIN:
            app_start();
            sys_key_event_enable();
            break;
        }
        break;
    case APP_STA_PAUSE:
        break;
    case APP_STA_RESUME:
        break;
    case APP_STA_STOP:
        break;
    case APP_STA_DESTROY:
        log_info("APP_STA_DESTROY\n");
        break;
    }

    return 0;
}


int app_dongle_key_event_handler(struct sys_event *event)
{
    int ret = false;
    struct key_event *key = &event->u.key;
    u8 key_event;

    key_event = dongle_key_event_get(key);

    //if(key_event != KEY_NULL)
    static unsigned char dddd = 0;
    dddd ++;
    if(dddd == 0)
    {


    if(key->type == 3)
        log_info("key_event:%d %x %d, init:%d, tmr:%d\n", key_event, key->value, key->type, key->init, key->tmr);
    if(key->type != 3)
        log_info("key_event:%d %d %d, init:%d, tmr:%d\n", key_event, key->value, key->type, key->init, key->tmr);
    }
    key_event = 100;
    switch (key_event) {

#if TCFG_PC_ENABLE
    case  KEY_MUSIC_PP:
        log_info("KEY_MUSIC_PP\n");
        hid_key_handler(0, USB_AUDIO_PP);
        break;
    case  KEY_MUSIC_PREV:
        log_info("KEY_MUSIC_PREV\n");
        hid_key_handler(0, USB_AUDIO_PREFILE);
        break;
    case  KEY_MUSIC_NEXT:
        log_info("KEY_MUSIC_NEXT\n");
        hid_key_handler(0, USB_AUDIO_NEXTFILE);
        break;
    case  KEY_VOL_UP:
        log_info("KEY_VOL_UP\n");
        hid_key_handler(0, USB_AUDIO_VOLUP);
        log_info("vol+: %d", app_audio_get_volume(APP_AUDIO_CURRENT_STATE));
        break;
    case  KEY_VOL_DOWN:
        log_info("KEY_VOL_DOWN\n");
        hid_key_handler(0, USB_AUDIO_VOLDOWN);
        log_info("vol-: %d", app_audio_get_volume(APP_AUDIO_CURRENT_STATE));
        break;
#endif
    case 123:
        {
            log_info("my_rocker_ad_key\n");
            //hid_key_handler(0, )
            break;
        }

    case  KEY_NULL:
        break;
    }
    return ret;
}

static int event_handler(struct application *app, struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        return app_dongle_key_event_handler(event);

    case SYS_BT_EVENT:
        return 0;

    default:
        return FALSE;
    }
    return FALSE;
}



static const struct application_operation app_dongle_ops = {
    .state_machine  = state_machine,
    .event_handler 	= event_handler,
};

REGISTER_APPLICATION(app_app_donle) = {
    .name 	= "dongle",
    .action	= ACTION_DONGLE_MAIN,
    .ops 	= &app_dongle_ops,
    .state  = APP_STA_DESTROY,
};



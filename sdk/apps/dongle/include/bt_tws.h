#ifndef APP_BT_TWS_H
#define APP_BT_TWS_H

#include "classic/tws_api.h"
#include "classic/tws_event.h"

#define    TWS_ACTIVE_DEIVCE                 0
#define    TWS_UNACTIVE_DEIVCE               1

#define TWS_FUNC_ID_VOL_SYNC        TWS_FUNC_ID('V', 'O', 'L', 'S')
#define TWS_FUNC_ID_VBAT_SYNC       TWS_FUNC_ID('V', 'B', 'A', 'T')
#define TWS_FUNC_ID_CHARGE_SYNC     TWS_FUNC_ID('C', 'H', 'G', 'S')
#define TWS_FUNC_ID_BOX_SYNC        TWS_FUNC_ID('B', 'O', 'X', 'S')
#define TWS_FUNC_ID_AI_DMA_RAND     TWS_FUNC_ID('A', 'I', 'D', 'M')
#define TWS_FUNC_ID_AI_SPEECH_STOP  TWS_FUNC_ID('A', 'I', 'S', 'T')
#define TWS_FUNC_ID_APP_MODE  		TWS_FUNC_ID('M', 'O', 'D', 'E')

enum {
    DEBUG_LINK_PAGE_STATE = 0,
    DEBUG_LINK_INQUIRY_STATE,
    DEBUG_LINK_PAGE_SCAN_STATE,
    DEBUG_LINK_INQUIRY_SCAN_STATE,
    DEBUG_LINK_CONNECTION_STATE,
    DEBUG_LINK_PAGE_TWS_STATE,
    DEBUG_LINK_PAGE_SCAN_TWS_STATE,
};
enum {
    BT_TWS_STATUS_INIT_OK = 1,
    BT_TWS_STATUS_SEARCH_START,
    BT_TWS_STATUS_SEARCH_TIMEOUT,
    BT_TWS_STATUS_PHONE_CONN,
    BT_TWS_STATUS_PHONE_DISCONN,
};
enum {
    SYNC_CMD_TWS_CONN_TONE      = 1,
    SYNC_CMD_PHONE_CONN_TONE,
    SYNC_CMD_PHONE_NUM_TONE,
    SYNC_CMD_PHONE_RING_TONE,
    SYNC_CMD_PHONE_SYNC_NUM_RING_TONE,
    SYNC_CMD_LED_TWS_CONN_STATUS,
    SYNC_CMD_LED_PHONE_CONN_STATUS,
    SYNC_CMD_LED_PHONE_DISCONN_STATUS,
    SYNC_CMD_POWER_OFF_TOGETHER,
    SYNC_CMD_MAX_VOL,

    SYNC_CMD_MODE_START,
    SYNC_CMD_MODE_BT = SYNC_CMD_MODE_START,
    SYNC_CMD_MODE_MUSIC,
    SYNC_CMD_MODE_LINEIN,
    SYNC_CMD_MODE_FM,
    SYNC_CMD_MODE_PC,
    SYNC_CMD_MODE_ENC,
    SYNC_CMD_MODE_RTC,
    SYNC_CMD_MODE_SPDIF,
    SYNC_CMD_MODE_STOP,
    SYNC_CMD_LOW_LATENCY_ENABLE,
    SYNC_CMD_LOW_LATENCY_DISABLE,
    SYNC_CMD_EARPHONE_CHAREG_START,
    SYNC_CMD_IRSENSOR_EVENT_NEAR,
    SYNC_CMD_IRSENSOR_EVENT_FAR,


    SYNC_CMD_BOX_ENTER_BT,
    SYNC_CMD_BOX_EXIT_BT,
    SYNC_CMD_BOX_INIT_EXIT_BT = 0x80 | SYNC_CMD_BOX_EXIT_BT,
};

enum {
    TWS_SYNC_VOL     = 0,
    TWS_SYNC_CALL_VOL,
    TWS_SYNC_VBAT,
    TWS_SYNC_CHG,
    TWS_SYNC_BOX,

};

enum {
    TWS_BOX_EXIT_BT = 0,
    TWS_BOX_ENTER_BT,
    TWS_BOX_NOTICE_A2DP_BACK_TO_BT_MODE,
    TWS_BOX_A2DP_BACK_TO_BT_MODE_START,
};

struct tws_sync_info_t {
    u8 type;
    union {
        s8 volume_lev;
        u16 vbat_lev;
        u8 chg_lev;
        u8 tws_box;
    } u;
};


#define TWS_WAIT_CONN_TIMEOUT      (400)

#define TWS_SYNC_TIME_DO       800
// #define TWS_CON_SUPER_TIMEOUT   8000
#define TWS_CON_SEARCH_TIMEOUT  0x07//(n*1.28s)


char bt_tws_get_local_channel();

int bt_tws_connction_status_event_handler(struct bt_event *evt);

int bt_tws_poweron();

int bt_tws_poweroff();

int bt_tws_start_search_sibling();

void bt_tws_hci_event_connect();

int bt_tws_phone_connected();

void bt_tws_phone_page_timeout();

void bt_tws_phone_connect_timeout();

void bt_tws_phone_disconnected();

int bt_tws_sync_phone_num(void *priv);

int bt_tws_sync_led_status();

int get_bt_tws_connect_status();

u8 is_tws_active_device(void);

void set_tws_active_device(u8 device_role);


void tws_page_scan_deal_by_esco(u8 esco_flag);
void tws_user_sync_box(u8 cmd, u8 value);

#endif

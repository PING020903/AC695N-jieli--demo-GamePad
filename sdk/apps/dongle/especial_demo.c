#include "os/os_api.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"
#include "btstack_demo.h"
#include "system/timer.h"

#define LOG_TAG_CONST       BTSTACK
#define LOG_TAG             "[BTSTACK]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#define TWS_BLE_FAST_SEARCH

/*

----------------------   ble fast search 启动\关闭接口    ------------------------
	suspend :1 通过外部调用resume来驱动运行

	void tws_ble_fast_search_scan(A_payload,bt_get_tws_device_indicate(NULL),suspend);
	void tws_ble_fast_conn_scan(B_payload,suspend);

	void tws_ble_fast_search_scan_resume(u16 slot);
	void tws_ble_fast_conn_scan_resume(u16 slot);
	void tws_ble_fast_conn_adv_resume(u16 slot);

	void tws_ble_fast_search_conn_close();

----------------------   ble fast search 返回结果    ------------------------
 	void app_tws_fast_search_cb(u8 * addr,u8 *data)


----------------------   bredr 启动\关闭接口    ------------------------
	bredr 启动、关闭
	void bredr_rf_suspend();
	void bredr_rf_resume();


----------------------    启动蓝牙收发状态    ------------------------
	            TX     RX
	   AI800x   PA13   PA12
	   AC692x   PA13   PA12
	   AC693x   PA8    PA9
	   AC695x   PA9    PA10
	   AC694x   PB1    PB2
	 bt_set_rxtx_status_enable(1);


----------------------    干扰检测     ------------------------

///变量初始化
	void disturb_scan_var_init(void);

///结果更新
	void disturb_scan_channel_result();

///全频段检测
	int link_disturb_scan_all_enable();

///固定频段检测
	int link_disturb_scan_fre_enable();
	void link_disturb_scan_fre_resume(u16 slot, u8 channle);



///设置lmp超时不判断是否加密
	void set_bt_need_enc(u8 enc)


*/



#ifdef  TWS_BLE_FAST_SEARCH

void tws_fast_search_cb_register(void (*handle)(u8 *addr, u8 *data));
void tws_ble_fast_search_scan(u8 *data, int pair_code, u8 suspend);
void tws_ble_fast_conn_scan(u8 *data, u8 suspend);
u16 bt_get_tws_device_indicate(u8 *tws_device_indicate);
void tws_ble_fast_search_conn_close();
void tws_ble_fast_search_scan_resume();
void tws_ble_fast_conn_resume();

////tws 快速通信的内容最多14byte
u8 A_payload[14];
u8 B_payload[14];

u32 g_le_tws_search_aa = 0xffffffff;
u32 g_le_tws_pair_aa = 0xffffffff;
u32 g_le_tws_connect_aa = 0xffffffff;
u16 fast_search_timer, fast_conn_timer;
u16 disturb_timer;

static u8 const code_buf[6] = {0x45, 0x89, 0x76, 0x43, 0x8a, 0x9b};

unsigned long tws_le_acc_generation(u8 *addr);

static void tws_ble_fast_search_scan_timeout()
{
    printf("------fast search scan timeout ----  \n");
    fast_search_timer = 0 ;
    tws_ble_fast_search_conn_close();
}

void tws_ble_fast_search_scan_api()
{
    tws_ble_fast_search_scan(A_payload, bt_get_tws_device_indicate(NULL), 1);

    if (fast_search_timer == 0) {
        fast_search_timer = sys_timeout_add(NULL, tws_ble_fast_search_scan_timeout, 8000);
    }
}

static void tws_ble_fast_conn_scan_timeout()
{
    printf("------fast conn scan timeout ----  \n");
    fast_conn_timer = 0 ;
    tws_ble_fast_search_conn_close();
}

void tws_ble_fast_conn_scan_api()
{
    tws_ble_fast_conn_scan(B_payload, 1);

    if (fast_conn_timer == 0) {
        fast_conn_timer = sys_timeout_add(NULL, tws_ble_fast_conn_scan_timeout, 8000);
    }
}

static void tws_le_acc_generation_init(void)
{
    g_le_tws_search_aa  = tws_le_acc_generation(code_buf);
    g_le_tws_pair_aa       = g_le_tws_search_aa;
    g_le_tws_connect_aa    = g_le_tws_search_aa;
    printf("------  adv aa %x \n", g_le_tws_search_aa);
}

u32 tws_le_get_pair_aa(void)
{
    ASSERT(g_le_tws_pair_aa != 0xffffffff, "g_le_tws_aa_pair != 0xffffffff");
    return g_le_tws_pair_aa;
}

u32 tws_le_get_connect_aa(void)
{
    ASSERT(g_le_tws_connect_aa != 0xffffffff, "g_le_tws_connect_aa != 0xffffffff");
    return g_le_tws_connect_aa;
}

u32 tws_le_get_search_aa(void)
{
    ASSERT(g_le_tws_search_aa != 0xffffffff, "g_le_tws_aa_rearch != 0xffffffff");
    return g_le_tws_search_aa;
}

static void app_tws_fast_search_cb(u8 *addr, u8 *data)
{
    printf("-------info--  \n");
    put_buf(addr, 6);
    put_buf(data, 14);
}

void tws_ble_fast_search_init()
{
    tws_le_acc_generation_init();
    tws_fast_search_cb_register(app_tws_fast_search_cb);
}

#if 1
extern u8 disturb_map[10];
extern s8 disturb_channle_rssi[16][2];


#define AFH_CHA_NUKNOWN      0
#define AFH_CHA_GOOD         1
#define AFH_CHA_RESERVED     2
#define AFH_CHA_BAD          3

#define AFH_USED             1
#define AFH_NOTUSED          0



void disturb_scan_channel_used(u8 used_cnt)
{
    u8 i, j, k;
    s8 rssi, channle, channle1;
    u8 cnt = 0;
    for (i = 0; i < 16; i++) {
        channle = disturb_channle_rssi[i][0] - 2;
        for (j = 0; j < 5; j++) {
            channle1 = channle + j;
            disturb_map[channle1 / 8] |= (AFH_USED << ((channle1) % 8));
            cnt++;
            if (cnt > used_cnt) {
                return;
            }
        }
    }
}


void disturb_scan_channel_result()
{
    u8 i, j, k;
    s8 rssi, channle, channle1;
    /* for(i=0;i<16;i++)
    {
    	printf("%d %d \n",disturb_channle_rssi[i][0],disturb_channle_rssi[i][1]);
    } */

    for (i = 0; i < 15; i++) {
        k = i;
        for (j = i + 1; j < 16; j++) {
            if (disturb_channle_rssi[j][1] < disturb_channle_rssi[k][1]) {
                k = j;
            }
        }

        rssi = disturb_channle_rssi[k][1];
        channle = disturb_channle_rssi[k][0];

        disturb_channle_rssi[k][1] = disturb_channle_rssi[i][1];
        disturb_channle_rssi[k][0] = disturb_channle_rssi[i][0];

        disturb_channle_rssi[i][1] = rssi;
        disturb_channle_rssi[i][0] = channle;
    }

    disturb_scan_channel_used(40);

    printf("\n---+++++  list ---\n");

    for (i = 0; i < 16; i++) {
        printf("%d %d \n", disturb_channle_rssi[i][0], disturb_channle_rssi[i][1]);
    }

    printf("\n--- map ---\n");

    for (i = 0; i < 10; i++) {
        printf("%x \n", disturb_map[i]);
    }

}

#endif

#else

void tws_ble_fast_search_init()
{

}
#endif





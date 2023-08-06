#include "system/includes.h"
#include "server/server_core.h"

void update_close_hw()
{

}

u8 pdown_keep_pw_gate()
{
    return false;
}


u16 update_result_get(void)
{
    return 0;
}

void update_mode_api(u8 type, ...)
{

}

void update_loader_download_init(void)
{

}

void (*lmp_ch_update_resume_hdl)(void *priv) = NULL;

u8 update_id_info_fill_content(u8 *p, u16 allow_len)
{
    return 0;
}

void bt_lmp_update_loader_download_init(void)
{
}

void ble_test_update_loader_download_init(void)
{

}

int filter_out_sbc_data_en(void)
{
    return 0;
}

int lib_btstack_version(void)
{
    return 0;
}
int lib_media_version(void)
{
    return 0;
}


#if 0

void get_random_number(u8 *ptr, u8 len)
{
}
int hci_send_cmd(void *cmd, ...)
{
    return 0;
}
int btctrler_task_ready()
{
    return 0;
}
void bt_pll_para(u32 osc, u32 sys, u8 low_power, u8 xosc)
{
}
int le_controller_set_mac(void *addr)
{
    return 0;
}
void lmp_hci_write_local_address(const u8 *addr)
{
}
void *hci_transport_h4_controller_instance(void)
{
    return NULL;
}
int btctrler_task_init(const void *transport, const void *config)
{
    return 0;
}
void reset_source_dump(void)
{
}
void bt_max_pwr_set(u8 pwr, u8 pg_pwr, u8 iq_pwr, u8 ble_pwr)
{
}
void lp_winsize_init(void *lp)
{
}
#endif

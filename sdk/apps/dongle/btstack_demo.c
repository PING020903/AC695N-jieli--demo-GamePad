#include "os/os_api.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"
#include "bluetooth.h"
#include "app_config.h"
#include "app_action.h"
#include "generic/lbuf.h"
#include "btstack_demo.h"
#include "user_cfg.h"

#define LOG_TAG_CONST       BTSTACK
#define LOG_TAG             "[BTSTACK]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

/* void tws_ble_fast_search_init(); */
void lmp_hci_write_local_address(const u8 *addr);
void bt_set_rxtx_status_enable(u8 en);
void set_bt_need_enc(u8 enc);

#define THIS_TASK_NAME      "btstack"

struct btstack_handle {

    u8 *pHci_buffer;   //hci data path memory

    struct lbuff_head *pEVT;        //hci event path pointer

    u8 state;

    u8 init_state;
};

static struct btstack_handle *hdl;

#define __this      (hdl)


struct hci_event {
    u8 length;
    u8 payload[0];
};

const uint8_t le_advertisements_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
};
const uint8_t le_advertisements_data_len = sizeof(le_advertisements_data);

const uint8_t le_scan_response_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
    // Name
    0x0c, 0x09, 'J', 'L', ' ', 'A', 'C', '6', '9', '2', 'X', 'L', 'E',
    // Incomplete List of 16-bit Service Class UUIDs -- FF10 - only valid for testing!
    0x03, 0x02, 0x10, 0xff,
};

const uint8_t le_scan_response_data_len = sizeof(le_scan_response_data);

#define LITTLE_ENDIAN_READ_16(packet, pos)      ((uint16_t) packet[pos]) | (((uint16_t)packet[(pos) + 1]) << 8);
#define LITTLE_ENDIAN_READ_24(packet, pos)      ((uint32_t) packet[pos]) | (((uint32_t)packet[(pos) + 1]) << 8) | (((uint32_t)packet[(pos) + 2]) << 16);
#define LITTLE_ENDIAN_READ_32(packet, pos)      ((uint32_t) packet[pos]) | (((uint32_t)packet[(pos) + 1]) << 8) | (((uint32_t)packet[(pos) + 2]) << 16) | (((uint32_t)packet[(pos) + 3]) << 24);


const static u8 eir_data[] = {
    0x05, 0x03, 0x24, 0x11, 0x00, 0x12,
    0x0d, 0x09, 'J', 'o', 'y', '-', 'C', 'o', 'n', ' ', '(', 'R', ')', 0,
    0x09, 0x10, 0x02, 0x00, 0x7c, 0x05, 0x09, 0x20, 0x00, 0x01, 0x00,
};

static u16 g_handle;

static void hci_send_acl_packet_test(void)
{
    static u8 test_cnt = 0x00;
    u8 test_buf[32];
    u16 test_len = sizeof(test_buf);
    test_cnt++;
    memcpy(&test_buf[0], &g_handle, 2);
    test_len -= 2;  //g_handle  2byte
    memcpy(&test_buf[2], &test_len, 2);
    test_len -= 6;  //ACL_DATA_LEN 2byte + l2cap_len:2byte + l2cap_CID:2byte
    memcpy(&test_buf[4], &test_len, 2);
    memset(&test_buf[6], test_cnt, sizeof(test_buf) - 6);
    hci_send_acl_packet(test_buf, sizeof(test_buf));
}

static void hci_init_state_machine(void)
{
    switch (__this->init_state) {
    case HCI_INIT_SEND_RESET:
        log_info("HCI CMD - RESET");
        hci_send_cmd(&hci_reset);
        __this->init_state = HCI_INIT_W4_SEND_RESET;
        break;
    case HCI_INIT_READ_BD_ADDR:
        log_info("HCI CMD - READ_BD_ADDR");
        hci_send_cmd(&hci_read_bd_addr);
        __this->init_state = HCI_INIT_W4_READ_BD_ADDR;
        break;
    case HCI_INIT_READ_BUFFER_SIZE:
        log_info("HCI CMD - READ_BUFFER_SIZE");
        hci_send_cmd(&hci_read_buffer_size);
        __this->init_state = HCI_INIT_W4_READ_BUFFER_SIZE;
        break;
    case HCI_INIT_SEND_READ_LOCAL_VERSION_INFORMATION:
        log_info("HCI CMD - READ_LOCAL_VERSION_INFORMATION");
        hci_send_cmd(&hci_read_local_version_information);
        __this->init_state = HCI_INIT_W4_SEND_READ_LOCAL_VERSION_INFORMATION;
        break;
    case HCI_INIT_SEND_READ_LOCAL_NAME:
        log_info("HCI CMD - READ_LOCAL_NAME");
        hci_send_cmd(&hci_read_local_name);
        __this->init_state = HCI_INIT_W4_SEND_READ_LOCAL_NAME;
        break;
    case HCI_INIT_SET_EVENT_MASK:
        log_info("HCI CMD - SET_EVENT_MASK");
        hci_send_cmd(&hci_set_event_mask, 0xffffffff, 0x3FFFFFFF);
        __this->init_state = HCI_INIT_W4_SET_EVENT_MASK;
        break;

#ifdef LE_INIT_ENABLE
    case HCI_INIT_LE_READ_BUFFER_SIZE:
        log_info("HCI CMD - LE_READ_BUFFER_SIZE");
        hci_send_cmd(&hci_le_read_buffer_size);
        __this->init_state = HCI_INIT_W4_LE_READ_BUFFER_SIZE;
        break;
    case HCI_INIT_LE_SET_EVENT_MASK:
        log_info("HCI CMD - LE_SET_EVENT_MASK");
        hci_send_cmd(&hci_le_set_event_mask, 0x1FF, 0x0);
        __this->init_state = HCI_INIT_W4_LE_SET_EVENT_MASK;
        break;

#ifdef LE_PERIPHERAL_ENABLE
    case HCI_INIT_LE_SET_ADV_PARAMETERS:
        log_info("HCI CMD - LE_SET_ADV_PARAMETERS");
        {
            u8 null_addr[6];
            memset(null_addr, 0, 6);
            hci_send_cmd(&hci_le_set_advertising_parameters,
                         100 * 1000 / 625,
                         100 * 1000 / 625,
                         0,
                         0,
                         0,
                         null_addr,
                         0x7,
                         0);
        }
        __this->init_state = HCI_INIT_W4_LE_SET_ADV_PARAMETERS;
    case HCI_INIT_LE_SET_ADV_DATA:
        log_info("HCI CMD - LE_SET_ADV_DATA");
        hci_send_cmd(&hci_le_set_advertising_data, le_advertisements_data_len, le_advertisements_data);
        __this->init_state = HCI_INIT_W4_LE_SET_ADV_DATA;
        break;
    case HCI_INIT_LE_SET_SCAN_RESPONSE_DATA:
        log_info("HCI CMD - LE_SET_SCAN_RESPONSE_DATA");
        hci_send_cmd(&hci_le_set_scan_response_data, le_scan_response_data_len, le_scan_response_data);
        __this->init_state = HCI_INIT_W4_LE_SET_SCAN_RESPONSE_DATA;
        break;
    case HCI_INIT_LE_SET_ADV_ENABLE:
        log_info("HCI CMD - LE_SET_ADV_ENABLE");
        hci_send_cmd(&hci_le_set_advertise_enable, 1);
        __this->init_state = HCI_INIT_W4_LE_SET_ADV_ENABLE;
        break;
#endif

#ifdef LE_CENTRAL_ENABLE
    case HCI_INIT_LE_SET_SCAN_PARAMETERS:
        log_info("HCI CMD - HCI_INIT_LE_SET_SCAN_PARAMETERS");
        hci_send_cmd(&hci_le_set_scan_parameters, 0, 0x30, 0x30, 0, 0);
        __this->init_state = HCI_INIT_W4_LE_SET_SCAN_PARAMETERS;
        break;

    case HCI_INIT_LE_SET_SCAN_ENABLE:
        log_info("HCI CMD - HCI_INIT_W4_LE_SET_SCAN_ENABLE");
        hci_send_cmd(&hci_le_set_scan_enable, 1, 1);
        __this->init_state = HCI_INIT_W4_LE_SET_SCAN_ENABLE;
        break;
#endif

#endif

#ifdef CLASSIC_INIT_ENABLE
    case HCI_INIT_WRITE_CLASS_OF_DEVICE:
        log_info("HCI CMD - HCI_INIT_SEND_WRITE_CLASS_OF_DEVICE");
        /* hci_send_cmd(&hci_write_class_of_device, 0x002508); */
        hci_send_cmd(&hci_write_class_of_device, 0x240404);
        __this->init_state = HCI_INIT_W4_WRITE_CLASS_OF_DEVICE;
        break;

    case HCI_INIT_WRITE_LOCAL_NAME:
        log_info("HCI CMD - HCI_INIT_SEND_WRITE_LOCAL_NAME");
        hci_send_cmd(&hci_write_local_name, "Joy-Con(R)"/* bt_get_local_name() */);
        __this->init_state = HCI_INIT_W4_WRITE_LOCAL_NAME;
        break;

    case HCI_INIT_WRITE_EIR_DATA:
        log_info("HCI CMD - HCI_INIT_WRITE_EIR_DATA");
        hci_send_cmd(&hci_write_extended_inquiry_response, 1, eir_data);
        __this->init_state = HCI_INIT_W4_WRITE_EIR_DATA;
        break;

#ifdef CLASSIC_RESPONDING_SIDE
    case HCI_INIT_WRITE_SCAN_ENABLE:
        log_info("HCI CMD - HCI_INIT_WRITE_SCAN_ENABLE");
        /* hci_send_cmd(&hci_inquiry, 0x9E8B33L, 0x30, 0xff); */
        hci_send_cmd(&hci_write_scan_enable, 3);
        __this->init_state = HCI_INIT_W4_WRITE_SCAN_ENABLE;
        __this->state = HCI_STATE_WORKING;
        break;
#endif

#ifdef CLASSIC_INITIATING_SIDE
    case HCI_INIT_WRITE_CREATE_CONNECTION:
        set_bt_need_enc(0);
        u8 addr[] = {0xaa, 0xbb, 0xcc, 0x00, 0x00, 0x02};
        hci_send_cmd(&hci_create_connection, addr, 0, 0, 0, 0, 0);
        __this->init_state = HCI_INIT_W4_WRITE_CREATE_CONNECTION;
        break;
#endif

    case HCI_INIT_SEND_ACL_DATA:
        if (g_handle) {
            hci_send_acl_packet_test(); //kick start
            __this->state = HCI_STATE_WORKING;
        } else {
            __this->init_state = HCI_INIT_ACL_DATA_TEST;
        }
        break;

    case HCI_INIT_RECEIVE_ACL_DATA:
        break;

#endif
    }
}

static void hci_init(void)
{
    //HCI
    __this = (struct btstack_handle *)malloc(sizeof(struct btstack_handle));
    ASSERT(__this);
    memset(__this, 0x0, sizeof(struct btstack_handle));

    __this->pHci_buffer = malloc(HCI_MAX_TOTAL_PAYLOAD);
    ASSERT(__this->pHci_buffer);
    memset(__this->pHci_buffer, 0x0, HCI_MAX_TOTAL_PAYLOAD);

    __this->pEVT = lbuf_init(__this->pHci_buffer, HCI_MAX_EVENT_PAYLOAD, 4, 0);
    ASSERT(__this->pEVT);

    __this->state = HCI_STATE_INITIALIZING;

    __this->init_state = HCI_INIT_SEND_RESET;

    /* tws_ble_fast_search_init(); */

    hci_init_state_machine();
}

#define LINK_TYPE_SCO  0x00
#define LINK_TYPE_ACL  0x01
#define LINK_TYPE_ESCO  0x02

static void hci_event_handler(u8 *packet, u16 size)
{
    u8 length = packet[1];
    u16 handle;

    switch (packet[0]) {
    case HCI_EVENT_COMMAND_COMPLETE:
        log_info("HCI_EVENT_COMMAND_COMPLETE");
        break;
    case HCI_EVENT_COMMAND_STATUS:
        log_info("HCI_EVENT_COMMAND_STATUS");
        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        handle = LITTLE_ENDIAN_READ_16(packet, 3);
        log_info("HCI_EVENT_DISCONNECTION_COMPLETE : handle 0x%x / reason 0x%x", handle, packet[5]);
        g_handle = 0;
        break;

    case HCI_EVENT_CONNECTION_REQUEST:
        log_info("HCI_EVENT_CONNECTION_REQUEST");
        /* addr = &packet[2]; */
        if (packet[11] == LINK_TYPE_ACL) {
            u8 addr[6];
            for (u8 i = 0; i < sizeof(addr); i++) {
                addr[sizeof(addr) - 1 - i] = packet[2 + i];
            }

            hci_send_cmd(&hci_accept_connection_request, addr, 0);
        } else {
            ASSERT(0);
        }
        break;

    case HCI_EVENT_CONNECTION_COMPLETE:
        log_info("HCI_EVENT_CONNECTION_COMPLETE");
        handle  = LITTLE_ENDIAN_READ_16(packet, 3);
        log_info("handle : %d / type : %d / encryption_enable : %d", handle, packet[11], packet[12]);
        g_handle = handle;
        break;

    case HCI_EVENT_PAGE_SCAN_REPETITION_MODE_CHANGE:
        log_info("HCI_EVENT_PAGE_SCAN_REPETITION_MODE_CHANGE");
        break;

    case HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE:
        log_info("HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE");
        break;

    case HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS:
        /* log_info("HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS"); */
        break;

    case HCI_EVENT_LE_META: {
        switch (packet[2]) {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            log_info("HCI_SUBEVENT_LE_CONNECTION_COMPLETE");
            break;
        case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
            log_info("HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE");
            break;
        case HCI_SUBEVENT_LE_ADVERTISING_REPORT:
            log_info("HCI_SUBEVENT_LE_ADVERTISING_REPORT");
            break;
        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    if (__this->state == HCI_STATE_INITIALIZING) {
        __this->init_state++;
        hci_init_state_machine();
    }
}

static void hci_acl_handler(u8 *packet, u16 size)
{
    /* log_info("BTSTACK_HCI_ACL : %d", size); */
    /* log_info_hexdump(packet, size); */

#ifdef CLASSIC_RESPONDING_SIDE
    hci_send_acl_packet_test();
#endif

#ifdef CLASSIC_INITIATING_SIDE
    hci_send_acl_packet_test();
#endif

    //To L2cap Layer
}

static void btstack_task(void *p)
{
    int ret;
    int msg[8];

    while (1) {
        /* log_info("Q pend"); */
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        /* log_info("ret : 0x%x", ret); */
        /* log_info("msg : 0x%x", msg[0]); */
        if (ret != OS_TASKQ) {
            continue;
        }
        switch (msg[0]) {
        case BTSTACK_TASK_READY:
            hci_init();
            break;
        case BTSTACK_HCI_EVENT: {
            struct hci_event *p;
            p = lbuf_pop(__this->pEVT, 1);
            hci_event_handler(p->payload, p->length);
            lbuf_free(p);
        }
        break;
        case BTSTACK_HCI_ACL:
            hci_acl_handler((u8 *)msg[1], msg[2]);
            break;
        case BTSTACK_TASK_EXIT:
            os_sem_post((OS_SEM *)msg[1]);
            os_time_dly(10000);
            break;
        }
    }
}

int hci_packet_handler(u8 type, u8 *packet, u16 size)
{
    int err = 0;
    int msg[2];

    /* log_info("HCI PH : 0x%x / %d", packet, size); */
    /* log_info_hexdump(packet, size); */

    switch (type) {
    case HCI_EVENT_PACKET: {
        struct hci_event *p;
        p = lbuf_alloc(__this->pEVT, sizeof(struct hci_event) + size);
        p->length = size;
        memcpy(p->payload, packet, size);
        lbuf_push(p, 1);
    }
    err = os_taskq_post_type(THIS_TASK_NAME, BTSTACK_HCI_EVENT, 0, NULL);
    break;
    case HCI_ACL_DATA_PACKET:
        msg[0] = (int)packet;
        msg[1] = size;
        err = os_taskq_post_type(THIS_TASK_NAME, BTSTACK_HCI_ACL, 2, msg);
        break;
    default:
        return 0;
    }

    if (err != OS_NO_ERR) {
        log_error("OS_ERR : 0x%x", err);
    }

    return err;
}

static void btstack_task_ready(void)
{
    int err;

    err = os_taskq_post_type(THIS_TASK_NAME, BTSTACK_TASK_READY, 0, NULL);

    if (err != OS_NO_ERR) {
        log_error("OS_ERR : 0x%x", err);
    }
}

/* #define APP_BT_MAC_ADDR                 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 */


/* const u8 le_mac_addr[6] = {APP_BT_MAC_ADDR}; */
/* const u8 le_mac_addr[6] = {0x22, 0xfb, 0x6e, 0x83, 0x15, 0x00}; */

int btstack_demo_init()
{
    log_info("bt_get_mac_addr : ");
    log_info_hexdump((u8 *)bt_get_mac_addr(), 6);


    /* le_controller_set_mac((void *)&le_mac_addr); */
    le_controller_set_mac((void *)bt_get_mac_addr());
    lmp_hci_write_local_address((void *)bt_get_mac_addr());

    /* void wdt_close(void); */
    /* wdt_close(); */
    /* btctrler_task_init((void *)hci_transport_usb_instance(), NULL); */
    /* return 0; */

    btctrler_task_init((void *)hci_transport_h4_controller_instance(), NULL);
    while (btctrler_task_ready() == 0) {
        os_time_dly(2);
    }

    task_create(btstack_task, NULL, THIS_TASK_NAME);

    btstack_task_ready();

    return 0;
}


int btstack_exit()
{
    OS_SEM sem;

    btctrler_task_exit();

    os_sem_create(&sem, 0);
    os_taskq_post_type(THIS_TASK_NAME, BTSTACK_TASK_EXIT, 1, (int *)&sem);

    os_sem_pend(&sem, 0);

    log_info("Exit");

    task_kill(THIS_TASK_NAME);

    return 0;
}




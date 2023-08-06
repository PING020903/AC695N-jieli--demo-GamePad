#include "os/os_api.h"
#include "usb/device/usb_stack.h"
#include "usb/device/hid.h"
#include "usb_config.h"

#include "app_config.h"

#ifdef TCFG_USB_SLAVE_USER_HID
#undef TCFG_USB_SLAVE_HID_ENABLE
#define TCFG_USB_SLAVE_HID_ENABLE           0
#endif

#if TCFG_USB_SLAVE_HID_ENABLE

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


#if 1
static const u8 sHIDDescriptor[] = {
//HID
    //InterfaceDeszcriptor:
    USB_DT_INTERFACE_SIZE,      // Length    0x09
    USB_DT_INTERFACE,           // DescriptorType    0x04
    /* 0x04,                      // bInterface number */
    0x00,                       // bInterface number
    0x00,                       // AlternateSetting
    0x02,                       // NumEndpoint
    /* 0x02,                        // NumEndpoint */
    0xff,                       // Class = Human Interface Device
    0x5d,                       // Subclass, 0 No subclass, 1 Boot Interface subclass
    0x01,                       // Procotol, 0 None, 1 Keyboard, 2 Mouse
    0x00,                       // Interface Name

    //HIDDescriptor:
    0x11,                       // bLength
    USB_HID_DT_HID,             // bDescriptorType, HID Descriptor   ((0x01<<5)|0x01) = 0x21
    0x10, 0x01,                 // bcdHID, HID Class Specification release NO.
    0x01,                       // bCuntryCode, Country localization (=none)
    0x25,                       // bNumDescriptors, Number of descriptors to follow
    0x81,                       // bDescriptorType, Report Desc. 0x22, Physical Desc. 0x23
    0x14,                       //LOW(ReportLength)
    0x03,                       //HIGH(ReportLength)
    0x03,
    0x03,
    0x04,
    0x13,
    0x02,
    0x08,
    0x03,
    0x03,

    //EndpointDescriptor:
    USB_DT_ENDPOINT_SIZE,       // bLength  0x07
    USB_DT_ENDPOINT,            // bDescriptorType, Type    0x05
    0x81,                       // bEndpointAddress
    USB_ENDPOINT_XFER_INT,      // Interrupt    0x03
    LOBYTE(MAXP_SIZE_HIDIN), HIBYTE(MAXP_SIZE_HIDIN),// Maximum packet size 0x08, 0x00
    2,     // Poll every 2msec seconds

    0x07,//Length
    0x05,//DescriptorType
    0x02,//EndpointAddress
    0x03,//Interrupt
    0x20, 0x00,//Maximum packet size
    0x08,//Poll every 8msec seconds

    0x09,//Length
    0x04,//DescriptorType
    0x01,//bInterfaces
    0x00,//Alternatce number
    0x02,//NumEndpoeSetting
    0xff,//Classint
    0x5d,//Subclass
    0x03,//Procotol
    0x00,//Interface Name

    0x1b,       // Length
    0x21,       // DescriptorType
    0x00, 0x01, // bcdHID
    0x01,       // bCuntryCode
    0x01,
    0x83,
    0x40,
    0x01,
    0x04,
    0x20,
    0x16, 0x85,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x16, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x07,//Length
    0x05,//DescriptorType
    0x83,//EndpointAddress
    0x03,//Interrupt
    0x20, 0x00,//Maximum packet size
    0x02,//Poll every 2msec seconds

    0x07,//Length
    0x05,//DescriptorType
    0x04,//EndpointAddress
    0x03,//Interrupt
    0x20, 0x00,//Maximum packet size
    0x02,//Poll every 4msec seconds

    0x07,//Length
    0x05,//DescriptorType
    0x85,//EndpointAddress
    0x03,//Interrupt
    0x20, 0x00,//Maximum packet size
    0x04,//Poll every 4msec seconds

    0x07,//Length
    0x05,//DescriptorType
    0x05,//EndpointAddress
    0x03,//Interrupt
    0x20, 0x00,//Maximum packet size
    0x10,//Poll every 4msec seconds

    0x09,//Length
    0x04,//DescriptorType
    0x02,//bInterfaces
    0x00,//Alternatce number
    0x01,//NumEndpoeSetting
    0xff,//Classint
    0x5d,//Subclass
    0x02,//Procotol
    0x00,//Interface Name

    0x09,// Length
    0x21,// DescriptorType
    0x00,// bcdHID
    0x01,// bCuntryCode
    0x01,
    0x22,
    0x86,
    0x07,
    0x00,

    0x07,//Length
    0x05,//DescriptorType
    0x86,//EndpointAddress
    0x03,//Interrupt
    0x20, 0x00,//Maximum packet size
    0x10,//Poll every 16msec seconds
};
#endif // 1
#if 0//旧版本
static const u8 sHIDDescriptor[] = {
//HID
    //InterfaceDeszcriptor:
    USB_DT_INTERFACE_SIZE,     // Length
    USB_DT_INTERFACE,          // DescriptorType
    /* 0x04,                      // bInterface number */
    0x00,                       // bInterface number
    0x00,                      // AlternateSetting
    0x01,                      // NumEndpoint
    /* 0x02,                        // NumEndpoint */
    USB_CLASS_HID,             // Class = Human Interface Device
    0x00,                      // Subclass, 0 No subclass, 1 Boot Interface subclass
    0x00,                      // Procotol, 0 None, 1 Keyboard, 2 Mouse
    0x00,                      // Interface Name

    //HIDDescriptor:
    0x09,                      // bLength
    USB_HID_DT_HID,            // bDescriptorType, HID Descriptor
    0x00, 0x01,                // bcdHID, HID Class Specification release NO.
    0x00,                      // bCuntryCode, Country localization (=none)
    0x01,                       // bNumDescriptors, Number of descriptors to follow
    0x22,                       // bDescriptorType, Report Desc. 0x22, Physical Desc. 0x23
    0,//LOW(ReportLength)
    0, //HIGH(ReportLength)

    //EndpointDescriptor:
    USB_DT_ENDPOINT_SIZE,       // bLength
    USB_DT_ENDPOINT,            // bDescriptorType, Type
    USB_DIR_IN | HID_EP_IN,     // bEndpointAddress
    USB_ENDPOINT_XFER_INT,      // Interrupt
    LOBYTE(MAXP_SIZE_HIDIN), HIBYTE(MAXP_SIZE_HIDIN),// Maximum packet size
    1,     // Poll every 10msec seconds

//@Endpoint Descriptor:
    /* USB_DT_ENDPOINT_SIZE,       // bLength
    USB_DT_ENDPOINT,            // bDescriptorType, Type
    USB_DIR_OUT | HID_EP_OUT,   // bEndpointAddress
    USB_ENDPOINT_XFER_INT,      // Interrupt
    LOBYTE(MAXP_SIZE_HIDOUT), HIBYTE(MAXP_SIZE_HIDOUT),// Maximum packet size
    0x01,                       // bInterval, for high speed 2^(n-1) * 125us, for full/low speed n * 1ms */
};
#endif // 0
#if 0//错误版本
static const uint8_t sHIDDescriptor[ 41 ] = {
	/*配置描述*/
	0x09,					/* 当前长度 */
	0x02,					/* 当前类型: configuration */
	41, 0x00,				/* 0x29, 该配置描述符总长度, 一共两位(低位在前 高位在后) */
	0x02,					/* ID of this configuration, 已经有一个audio配置占据了#1 */
	0x08,					/* Get Configuration 和Set Configuration请求的配置值 */
	0x00,					/* 字符串描述符索引(None: 0x00) */
	0x32,					/* 从总线中获取供电, MaxPower 100 mA */


	/*接口描述*/
	0x09,					/* 当前长度 */
	0x04,					/* 当前类型: Interface descriptor */
	0x00,         			/* 接口数 */
  	0x00,         			/* 备用接口ID(None: 0x00) */
  	0x02,         			/* 端点数目 */
  	0x03,         			/* 接口类型: HID */
  	0x00,         			/* 子接口类型 : 1=BOOT, 0=no boot (是否可以在boot模式下运行) */
  	0x00,         			/* 接口协议 : 0=none, 1=keyboard, 2=mouse */
  	0x00,            		/* 接口字符串描述符(None: 0x00) */


  	/*自定义HID描述*/
  	0x09,         			/* bLength: HID Descriptor size */
  	0x21, 					/* bDescriptorType: HID 0x21 */
  	0x10, 0x01,         	/* bcdHID: HID Class Spec release number HID类发布规格 */
  	0x00,         			/* bCountryCode: Hardware target country */
  	0x01,         			/* bNumDescriptors: Number of HID class descriptors to follow 要遵循的 HID 类描述符数量 */
  	0x22,         			/* bDescriptorType: HID-report 0x22 */
  	0x3b, 0x00,				/* wItemLength: 报告描述标签长度 ReportDescriptor[ 59 ] */


  	/*端点描述*/
  	0x07,          			/* bLength: Endpoint Descriptor size */
  	0x05, 					/* 当前类型: 端点 */
  	0x81,          			/* bEndpointAddress: Endpoint Address (IN) ( 端点地址IN ) */
  	0x03,          			/* bmAttributes: Interrupt endpoint ( 属性: 接口端点 ) */
  	0x40, 0x00,          	/* wMaxPacketSize: 64 Bytes max ( 单次数据包大小: 最大64字节 ) */
  	0x01,          			/* bInterval: Polling Interval (1 ms) 间隔： 轮询间隔 */


  	/*端点描述*/
  	0x07,					/* bLength: Endpoint Descriptor size */
  	0x05,					/* 当前类型: 端点 */
  	0x01,					/*	Endpoint Address (OUT) ( 端点地址OUT ) */
  	0x03,					/* bmAttributes: Interrupt endpoint ( 属性: 接口端点 ) */
  	0x40, 0x00,				/* wMaxPacketSize: 64 Bytes max ( 单次数据包大小: 最大64字节 ) */
  	0x10,					/* bInterval: Polling Interval (16 ms) 间隔： 轮询间隔 */
};
#endif // 0

#if 1//旧版本
static const u8 sHIDReportDesc[] = {
    USAGE_PAGE(1, CONSUMER_PAGE),
    USAGE(1, CONSUMER_CONTROL),
    COLLECTION(1, APPLICATION),

    LOGICAL_MIN(1, 0x00),
    LOGICAL_MAX(1, 0x01),

    USAGE(1, VOLUME_INC),
    USAGE(1, VOLUME_DEC),
    USAGE(1, MUTE),
    USAGE(1, PLAY_PAUSE),
    USAGE(1, SCAN_NEXT_TRACK),
    USAGE(1, SCAN_PREV_TRACK),
    USAGE(1, FAST_FORWARD),
    USAGE(1, STOP),

    USAGE(1, TRACKING_INC),
    USAGE(1, TRACKING_DEC),
    USAGE(1, STOP_EJECT),
    USAGE(1, VOLUME),
    USAGE(2, BALANCE_LEFT),
    USAGE(2, BALANCE_RIGHT),
    USAGE(1, PLAY),
    USAGE(1, PAUSE),

    REPORT_SIZE(1, 0x01),
    REPORT_COUNT(1, 0x10),
    INPUT(1, 0x42),
    END_COLLECTION,

};
#endif // 0
#if 0
static const char sHIDReportDesc[ 59 ] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)
    0x09, 0xbb,                    //   USAGE (Throttle)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x03,              //   LOGICAL_MAXIMUM (1023)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x04,                    //   USAGE_MAXIMUM (Button 4)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0x55, 0x00,                    //   UNIT_EXPONENT (0)
    0x65, 0x00,                    //   UNIT (None)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};
#endif // 0
static u32 get_hid_report_desc_len(u32 index)
{
    u32 len = 0;
    len = sizeof(sHIDReportDesc);
    return len;
}
static void *get_hid_report_desc(u32 index)
{
    u8 *ptr  = NULL;
    ptr = (u8 *)sHIDReportDesc;
    return ptr;
}


static u8 *hid_ep_in_dma;
static u8 *hid_ep_out_dma;
u32 hid_tx_data_my(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, HID_EP_IN, buffer, len);
}
static u32 hid_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, HID_EP_IN, buffer, len);
}
static void hid_rx_data(struct usb_device_t *usb_device, u32 ep)
{
    /*const usb_dev usb_id = usb_device2id(usb_device);
    u32 rx_len = usb_g_intr_read(usb_id, ep, NULL, 64, 0);
    hid_tx_data(usb_device, hid_ep_out_dma, rx_len);*/
}

static void hid_endpoint_init(struct usb_device_t *usb_device, u32 itf)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    usb_g_ep_config(usb_id, HID_EP_IN | USB_DIR_IN, USB_ENDPOINT_XFER_INT, 0, hid_ep_in_dma, MAXP_SIZE_HIDIN);
    usb_enable_ep(usb_id, HID_EP_IN);

    /* usb_g_set_intr_hander(usb_id, HID_EP_OUT, hid_rx_data); */
    /* usb_g_ep_config(usb_id, HID_EP_OUT, USB_ENDPOINT_XFER_INT, 1, ep_buffer, MAXP_SIZE_HIDOUT); */
}
u32 hid_register(const usb_dev usb_id)
{
    printf("---------- 6 ----------\n");
    hid_ep_in_dma = usb_alloc_ep_dmabuffer(usb_id, HID_EP_IN | USB_DIR_IN, MAXP_SIZE_HIDIN);

    /* hid_ep_out_dma = usb_alloc_ep_dmabuffer(usb_id, HID_EP_OUT,MAXP_SIZE_HIDOUT); */
    return 0;
}

void hid_release(const usb_dev usb_id)
{
    return ;
}

static void hid_reset(struct usb_device_t *usb_device, u32 itf)
{
    printf("---------- hid reset 20 ----------\n");
    const usb_dev usb_id = usb_device2id(usb_device);
    log_debug("%s", __func__);
#if USB_ROOT2
    usb_disable_ep(usb_id, HID_EP_IN);
#else
    hid_endpoint_init(usb_device, itf);
#endif
}
static u32 hid_recv_output_report(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 ret = 0;
    u8 read_ep[8];
    u8 mute;
    u16 volume = 0;
    usb_read_ep0(usb_id, read_ep, MIN(sizeof(read_ep), setup->wLength));
    ret = USB_EP0_STAGE_SETUP;
    put_buf(read_ep, 8);


    return ret;
}

static u32 hid_itf_hander(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    printf("---------- 8 ----------\n");
    if (req == -1) {
        return 0;
    }
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 tx_len;
    u8 *tx_payload = usb_get_setup_buffer(usb_device);
    u32 bRequestType = req->bRequestType & USB_TYPE_MASK;
    switch (bRequestType) {
    case USB_TYPE_STANDARD:
        printf("---------- (req->bRequestType) -> hid_itf_handler_caseSTANDARD 9 ----------\n");
        switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR:
            printf("---------- caseSTANDARD -> caseREQ_GET_DESC 13 ----------\n");
            switch (HIBYTE(req->wValue)) {
            case USB_HID_DT_HID:
                printf("---------- IN caseSTANDARD -> caseREQ_GET_DESC -> caseHID_DT_HID -> caseUSB_REQ_GET_DESC 11 ----------\n");
                tx_payload = (u8 *)sHIDDescriptor + USB_DT_INTERFACE_SIZE;
                tx_len = 9;
                tx_payload = usb_set_data_payload(usb_device, req, tx_payload, tx_len);
                tx_payload[7] = LOBYTE(get_hid_report_desc_len(req->wIndex));
                tx_payload[8] = HIBYTE(get_hid_report_desc_len(req->wIndex));
                break;
            case USB_HID_DT_REPORT:
                printf("---------- IN caseSTANDARD -> caseREQ_GET_DESC -> caseHID_DT_REPORT -> caseUSB_HID_DT_REPORT 12 ----------\n");
                hid_endpoint_init(usb_device, req->wIndex);
                tx_len = get_hid_report_desc_len(req->wIndex);
                tx_payload = get_hid_report_desc(req->wIndex);
                usb_set_data_payload(usb_device, req, tx_payload, tx_len);
                break;
            }// USB_REQ_GET_DESCRIPTOR
            break;
        case USB_REQ_SET_DESCRIPTOR:
            printf("---------- caseSTANDARD -> caseREQ_SET_DESC 14 ----------\n");
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        case USB_REQ_SET_INTERFACE:
            printf("---------- caseSTANDARD -> caseREQ_SET_INTERFACE 15 ----------\n");
            if (usb_device->bDeviceStates == USB_DEFAULT) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_ADDRESS) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_CONFIGURED) {
                //只有一个interface 没有Alternate
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            }
            break;
        case USB_REQ_GET_INTERFACE:
            printf("---------- caseSTANDARD -> caseREQ_GET_INTERFACE 16 ----------\n");
            if (req->wLength) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_DEFAULT) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_ADDRESS) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_CONFIGURED) {
                tx_len = 1;
                tx_payload[0] = 0x00;
                usb_set_data_payload(usb_device, req, tx_payload, tx_len);
            }
            break;
        case USB_REQ_GET_STATUS:
            printf("---------- caseSTANDARD -> caseREQ_GET_STATUS 17 ----------\n");
            if (usb_device->bDeviceStates == USB_DEFAULT) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_ADDRESS) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            }
            break;
        }//bRequest @ USB_TYPE_STANDARD
        break;

    case USB_TYPE_CLASS: {
        printf("---------- (req->bRequestType) -> hid_itf_handler_caseTYPE_CALSS 10 ----------\n");
        switch (req->bRequest) {
        case USB_REQ_SET_IDLE:
            printf("---------- caseTYPE_CALSS -> caseREQ_SET_IDLE 18 ----------\n");
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        case USB_REQ_GET_IDLE:
            printf("---------- caseTYPE_CALSS -> caseREQ_GET_IDLE 19 ----------\n");
            tx_len = 1;
            tx_payload[0] = 0;
            usb_set_data_payload(usb_device, req, tx_payload, tx_len);
            break;
        case USB_REQ_SET_REPORT:
            printf("---------- caseTYPE_CALSS -> caseREQ_SET_REPORT 20 ----------\n");
            usb_set_setup_recv(usb_device, hid_recv_output_report);
            break;
        }//bRequest @ USB_TYPE_CLASS
    }
    break;
    }
    printf("---------- hid_itf_handle END 22 ----------\n");
    return 0;
}

u32 hid_desc_config(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{
    printf("---------- 7 ----------\n");
    log_debug("hid interface num:%d\n", *cur_itf_num);
    u8 *_ptr = ptr;
    memcpy(ptr, sHIDDescriptor, sizeof(sHIDDescriptor));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, hid_itf_hander) != *cur_itf_num) {
        ASSERT(0, "hid set interface_hander fail");
    }
    if (usb_set_reset_hander(usb_id, *cur_itf_num, hid_reset) != *cur_itf_num) {
        ASSERT(0, "hid set interface_reset_hander fail");
    }

    ptr[USB_DT_INTERFACE_SIZE + 7] = LOBYTE(get_hid_report_desc_len(0));
    ptr[USB_DT_INTERFACE_SIZE + 8] = HIBYTE(get_hid_report_desc_len(0));
    *cur_itf_num = *cur_itf_num + 1;
    return sizeof(sHIDDescriptor) ;
}

void hid_key_handler(struct usb_device_t *usb_device, u32 hid_key)
{
    const usb_dev usb_id = usb_device2id(usb_device);

    u16 key_buf = hid_key;
    hid_tx_data(usb_device, (const u8 *)&key_buf, 2);//usb_device, 取地址后强转key_buf(该key_buf由hid_key赋值而来), 长度2
    os_time_dly(2);
    key_buf = 0;
    hid_tx_data(usb_device, (const u8 *)&key_buf, 2);
}


struct hid_button {
    u8 report_id;
    u8 button1: 1;
    u8 button2: 1;
    u8 button3: 1;
    u8 no_button: 5;
    u8 X_axis;
    u8 Y_axis;
};
struct hid_button hid_key;
void hid_test(struct usb_device_t *usb_device)
{
    static u32 tx_count = 0;

    hid_key_handler(usb_device, BIT(tx_count));
    tx_count ++;
    if (BIT(tx_count) > USB_AUDIO_PAUSE) {
        tx_count = 0;
    }
}
#else
void hid_key_handler(struct usb_device_t *usb_device, u32 hid_key)
{

}
#endif

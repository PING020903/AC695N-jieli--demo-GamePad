#include "os/os_api.h"
#include "usb/device/usb_stack.h"
#include "usb/device/hid.h"
#include "usb_config.h"

#include "app_config.h"
#include "generic/gpio.h"

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
#define PS3_SUPPORT 1
#include "debug.h"


#if ENABLE  // 原文
#if ENABLE  // 原先版本
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
#endif


#if ENABLE  // 原先版本
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
#endif
#endif
/****************************************** PS3 ************************************************/
#if PS3_SUPPORT
static const unsigned char ps3_configuration_descriptor[] =
{
    USB_DT_INTERFACE_SIZE,  // Size of this descriptor in bytes.
    USB_DT_INTERFACE,       // Descriptor type( interface ).
    0x00,                   // Number of this interface.
    0x00,                   // Value used to select alternate setting for the interface identified in the prior field.
    0x02,                   // Number of endpoints used by this interface.
    0x03,                   // Class code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    0x00,                   // Index of string descriptor describing this interface.

/* ps3_endpointOUT_descriptor */
	0x09,                   // Size of this descriptor in bytes.
	0x21,                   // Descriptor type( HID ).
	0x10, 0x01,             // HID specification release number in binary-coded-decimal.
	0x00,                   // Numeric expression identifying country code of the localized hardware.
	0x01,                   // Numeric expression identifying the number of class descriptor.
	0x22,                   // Constant name identifying type of class descriptor.
	0x94, 0x00,             // Numeric expression that is the total size of the report descriptor.

/* ps3_endpointOUT_descriptor */
	0x07,                    // Size of this descriptor in bytes.
	0x05,                    // Descriptor type( endpoint ).
	HID_EP_OUT,              // The address of the endpoint on the USB device described by this descriptor.
	0x03,                    // This field describes the endpoint's attributes when it is configured using the bConfigurationValue.
	0x40, 0x00,              // Maximum packet size this endpoint is capable of sending or receiving when this configuration is selected.
	0x04,                    // Interval for polling endpoint for data transfers( 4ms/unit ).

/* ps3_endpointIN_descriptor */
	0x07,
	0x05,
	0x81,
	0x03,
	0x40, 0x00,
	0x04
};

//HID Report Descriptor
static const unsigned char  HID_ReportDescriptor[] = {
		0x05, 0x01,
		0x09, 0x04,
		0xA1, 0x01,
		0xA1, 0x02,
		0x85, 0x01,
		0x75, 0x08,
		0x95, 0x01,
		0x15, 0x00,
		0x26, 0xFF,
		0x00, 0x81,
		0x03, 0x75,
		0x01, 0x95,
		0x13, 0x15,
		0x00, 0x25,
		0x01, 0x35,
		0x00, 0x45,
		0x01, 0x05,
		0x09, 0x19,
		0x01, 0x29,
		0x13, 0x81,
		0x02, 0x75,
		0x01, 0x95,
		0x0D, 0x06,
		0x00, 0xFF,
		0x81, 0x03,
		0x15, 0x00,
		0x26, 0xFF,
		0x00, 0x05,
		0x01, 0x09,
		0x01, 0xA1,
		0x00, 0x75,
		0x08, 0x95,
		0x04, 0x35,
		0x00, 0x46,
		0xFF, 0x00,
		0x09, 0x30,
		0x09, 0x31,
		0x09, 0x32,
		0x09, 0x35,
		0x81, 0x02,
		0xC0, 0x05,
		0x01, 0x75,
		0x08, 0x95,
		0x27, 0x09,
		0x01, 0x81,
		0x02, 0x75,
		0x08, 0x95,
		0x30, 0x09,
		0x01, 0x91,
		0x02, 0x75,
		0x08, 0x95,
		0x30, 0x09,
		0x01, 0xB1,
		0x02, 0xC0,
		0xA1, 0x02,
		0x85, 0x02,
		0x75, 0x08,
		0x95, 0x30,
		0x09, 0x01,
		0xB1, 0x02,
		0xC0, 0xA1,
		0x02, 0x85,
		0xEE, 0x75,
		0x08, 0x95,
		0x30, 0x09,
		0x01, 0xB1,
		0x02, 0xC0,
		0xA1, 0x02,
		0x85, 0xEF,
		0x75, 0x08,
		0x95, 0x30,
		0x09, 0x01,
		0xB1, 0x02,
		0xC0, 0xC0
};
static const unsigned char Report01[] = {
		0x00, 0x01,
		0x03, 0x00,
		0x05, 0x0C,
		0x01, 0x02,
		0x18, 0x18,
		0x18, 0x18,
		0x09, 0x0A,
		0x10, 0x11,
		0x12, 0x13,
		0x00, 0x00,
		0x00, 0x00,
		0x04, 0x00,
		0x02, 0x02,
		0x02, 0x02,
		0x00, 0x00,
		0x00, 0x04,
		0x04, 0x04,
		0x04, 0x00,
		0x00, 0x02,
		0x01, 0x02,
		0x00, 0x64,
		0x00, 0x17,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00

};
static unsigned char ReportF2[] = {
		0xF2, 0xFF,
		0xFF, 0x00,
		0x04, 0x32,
		0x93, 0xB2,
		0x63, 0x25,
		0x00, 0x03,
		0x50, 0x81,
		0xD8, 0x01,
		0x8A, 0x13,
		0x00, 0x00,
		0x00, 0x00,
		0x04, 0x00,
		0x02, 0x02,
		0x02, 0x02,
		0x00, 0x00,
		0x00, 0x04,
		0x04, 0x04,
		0x04, 0x00,
		0x00, 0x02,
		0x01, 0x02,
		0x00, 0x64,
		0x00, 0x17,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00
};
static const unsigned char ReportF5[] = {
		0x01, 0x00,
		0xC0, 0x14,
		0x3D, 0x1A,
		0xD2, 0x05,
		0x21, 0x1A,
		0x00, 0x03,
		0x50, 0x81,
		0xD8, 0x01,
		0x8A, 0x13,
		0x00, 0x00,
		0x00, 0x00,
		0x04, 0x00,
		0x02, 0x02,
		0x02, 0x02,
		0x00, 0x00,
		0x00, 0x04,
		0x04, 0x04,
		0x04, 0x00,
		0x00, 0x02,
		0x01, 0x02,
		0x00, 0x64,
		0x00, 0x17,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00
};
static const unsigned char ReportF7[] = {
		0x01, 0x00,
		0xF7, 0x02,
		0xFD, 0x01,
		0xEE, 0xFF,
		0x10, 0x12,
		0x00, 0x02,
		0xE5, 0x01,
		0xFD, 0x00,
		0x00, 0x02,
		0x02, 0x02,
		0x00, 0x03,
		0x00, 0x00,
		0x02, 0x00,
		0x00, 0x02,
		0x62, 0x01,
		0x02, 0x01,
		0x5E, 0x00,
		0x32, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x04,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00
};
static const unsigned char ReportF8[] = {
		0x00, 0x01,
		0x00, 0x00,
		0x00, 0x14,
		0x00, 0x00,
		0x00, 0x00,
		0x33, 0x02,
		0xC4, 0x01,
		0xED, 0x00,
		0x00, 0x02,
		0x02, 0x02,
		0x00, 0x03,
		0x00, 0x00,
		0x02, 0x00,
		0x00, 0x02,
		0x62, 0x01,
		0x02, 0x01,
		0x5E, 0x00,
		0x32, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x04,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00
};
static const unsigned char ReportEF1[] = {
		0x00, 0xEF,
		0x04, 0x00,
		0x05, 0x03,
		0x01, 0xA0,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x02,
		0x03, 0x01,
		0x95, 0x02,
		0x05, 0x01,
		0x92, 0x02,
		0x00, 0x01,
		0x8E, 0x01,
		0xF2, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x04,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00
};
static const unsigned char ReportEF2[] = {
		0x00, 0xEF,
		0x04, 0x00,
		0x05, 0x03,
		0x01, 0xB0,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x02,
		0x03, 0x01,
		0x95, 0x02,
		0x05, 0x01,
		0x92, 0x02,
		0x00, 0x01,
		0x8E, 0x01,
		0xF2, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x04,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00
};

static u8* ps3_ep_in_dma;
static u8* ps3_ep_out_dma;
u8 ps3_read_ep[64];
u8 ps3_out_DMA[64];
static unsigned char report_EF_flag[2] = { 0 };

static void* ps3_report_point(int index)
{
	if (index == 0x0301)
		return (unsigned char*)Report01;
	if (index == 0x03f2)
		return (unsigned char*)ReportF2;
	if (index == 0x03f5)
		return (unsigned char*)ReportF5;
	if (index == 0x03f7)
		return (unsigned char*)ReportF7;
	if (index == 0x03f8)
		return (unsigned char*)ReportF8;
	if (index == 0x03ef) {
		if (ps3_read_ep[4] == 0x03 && ps3_read_ep[5] == 0x01 && ps3_read_ep[6] == 0xa0)
			return (unsigned char*)ReportEF1;
		if (ps3_read_ep[4] == 0x03 && ps3_read_ep[5] == 0x01 && ps3_read_ep[6] == 0xb0)
			return (unsigned char*)ReportEF2;
	}
}
static int ps3_report_len(int index)
{
	if (index == 0x0301)
		return sizeof(Report01);
	if (index == 0x03f2)
		return sizeof(ReportF2);
	if (index == 0x03f5)
		return sizeof(ReportF5);
	if (index == 0x03f7)
		return sizeof(ReportF7);
	if (index == 0x03f8)
		return sizeof(ReportF8);
	if (index == 0x03ef) {
		if (ps3_read_ep[4] == 0x03 && ps3_read_ep[5] == 0x01 && ps3_read_ep[6] == 0xa0)
			return sizeof(ReportEF1);
		if (ps3_read_ep[4] == 0x03 && ps3_read_ep[5] == 0x01 && ps3_read_ep[6] == 0xb0)
			return sizeof(ReportEF2);
	}
}


// 配置PS3的USB端点缓冲区
u32 ps3_register(const usb_dev usb_id)
{
	for (int i = 4; i <= 9; i++)	// 修改疑似序列号的data, 因为相同的情况下会导致手柄控制冲突
		ReportF2[i] = rand32() % 255;

    ps3_ep_in_dma = usb_alloc_ep_dmabuffer(usb_id, (0x01 | 0x80), MAXP_SIZE_HIDIN);
    if (ps3_ep_in_dma == NULL)
        log_error(" 'ps3_ep_in_dma' is NULL");

    ps3_ep_out_dma = usb_alloc_ep_dmabuffer(usb_id, (HID_EP_OUT), MAXP_SIZE_HIDOUT);
    if (ps3_ep_out_dma == NULL)
        log_error(" 'ps3_ep_out_dma' is NULL");
    return 0;
}
static u32 ps3_tx_data(struct usb_device_t* usb_device, const u8* buffer, u32 len)
{
	//log_debug("---------- %s ----------", __func__);
	const usb_dev usb_id = usb_device2id(usb_device);
	return usb_g_intr_write(usb_id, 0x01, buffer, len);
}
static usb_interrupt ps3_rx_data(struct usb_device_t* usb_device, u32 ep)
{
	//log_debug("---------- %s ----------", __func__);
	const usb_dev usb_id = usb_device2id(usb_device);
	u32 rx_len = usb_g_intr_read(usb_id, ep, ps3_out_DMA, 64, 0);

	//ps3_tx_data(usb_device, ps3_ep_out_dma, rx_len);

	//usb_g_intr_write(usb_id, 0x01, fraud_temp, 0x14);
	//usb_g_intr_write(usb_id, 0x01, temp, 0x03);

}
/* 一次将所有使用的端点初始化 */
static void ps3_endpoint_init(struct usb_device_t* usb_device, u32 itf)
{
	printf("---------- %s ----------\n", __func__);
	const usb_dev usb_id = usb_device2id(usb_device);
	memset(ps3_ep_out_dma, 0, MAXP_SIZE_HIDOUT);
	usb_g_ep_config(usb_id, (HID_EP_OUT), USB_ENDPOINT_XFER_INT, 1, ps3_ep_out_dma, MAXP_SIZE_HIDOUT);
	usb_g_set_intr_hander(usb_id, (HID_EP_OUT), ps3_rx_data);
	usb_enable_ep(usb_id, 2);

	memset(ps3_ep_in_dma, 0, MAXP_SIZE_HIDIN);
	usb_g_ep_config(usb_id, (0x01 | 0x80), USB_ENDPOINT_XFER_INT, 0, ps3_ep_in_dma, MAXP_SIZE_HIDIN);
	usb_enable_ep(usb_id, 1);
}
static void ps3_reset_hander(struct usb_device_t* usb_device, u32 itf)
{
	log_debug("---------- %s ----------", __func__);
	extern u8 usb_reset_count;
	usb_reset_count++;
	const usb_dev usb_id = usb_device2id(usb_device);
	//ps3_endpoint_init(usb_device, itf);
}

unsigned char get_report_f7_FLAG = 0;
unsigned char PS3_host_flag = 0;
unsigned char ps3_player_ID;
//static unsigned char arr_OUTrecv[64];
static u32 ps3_recv_output_report(struct usb_device_t* usb_device, struct usb_ctrlrequest* setup)
{
	gpio_direction_output(IO_PORTA_03, 1);
	const usb_dev usb_id = usb_device2id(usb_device);
	usb_read_ep0(usb_id, ps3_read_ep, MIN(sizeof(ps3_read_ep), setup->wLength));
	//printf("USB_REQ_SET_REPORT %x, %x, %x", ps3_read_ep[4], ps3_read_ep[5], ps3_read_ep[6]);
	unsigned char temp = ps3_read_ep[9] << 4;
	u8 four = (ps3_read_ep[9] >> 4);	// 獲取高4位, 为1, 则係4号
	u8 ID_Num = (temp >> 4);	// 獲取低4位
	switch (ID_Num)
	{
	case 0x00: {
		if (four)
			four = 4;
		ID_Num = four + 0;
	}break;
	case 0x02: {
		if (four)
			four = 4;
		ID_Num = four + 1;
	}break;
	case 0x04: {
		if (four)
			four = 4;
		ID_Num = four + 2;
	}break;
	case 0x08: {
		if (four)
			four = 4;
		ID_Num = four + 3;
	}break;
	default:
		ID_Num = 1;
		break;
	}
	ps3_player_ID = ID_Num;
	gpio_direction_output(IO_PORTA_03, 0);
	return  USB_EP0_STAGE_SETUP;
}
static u32 ps3_interface_hander(struct usb_device_t* usb_device, struct usb_ctrlrequest* setup)
{
	u8 mute;
	extern unsigned char timer_send_flag;	// from my_put_test.c
	//timer_send_flag = 1;	// 停止手柄线程的定时器发送任务队列
	//log_debug("---------- %s ----------", __func__);
	//timer_send_flag = 0;
	const usb_dev usb_id = usb_device2id(usb_device);
	u32 tx_len;
	u8* tx_payload = usb_get_setup_buffer(usb_device);
	u32 bRequestType = setup->bRequestType & USB_TYPE_MASK;
	u32 ret = 0;

	/*根据例程, 三种情况皆为返回空包*/
	switch (setup->bRequest) {
	case USB_REQ_SET_INTERFACE: {
		/*当主机请求set_Interface的时候，确实是返回空包，端点就绪时候返回NAK, 未就绪时候返回STALL*/
		usb_set_setup_phase(usb_device, USB_EP0_STAGE_IN);	// no alt setting

	}break;
	case USB_REQ_GET_INTERFACE: {
		if (setup->wValue || (setup->wLength != 1)) {
			usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
		}
		else if (usb_device->bDeviceStates == USB_DEFAULT) {
			usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
		}
		else if (usb_device->bDeviceStates == USB_ADDRESS) {
			usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
		}
		else if (usb_device->bDeviceStates == USB_CONFIGURED) {
			tx_len = 1;
			tx_payload[0] = 0x00;
			usb_set_data_payload(usb_device, setup, tx_payload, tx_len);
		}
	}break;
	case USB_REQ_GET_DESCRIPTOR: {
		switch (setup->wValue)
		{
		case /*USB_HID_DT_REPORT*/0x2200: {
			usb_set_data_payload(usb_device, setup, HID_ReportDescriptor, sizeof(HID_ReportDescriptor));
			ps3_endpoint_init(usb_device, 0);
			get_report_f7_FLAG = 1;
		}break;
		case USB_REQ_GET_PROTOCOL: {
			const unsigned char protocl[] = { 0x04, 0x03, 0x09, 0x04 };
			usb_set_data_payload(usb_device, setup, protocl, sizeof(protocl));
		}break;
		default:
			break;
		}
	}break;
	case USB_REQ_GET_REPORT: {
		ps3_endpoint_init(usb_device, 0);
		switch (setup->wValue)
		{
		case 0x0301: {
			usb_set_data_payload(usb_device, setup, ps3_report_point(setup->wValue), ps3_report_len(setup->wValue));
		}break;
		case 0x03f2: {
			usb_set_data_payload(usb_device, setup, ps3_report_point(setup->wValue), ps3_report_len(setup->wValue));
		}break;
		case 0x03f5: {
			usb_set_data_payload(usb_device, setup, ps3_report_point(setup->wValue), ps3_report_len(setup->wValue));
		}break;
		case 0x03f7: {
			usb_set_data_payload(usb_device, setup, ps3_report_point(setup->wValue), ps3_report_len(setup->wValue));
			get_report_f7_FLAG = 1;
		}break;
		case 0x03f8: {
			usb_set_data_payload(usb_device, setup, ps3_report_point(setup->wValue), ps3_report_len(setup->wValue));
		}break;
		case 0x03ef: {
			usb_set_data_payload(usb_device, setup, ps3_report_point(setup->wValue), ps3_report_len(setup->wValue));
		}break;
		default:
			break;
		}
	}break;
	case USB_REQ_SET_REPORT: {
		PS3_host_flag = 1;	// PS3平臺特徵
		usb_set_setup_recv(usb_device, ps3_recv_output_report);
		/*log_debug("---------- REQUEST SET REPORT ----------\n %x %x, %x %x, %x %x, %x %x, %x %x,\n %x %x, %x %x, %x %x, %x %x, %x %x,\n %x %x,\
 %x %x, %x %x, %x %x, %x %x,\n %x %x, %x %x, %x %x, %x %x, %x %x,\n %x %x, %x %x, %x %x, %x %x, %x %x", ps3_read_ep[0], ps3_read_ep[1],
			ps3_read_ep[2], ps3_read_ep[3], ps3_read_ep[4], ps3_read_ep[5], ps3_read_ep[6], ps3_read_ep[7], ps3_read_ep[8], ps3_read_ep[9],
			ps3_read_ep[10], ps3_read_ep[11], ps3_read_ep[12], ps3_read_ep[13], ps3_read_ep[14], ps3_read_ep[15], ps3_read_ep[16], ps3_read_ep[17],
			ps3_read_ep[18], ps3_read_ep[19], ps3_read_ep[20], ps3_read_ep[21], ps3_read_ep[22], ps3_read_ep[23], ps3_read_ep[24], ps3_read_ep[25],
			ps3_read_ep[26], ps3_read_ep[27], ps3_read_ep[28], ps3_read_ep[29], ps3_read_ep[30], ps3_read_ep[31], ps3_read_ep[32], ps3_read_ep[33],
			ps3_read_ep[34], ps3_read_ep[35], ps3_read_ep[36], ps3_read_ep[37], ps3_read_ep[38], ps3_read_ep[39], ps3_read_ep[40], ps3_read_ep[41],
			ps3_read_ep[42], ps3_read_ep[43], ps3_read_ep[44], ps3_read_ep[45], ps3_read_ep[46], ps3_read_ep[47], ps3_read_ep[48], ps3_read_ep[49]);*/
	}break;
	default:
		ret = 1;
		break;
	}
	return ret;
}
u32 hid_my_ps3_0(const usb_dev usb_id, u8* ptr, u32* cur_itf_num)
{
	int i = 0;
	u8* tptr = ptr;
	u32 offset;
	u32 frame_len;
	log_debug("---------- %s ----------", __func__);
	log_debug("my device:%d\n", *cur_itf_num);

	memcpy(tptr, (u8*)ps3_configuration_descriptor, sizeof(ps3_configuration_descriptor));
	if (usb_set_interface_hander(usb_id, *cur_itf_num, ps3_interface_hander) != *cur_itf_num) {/*进入ASSERT宏, cpu_reset或者打印debug*/
			ASSERT(0, "PS3 set interface_hander fail");
	}
	if (usb_set_reset_hander(usb_id, *cur_itf_num, ps3_reset_hander) != *cur_itf_num) {
			ASSERT(0, "PS3 set interface_reset_hander fail");
	}

	(*cur_itf_num)++;
	i = sizeof(ps3_configuration_descriptor);
	return i;
}
#endif
/***********************************************************************************************/
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
static u32 hid_recv_output_report(struct usb_device_t* usb_device, struct usb_ctrlrequest* setup)
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

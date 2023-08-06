#ifndef  __USB_STACK_H__
#define  __USB_STACK_H__
#include "typedef.h"
#include "asm/usb.h"
#include "usb/ch9.h"
#include "usb/usb_phy.h"
#include "usb/otg.h"


#define     MAX_INTERFACE_NUM       6
#define     USB_SUSPEND_RESUME      0
#define     USB_SETUP_SIZE         (512)

#if 0
#define     USB_ATTACHED        BIT(0)
#define     USB_POWERED         BIT(1)
#define     USB_DEFAULT         BIT(2)
#define     USB_ADDRESS         BIT(3)
#define     USB_CONFIGURED      BIT(4)
#define     USB_SUSPENDED       BIT(5)
#else
enum {
    USB_ATTACHED,
    USB_POWERED,
    USB_DEFAULT,
    USB_ADDRESS,
    USB_CONFIGURED,
    USB_SUSPENDED
};
#endif
struct usb_device_t {
    u8 baddr;
    u8 bsetup_phase;          //ep0 setup状态机
    u16 wDataLength;    //ep0 setup data stage数据长度

    u8 *setup_buffer;   //本次传输的bufer地址
    u8 *setup_ptr;      //当前传输的位置
    u32(*setup_hook)(struct usb_device_t *, struct usb_ctrlrequest *);
    u32(*setup_recv)(struct usb_device_t *, struct usb_ctrlrequest *);

    u8 bDeviceStates;
    u8 bDataOverFlag;    //ep0 0包标识
    u8 wDeviceClass;    // 设备类
    u8 bRemoteWakup: 1;
    u8 baddr_config: 1;
#if USB_MAX_HW_NUM == 2
    u8 usb_id: 1;
    u8 res: 5;
#else
    u8 res: 6;
#endif
};

typedef u32(*itf_hander)(struct usb_device_t *usb_device, struct usb_ctrlrequest *);
typedef void(*itf_reset_hander)(struct usb_device_t *, u32 itf);
typedef void(*usb_interrupt)(struct usb_device_t *, u32 ep);
typedef u32(*desc_config)(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);

struct usb_setup_t {
    struct usb_device_t usb_device;
    struct usb_ctrlrequest request;
    itf_hander interface_hander[MAX_INTERFACE_NUM];
    itf_reset_hander reset_hander[MAX_INTERFACE_NUM];
} __attribute__((aligned(4)));

const usb_dev usb_device2id(const struct usb_device_t *usb_device);
struct usb_device_t *usb_id2device(const usb_dev usb_id);
void usb_control_transfer(struct usb_device_t *usb_device);                             //USB控制传输
void usb_device_set_class(struct usb_device_t *usb_device, u32 class_config);           //USB设备套装类
u32 usb_g_set_intr_hander(const usb_dev usb_id, u32 ep, usb_interrupt hander);          //USB_g_设置内部处理 ?
u32 usb_set_interface_hander(const usb_dev usb_id, u32 itf_num, itf_hander hander);     //USB设置接口处理
void usb_add_desc_config(const usb_dev usb_id, u32 index, const desc_config desc);      //USB添加描述配置
const u8 *usb_get_config_desc();                                                        //USB获取配置描述
u32 usb_set_reset_hander(const usb_dev usb_id, u32 itf_num, itf_reset_hander hander);   //USB设置复位处理
void usb_reset_interface(struct usb_device_t *usb_device);                              //USB复位接口
void usb_set_setup_recv(struct usb_device_t *usb_device, void *recv);                   //USB设置回传
void usb_set_setup_hook(struct usb_device_t *usb_device, void *hook);                   //USB设置钩子
int usb_device_mode(const usb_dev usb_id, const u32 class);                             //USB设备模式
u32 usb_otg_sof_check_init(const usb_dev id);                                           //USB_OTG帧起始位检查初始化
void usb_setup_init(const usb_dev usb_id, void *ptr, u8 *setup_buffer);                 //USB初始化设置
u32 usb_setup_release(const usb_dev usb_id);                                            //USB释放(解除)设置
u8 *usb_set_data_payload(struct usb_device_t *usb_device, struct usb_ctrlrequest *req, const void *data, u32 len);//USB设置数据有效负荷  ?(2023.07.14根据目前我对代码的分析,貌似是用以发送数据给主机的)
void usb_set_setup_phase(struct usb_device_t *usb_device, u8 setup_phase);              //USB设置阶段
void dump_setup_request(const struct usb_ctrlrequest *request);                         //跳出设置请求
void user_setup_filter_install(struct usb_device_t *usb_device);                        //用户设置过滤器安装
void usb_ep_enable(const usb_dev usb_id, u32 ep, u32 is_enable);                        //USB端点(endpoint)使能
void *usb_get_setup_buffer(const struct usb_device_t *usb_device);                      //USB获取设置缓冲区
u32 usb_root2_testing();

extern void usb_start();
extern void usb_stop();
extern void usb_pause();

/* #define usb_add_desc_config(fn) \                                    */
/*     const desc_config usb_desc_config##fn sec(.usb.desc_config) = fn */

#endif  /*USB_STACK_H*/

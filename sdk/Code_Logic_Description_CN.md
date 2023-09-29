# 关于该项目的说明

该项目是一个仿照Xbox360手柄做的，插入PC会显示该手柄为Xbox360手柄，暂未对Xbox360主机做适配，支持PS3

使用的MCU为杰理科技的AC6951C，该SDK是杰理官方的DEMO project ( version: soundbox )，带了RTOS非裸机

使用USB2.0与主机PC进行通讯，暂未启用Bluetooth功能，也未有市面上手柄音频输出的功能

#### 杰理文档链接

1. [杰理文档系统 (zh-jieli.com)](https://doc.zh-jieli.com/vue/#/docs/ac63)

2. [欢迎使用杰理AC63 开源项目文档 — JL Project Documentation (zh-jieli.com)](https://doc.zh-jieli.com/AC63/zh-cn/master/index.html)

3. https://github.com/Jieli-Tech/fw-AC63_BT_SDK

4. [fw-AC63_BT_SDK: Firmware for Generic Bluetooth SDK（AC63 series）, Support AC631N/AC635N/AC636N/AC637N/AC632N, compatible with AC69 series without audio support. (gitee.com)](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK)

#### 所具备的功能：

1. 方向键：上下左右；

2. A键，B键，X键，Y键；

3. 左摇杆，右摇杆；

4. 左扳机，右扳机；

5. L1键，R1键；

6. back键，start键，XboxBar键；

7. 连发按键（可对普通按键连发状态使能）；

8. 按键记录宏按键（可以记录一定按次数内的按键状态）；
   
   

## OS启动文件路径

> sdk\apps\dongle\app_main.c
> 
> sdk\apps\dongle\app_dongle.c

*其中 app_main.c 负责注册OS状态，到 app_dongle.c 通过状态机判断状态是否启动OS，启动OS后可以开始执行任务初始化函数*

## 主要逻辑实现文件路径

> sdk\apps\dongle\my_put_test.c
> 
> sdk\apps\dongle\my_put_test.h

## USB交互实现文件路径

> sdk\apps\common\usb\device\task_pc.c
> 
> sdk\apps\common\usb\device\descriptor.c
> 
> sdk\apps\common\usb\device\usb_device.c
> 
> sdk\apps\common\usb\device\uac1.c
> 
> sdk\apps\common\usb\device\hid.c
> 
> 对于USB交互实现，当前所列举的这几个文件均有改动，具体实现实际上依赖整个 sdk\apps\common\usb\device 目录，建议该目录下的文件都看



## 《代码分析》

### 大体逻辑

如下图，具体参考demo project中的*程序范例*

![_1657862246_1693050692149_1693050692000_wifi_0](https://github.com/PING020903/AC695N-demo-xbox360-WindowsPC_only/assets/88314322/efbbaab8-897a-46be-906f-eb54580993d5)


### 宏开关

```c
/**************define-switch, 宏开关**************/
#define READ_KEY            1   // 读按键
#define MERGE_KEY           1   // 合并键值
#define MOVEMENTS_SEND      1   // 键值变动发送

#define TRIGGER             1   // 扳机
#define LEFT_TRIGGER        1   // 左扳机
#define RIGHT_TRIGGER       1   // 右扳机
#define PWM_MOTOR           1   // PWM马达震动
#define LEFT_MOTOR          1   // 左马达
#define RIGHT_MOTOR         1   // 右马达

#define LEFT_ROCKER         1   // 左摇杆
#define LEFT_ROCKER_BUFFER  1   // 左摇杆缓冲区
#define LEFT_ROCKER_X_AXIS  1   // 左摇杆X轴
#define LEFT_ROCKER_Y_AXIS  1   // 左摇杆Y轴

#define RIGHT_ROCKER        1   // 右摇杆
#define RIGHT_ROCKER_BUFFER 1   // 右摇杆缓冲区
#define RIGHT_ROCKER_X_AXIS 1   // 右摇杆X轴
#define RIGHT_ROCKER_Y_AXIS 1   // 右摇杆Y轴

#define SUCCESSIVE_PRESS    1   // 连点功能
#define VM_TEST             1   // VM 区域读写
#define CHECK_MEMORY        0   // 打印实时内存信息

#define FUNC_TIMESTAMP      0   // 粗略测量函数执行时间

#define THREAD_CREATE       1   // 线程创建
#define MAIN_TIMER          1   // 主要定时器
#define PWM_TIEMR           1   // PWM定时器
#define SPECIAL_FUNC_TIMER  1   // 特殊功能定时器
#define MY_PRINTF           0   // 我的打印
#define PWM_INIT            1   // PWM初始化

#define RECORD_MOVEMENT     1   // 功能: 记录一定时间内的键值
#define MY_LIST             0   // 用不了malloc(), 转用planA: 数组, (planB: 链表)我用不了
#define MY_ARRAY            1   // 记录一定时间内的键值( 数组方式存储 )
#define RIGHT_ROCKER_PRINT  0
#define LEFT_ROCKER_PRINT   0
#define PS3_KEY_PRINT       1
/***********************************************/
```

给宏配置0则为关闭这个功能，配置1则开启。

该功能做的较粗略，还没有一一校验过是否能单独启动或单独关闭，可能在编译期间会报变量未定义（宏会将变量也跟着一齐关闭）的错误，请自行定义。

### 函数说明

以下一一列出了函数实现的返回值类型与参数以及修饰关键字

```c
static inline unsigned int USB_TX_data(const usb_dev usb_id, const u8 *buffer, unsigned int len);
static inline unsigned char merge_value(unsigned char all_key, unsigned char key, unsigned int bit);

/***************************************** xbox360(PC) function *****************************************/
static inline void send_data_to_host(void);
void my_read_key(void);
void read_trigger_value(void);
void left_read_rocker(void);
void right_read_rocker(void);
void my_led_function(void);
void connect_flicker(void);
void records_movement(void);

/***************************************** PS3 function *****************************************/
void ps3_read_key(void);
void ps3_read_trigger(void);
void ps3_left_read_rocker(void);
void ps3_right_read_rocker(void);
void ps3_player_led(void);
void ps3_PWM_shake(void);


void *my_task(void *p_arg);
void my_button_init(void);
void my_PWM_output_init(void);
static inline void *my_timer_task(void *p_arg);
static inline void *led_timer_task(void *p_arg);
static inline void *SpecialFunc_timer_task(void *p_arg);
void my_task_init(void);
```
---
## common function   公共函数
#### void my_task_init(void)
该函数用以将创建该线程与OS定时器以及一些IO的初始化工作，当执行完毕后不再执行。

#### static inline void *my_timer_task(void *p_arg)
OS定时器的回调函数，可传参，可以不用 *ststic* 与 *inline* 修饰只是为了增加一点点执行速度。定时器计时到了就会执行该函数。
这是当前项目中的主要定时器。

#### static inline void *led_timer_task(void *p_arg)
OS定时器的回调函数，可传参，可以不用 *ststic* 与 *inline* 修饰只是为了增加一点点执行速度。定时器计时到了就会执行该函数。
这是当前项目中负责PWM马达震动的定时器，兼职负责上电USB连接后指示灯闪烁。

#### static inline void *SpecialFunc_timer_task(void *p_arg)
OS定时器的回调函数，可传参，可以不用 *ststic* 与 *inline* 修饰只是为了增加一点点执行速度。定时器计时到了就会执行该函数。
目前用于重启USB栈切换描述符重新枚举的作用。

#### void my_PWM_output_init(void)
使用杰理MCU的MCPWM功能，跟STM32一样需要对硬件支持PWM输出的IO要初始化 时基、频率、占空比、输出模式。
更多具体需要初始化的成员请看代码。

#### void my_button_init(void)
按键所使用的普通IO与摇杆所使用的ADC IO的初始化，关于杰里MCU调用IO的封装函数，请看代码。

#### static inline unsigned char merge_value(unsigned char all_key, unsigned char key, unsigned int bit)
将键值key（键值只有0或1两种状态）填入all_key，bit表示要填如all_key的第几位，0~7bit。

---
## Xbox360 support only PC
#### void my_read_key(void)
读取普通IO输入的函数。
附带了连发功能的实现。

#### void read_trigger_value(void)
读取左扳机与右扳机的ADC值，设置两端的死区，取真实轴中间的值重新做轴映射。
![image](https://github.com/PING020903/AC695N-demo-xbox360-WindowsPC_only/assets/88314322/75350701-c57c-4d2f-a529-f528311a75d4)

#### void left_read_rocker(void)
读取左摇杆的ADC值，与读取扳机的ADC值类似，也要取轴中间值重新做轴映射，与之不同的是摇杆的ADC值初始值就在中间，需要给中间做死区。
实际有效读取区域如下图蓝色区域，中间黑色是非读取区域，极限边缘就是该圆边：
![image](https://github.com/PING020903/AC695N-demo-xbox360-WindowsPC_only/assets/88314322/ee93a0f9-1006-40c8-bdd1-fe1dd8d16458)

#### void right_read_rocker(void)
读取右摇杆的ADC值，实现原理与读取左摇杆相同。

#### static inline unsigned int USB_TX_data(const usb_dev usb_id, const u8 *buffer, unsigned int len)
调用本demo project中的封装的USB发送函数，usb_id 是USB设备的id号，buffer 要发送的字符串，len 要发送的字符串的长度。

#### static inline void send_data_to_host(void)
检查键值、摇杆、扳机是否有变化，判断主机是否允许从机输入（SOF PND），发送描述从机键值变化的字符串。
` static unsigned char data_send_to_host_temp[20] = {0x00}; `

#### void my_led_function(void)
控制PWM输出控制马达转速。
当设备的USB配置的OUT端点接收到震动指令的时候就会执行指令，控制当前PWM输出的占空比。

#### void connect_flicker(void)
上电后USB连接成功，处理主机发送给设备的数据，使player指示灯闪烁

#### void records_movement(void)
按键记录宏功能。键值有变化时记录键值，无变化时记录时间。使用数组存储键值与时间。

```c
#define ONCE_MAX_RECORD_TIMES   (0xfffd)
static unsigned char records_movement_key;       // 记录键被按下, 开始记录
static unsigned char records_flag = 0;           // records ready
static unsigned short RecordFunc_key_times = 0;  // 记录该功能按键按下时间
static volatile unsigned short record_times = 0; // 宏记录时长, Max time is 0xff
#if MY_ARRAY
#define CHAR_SIZE_NEXT (4)
#define SHORT_SIZE_NEXT (2)
#define MAX_RECORD_ARRAY_LEN (128)               // "记录"最大长度
static int record_player_led;                   // player LED IO
static unsigned short PWM_temp;                 // 减少进入PWM占空比设置函数
static unsigned char time_flag = 1;             // 每次记录前都要记得将时间清零
static unsigned char ban_flag = 0;              // 不能直接记录(case 1)后跳转到播放记录(case 5)
static unsigned char reappear_record = 0;       // 复现按键当前状态记录
static unsigned char records_length = 0;        // 记录长度( 参与计算 )
static unsigned char records_length_temp = 0;   // 临时记录长度( 不参与计算 )
static unsigned short reappear_times_temp[MAX_RECORD_ARRAY_LEN] = {0x00}; // 临时时间记录
static unsigned int records_keys[MAX_RECORD_ARRAY_LEN] = {0x00}; // 记录键值与时间
static unsigned int *records_keys_point = &records_keys;     // 第一次:  *((unsigned char*)records_keys_point + (4 * 0) ) = data_send_to_host[2],
                                                             //          *((unsigned char*)records_keys_point + 1 + (4 * 0) ) = data_send_to_host[3],
                                                             //          *((unsigned short*)records_keys_point + 1 + (2 * 0) ) = time;  //  记录持续时间, record duration
                                                             //
                                                             // 下一次:  *((unsigned char*)records_keys_point + (4 * 1) ) = data_send_to_host[2],
                                                             //          *((unsigned char*)records_keys_point + 1 + (4 * 1) ) = data_send_to_host[3],
                                                             //          *((unsigned short*)records_keys_point + 1 + (2 * 1) ) = time;
                                                             //
                                                             // 下下次:  *((unsigned char*)records_keys_point + (4 * 2)) = data_send_to_host[2],
                                                             //          *((unsigned char*)records_keys_point + 1 + (4 * 2)) = data_send_to_host[3],
                                                             //          *((unsigned short*)records_keys_point + 1 + (2 * 2)) = time;
/* a 'int' type of size */
/* 0000 0000 0000 0000 0000 0000 0000 0000 */
/* _________ _________ ___________________ */
/* Key value Key value    Times record     */
#endif
```
---
## support PS3, no support for 3-axis sensors
#### void ps3_read_key(void)
读取普通IO输入的函数。读取到按键输入时往数据中代表该按键AD值的位置填入0xff，避免在游戏"god of war"中按键按下界面无响应的问题。

#### void ps3_read_trigger(void)
读取扳机输入，参考PC_Xbox360部分的扳机输入读取函数。

#### void ps3_left_read_rocker(void)
读取左摇杆XY轴的AD值，PS3对应该值的处理不能像Xbox360手柄那样，否则在游戏中会有手柄在物理上达到边界，但在游戏中的表现并非达到边界的表现，具体见该项目首页的readme.md，楼主详细阐述了缘由

![image](https://github.com/PING020903/AC695N-demo-GamePad/assets/88314322/ccf2a8eb-94f8-41a6-9ed9-02a1b75613d5)

#### void ps3_right_read_rocker(void)
读取右摇杆的XY轴的AD值

#### void ps3_player_led(void)
该函数用以显示主机给手柄分配的编号。
关于主机分配的编号解析，在hid.c中处理，通过全局变量传递给该函数。

#### void ps3_PWM_shake(void)
响应主机给手柄发送的震动强度。
关于主机给手柄发送的震动强度，在hid.c中接收。

---
## USB连接部分

我在原本的 *uac1.c* 中添加创建了自己的usb接口处理函数以及 配置描述符、接口描述符、端点描述符，在 *descriptor.c* 中修改了设备描述符。

##### 设备描述符

```c
static const u8 sDeviceDescriptor[] = { //<Device Descriptor
    USB_DT_DEVICE_SIZE,         // bLength: Size of descriptor  0x12
    USB_DT_DEVICE,              // bDescriptorType: Device      0x01
#if defined(FUSB_MODE) && FUSB_MODE
    0x00, 0x02,     // bcdUSB: USB 2.0
#elif defined(FUSB_MODE) && (FUSB_MODE ==0 )
    0x00, 0x02,     // bcdUSB: USB 2.0
#else
#error "USB_SPEED_MODE not defined"
#endif
    0xff,           // bDeviceClass: none(填00意味着从接口描述符中获取这个'类信息')
    0xff,           // bDeviceSubClass: none
    0xff,           // bDeviceProtocol: none
    EP0_SETUP_LEN,//EP0_LEN,      // bMaxPacketSize0: 8/64 bytes    0x40
    0x5e, 0x04,     // idVendor: 0x4a4c - JL
    0x8e, 0x02,     // idProduct: chip id
    0x10, 0x01,     // bcdDevice: version 1.1
    0x01,           // iManufacturer: Index to string descriptor that contains the string <Your Name> in Unicode
    0x02,           // iProduct: Index to string descriptor that contains the string <Your Product Name> in Unicode
    0x03,           // iSerialNumber: none
    0x01            // bNumConfigurations: 1
};
```



##### 配置描述符

```c
static const u8 sConfigDescriptor[0x09] =     // < Config Descriptor >
{
    // bNumInterfaces: 在set_descriptor函数里面计算
    // 配置描述符的总长度也是在链接库中计算的
    0x09, 0x02, 0x90, 0x00, 0x04, 0x01, 0x00, 0xa0, 0xfa
};
```



##### 接口描述符以及端点描述符

```c
static const u8 sConfigDescriptor_interface_0[0x28] =     // < USER Interface Descriptor >
{
    //0x09, 0x02, 0x99, 0x00, 0x04, 0x01, 0x00, 0xa0, 0xfa,
    0x09, 0x04, 0x00, 0x00, 0x02, 0xff, 0x5d, 0x01, 0x00,
    0x11, 0x21, 0x10, 0x01, 0x01, 0x25, 0x81, 0x14, 0x03, 0x03, 0x03, 0x04, 0x13, 0x02, 0x08, 0x03, 0x03,
    0x07, 0x05, 0x81, 0x03, 0x20, 0x00, 0x01,
    0x07, 0x05, 0x02, 0x03, 0x20, 0x00, 0x08,
};
static const u8 sConfigDescriptor_interface_1[0x40] =
{
    0x09, 0x04, 0x01, 0x00, 0x04, 0xff, 0x5d, 0x03, 0x00,
    0x1b, 0x21, 0x00, 0x01, 0x01, 0x01, 0x83, 0x40, 0x01, 0x04, 0x20, 0x16, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x05, 0x83, 0x03, 0x20, 0x00, 0x02,
    0x07, 0x05, 0x04, 0x03, 0x20, 0x00, 0x04,
    0x07, 0x05, 0x85, 0x03, 0x20, 0x00, 0x40,
    0x07, 0x05, 0x03, 0x03, 0x20, 0x00, 0x10,
};
static const u8 sConfigDescriptor_interface_2[0x19] =
{
    0x09, 0x04, 0x02, 0x00, 0x01, 0xff, 0x5d, 0x02, 0x00,
    0x09, 0x21, 0x00, 0x01, 0x01, 0x22, 0x86, 0x07, 0x00,
    0x07, 0x05, 0x86, 0x03, 0x20, 0x00, 0x10,
};
static const u8 sConfigDescriptor_interface_3[0x0f] =
{
    0x09, 0x04, 0x03, 0x00, 0x00, 0xff, 0xfd, 0x13, 0x04,
    0x06, 0x41, 0x00, 0x01, 0x01, 0x03
};
```

###### 无须开发者将接口描述符与配置描述符手动拼接，链接库的库函数会帮你解决。

将重心放在如何接收数据如何处理即可。

在 *uac1.c* 中我创建了几个接口配置函数：（这几个接口配置函数兼承担了计算接口描述符长度的职责，用于返回给库函数参数）

```c
u32 uac_my_pc360_0(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);
u32 uac_my_pc360_1(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);
u32 uac_my_pc360_2(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);
u32 uac_my_pc360_3(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);
```

这几个接口配置了同一个接口处理函数:

```c
static u32 pc360_interface_hander(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup);
```

实际上，该接口处理函数主要是处理处于 *endpoint_0* 与 *set_interface* 阶段的答复，当 *set_interface* 答复完毕的时候，就直接与设备硬件上的 *endpoint* 进行通讯了。但这个步骤也是在接口配置函数中处理的，有一个复位处理函数。

```c
static void pc360_reset_hander(struct usb_device_t *usb_device, u32 itf)
{
    log_debug("---------- %s ----------", __func__);
    const usb_dev usb_id = usb_device2id(usb_device);
    xbox360_endpoint_init(usb_device, itf);
}
```

主机在配置的时候会下令从机设备的USB进行复位，当复位的时候就会完成 *endpoint* 的初始化， *endpoint* 初始化函数如下

```c
static void xbox360_endpoint_init(struct usb_device_t *usb_device, u32 itf)
{
    printf("---------- %s ----------\n", __func__);
    const usb_dev usb_id = usb_device2id(usb_device);
    memset(xbox360_ep_in_dma, 0, MAXP_SIZE_HIDIN);
    usb_g_ep_config(usb_id, (0x01 | 0x80), USB_ENDPOINT_XFER_INT, 0, xbox360_ep_in_dma, MAXP_SIZE_HIDIN);
    usb_enable_ep(usb_id, 1);

    memset(xbox360_ep_out_dma, 0, MAXP_SIZE_HIDOUT);
    usb_g_ep_config(usb_id, (HID_EP_OUT), USB_ENDPOINT_XFER_INT, 1, xbox360_ep_out_dma, MAXP_SIZE_HIDOUT);
    usb_g_set_intr_hander(usb_id, (HID_EP_OUT), hid_rx_data);
    usb_enable_ep(usb_id, 2);

/* 该端点主机检查后会在连接下发OUT包, 00 03 00
    没有该端点就不会下发 */
    memset(xbox360_ep3_dma, 0, MAXP_SIZE_HIDOUT);
    usb_g_ep_config(usb_id, 3U, USB_ENDPOINT_XFER_INT, 1, xbox360_ep3_dma, MAXP_SIZE_HIDOUT);
    usb_g_set_intr_hander(usb_id, 3U, test_rx_data);
    usb_enable_ep(usb_id, 3U);
}
```

可见函数中启用了端点，每个端点都配置了一个缓冲区，且传入了端点收到消息的回调函数的地址。

###### 端点处理回调函数

```c

typedef void (*usb_interrupt)(struct usb_device_t *, u32 ep)

static usb_interrupt hid_rx_data(struct usb_device_t *usb_device, u32 ep)
{
    //log_debug("---------- %s ----------", __func__);
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 rx_len = usb_g_intr_read(usb_id, ep, NULL, 64, 0);

    if( (xbox360_ep_out_dma[0] == 0x01) &&
        (xbox360_ep_out_dma[1] == 0x03) &&
        (xbox360_ep_out_dma[2] >= 0x02 || xbox360_ep_out_dma[2] <= 0x05) &&
        config_flag == 0){

        if(xbox360_ep_out_dma[2] == 0x02){      /* player1 */
            gpio_direction_output(IO_PORTA_03, 1 );
            gpio_direction_output(IO_PORTA_02, 0 );
            gpio_direction_output(IO_PORTA_01, 0 );
            gpio_direction_output(IO_PORTA_04, 0 );
            player_led = 1;
        }
        if(xbox360_ep_out_dma[2] == 0x03){      /* player2 */
            gpio_direction_output(IO_PORTA_03, 0 );
            gpio_direction_output(IO_PORTA_02, 1 );
            gpio_direction_output(IO_PORTA_01, 0 );
            gpio_direction_output(IO_PORTA_04, 0 );
            player_led = 2;
        }
        if(xbox360_ep_out_dma[2] == 0x04){      /* player3 */
            gpio_direction_output(IO_PORTA_03, 0 );
            gpio_direction_output(IO_PORTA_02, 0 );
            gpio_direction_output(IO_PORTA_01, 1 );
            gpio_direction_output(IO_PORTA_04, 0 );
            player_led = 3;
        }
        if(xbox360_ep_out_dma[2] == 0x05){      /* player4 */
            gpio_direction_output(IO_PORTA_03, 0 );
            gpio_direction_output(IO_PORTA_02, 0 );
            gpio_direction_output(IO_PORTA_01, 0 );
            gpio_direction_output(IO_PORTA_04, 1 );
            player_led = 4;
        }
        config_flag++;
    }
    if( (xbox360_ep_out_dma[0] == 0x01) &&
        (xbox360_ep_out_dma[1] == 0x03) &&
        (xbox360_ep_out_dma[2] >= 0x02 || xbox360_ep_out_dma[2] <= 0x05) &&
        config_flag != 0){
        hid_tx_data(usb_device, xbox360_ep_out_dma, rx_len);
    }

    if( (xbox360_ep_out_dma[0] == 0x02) &&
        (xbox360_ep_out_dma[1] == 0x08) &&
        (xbox360_ep_out_dma[2] == 0x03)){
        usb_g_intr_write(usb_id, 0x01, fraud_temp, 0x14);
    }
    if( (xbox360_ep_out_dma[0] == 0x00) &&
        (xbox360_ep_out_dma[1] == 0x03) &&
        (xbox360_ep_out_dma[2] == 0x00)){
        player_flicker_time = 1;
        u8 temp[0x03] = {0x05, 0x03, 0x00};
        usb_g_intr_write(usb_id, 0x01, temp, 0x03);
    }
    if( (xbox360_ep_out_dma[0] == 0x00) &&
        (xbox360_ep_out_dma[1] == 0x08) &&
        (xbox360_ep_out_dma[2] == 0x00) ){
        trigger[0] = xbox360_ep_out_dma[3];
        trigger[1] = xbox360_ep_out_dma[4];
    }
}
```

当然，缓冲区也是要开辟区块的，上面只是传入了缓冲区的地址。

```c
u32 xbox360_register(const usb_dev usb_id)
{
    printf("---------- %s ----------\n", __func__);
    /* 根据接口描述符, 设置接口ID 0x01   bit7: IN/OUT 对应 1/0 */
    xbox360_ep_in_dma = usb_alloc_ep_dmabuffer(usb_id, (0x01 | 0x80), MAXP_SIZE_HIDIN);
    if(xbox360_ep_in_dma == NULL)
        log_error(" 'xbox360_ep_in_dma' is NULL");

    xbox360_ep_out_dma = usb_alloc_ep_dmabuffer(usb_id, (HID_EP_OUT), MAXP_SIZE_HIDOUT);
    if(xbox360_ep_out_dma == NULL)
        log_error(" 'xbox360_ep_out_dma' is NULL");

    xbox360_ep3_dma = usb_alloc_ep_dmabuffer(usb_id, 3, MAXP_SIZE_HIDOUT);
    if(xbox360_ep3_dma == NULL)
        log_error(" 'xbox360_ep3_dma' is NULL");
    return 0;
}
```

该函数 `u32 xbox360_register(const usb_dev usb_id)`在 *usb_device.c* 中执行，执行后进行描述符配置，这个描述符配置就是以上所描述的 **USB连接部分** 所做的事情。
### 设备描述符的切换
杰理的这个USB栈，由于看不见具体实现，但是我反复试验，发现重启延时较长，而且只能在枚举成功后重启USB栈才不会导致整个MCU重启，如果我在 "主机获取设备描述符" 阶段重启USB栈，会出现USB栈重启后再次进入 "主机获取设备描述符" 该阶段的第二次上报设备描述符时整个MCU都重启，如此往复。。。楼主也是想了很久试了很久
故此不同设备的描述符切换的实现，是靠枚举成功后主从机信息交互的特征来判断的，特征正确将不再进行描述符的切换。
使用了全局变量来表示当前描述符是哪个设备的，定义在`\sdk\apps\common\usb\device\task_pc.c` `u8 usb_connect_timeout_flag;`
修改于`\sdk\apps\dongle\my_put_test.c`的 SPECIAL_FUNCTIONS 任务，其中係通过主机确认手柄是PS3手柄后发送的一个开启手柄的命令来判断当前描述符是否正确，该变量源于`\sdk\apps\common\usb\device\hid.c` `unsigned char PS3_host_flag;`


### 感谢阅读本文档，写得可能比较简陋，第一次写代码文档，若有遗漏的地方，请多包涵。


`

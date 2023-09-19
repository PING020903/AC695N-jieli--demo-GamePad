
#include "my_put_test.h"

#include "system/debug.h"
#include "system/includes.h"
#include "system/timer.h"

#include "asm/includes.h"
#include "asm/power/p33.h"
#include "asm/pwm_led.h"
#include "asm/gpio.h"
#include "asm/pwm_led.h"
#include "asm/adc_api.h"

#include "os/os_api.h"
#include "os/os_error.h"
#include "typedef.h"
#include "printf.h"
#include "event.h"
#include "syscfg_id.h"

#include "generic/gpio.h"
#include "generic/log.h"
#include "asm/mcpwm.h"

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
static u8 usb_timeout = 0;
#if FUNC_TIMESTAMP

/*  以一种很粗糙的方式去判断函数的执行时间
    建议一个一个函数启动该打印, 显示出时间戳, 否则可能会导致队列溢出
func_name:      | my_read_key   | read_trigger_value| left_read_rocker  | right_read_rocker | my_led_function
start_value:    |       2       |           4       |       6           |           8       |       0
end_value:      |       3       |           5       |       7           |           9       |       1
*/
#define RESET_VAL   (1)     // 该宏执行一次打印, 分别在刚进入函数与函数结束, 串口打印一次约耗时5ms, 谨慎使用( 容易导致任务队列溢出u )
#define START_FUNC  (0)     // 起始函数值
#define END_FUNC    (0)     // 结束函数值

static unsigned char count_all_func[10]; // function start to end time
#endif

#define MY_TASK_NAME        "thursday"
#define MAIN_TCC_TIMER      (4)
#define LED_TCC_TIMER       (8)
#define SPECIAL_FUNC_TCC    (450)

#define DEADBAND_X1         (490)       // internal deadband, minus side of the axis
#define DEADBAND_X2         (575)       // internal deadband, plus side of the axis
#define PS3_DEADBAND_X1     (460)
#define PS3_DEADBAND_X2     (600)
#define PS3_OUTSIDE_DEADBAND_X1 (50)
#define PS3_OUTSIDE_DEADBAND_X2 (1000)
#define TRIGGER_INIT_VAL    (30)        // trigger initial value
#define TRIGGER_PRESS_VAL   (45)        // more than the value, so press the trigger

extern usb_dev usbfd;            // from task_pc.c
extern unsigned char trigger[2]; // from uac1.c
extern unsigned char player_led; // from uac1.c

extern void my_pwm_led_on_display(u8 led_index, u16 led0_bright, u16 led1_bright);
extern void mcpwm_init(struct pwm_platform_data *arg);
extern void mcpwm_set_duty(pwm_ch_num_type pwm_ch, pwm_timer_num_type timer_ch, u16 duty);
extern void log_pwm_info(pwm_ch_num_type pwm_ch, pwm_timer_num_type timer_ch);

volatile unsigned int tcc_count = 0;
volatile unsigned int tcc_conut_led = 0;
volatile unsigned int tcc_connt_SpecialFunc = 0;

static int ret_id_timer; // timer ID
static int ret_id_timer_led;
static int ret_id_timer_SpecialFunctions;
unsigned char timer_send_flag = 0;

static unsigned char data_send_to_host[20] = { 0x00 };  /* using the variant have to set zero , in the after assign the value */
static unsigned char data_send_to_host_temp[20] = { 0x00 };
static unsigned char ps3_data_send_to_host[49] = { 0x00 };
static unsigned char ps3_data_send_to_host_temp[49] = { 0x00 };

#if READ_KEY
static volatile unsigned char my_key_val_1 = 0; // IO_PORTA_08
static volatile unsigned char my_key_val_2 = 0; // IO_PORTA_11
static volatile unsigned char my_key_val_3 = 0; // IO_PORTA_12
static volatile unsigned char my_key_val_4 = 0; // IO_PORTA_14

static volatile unsigned char a_key_val = 0; // IO_PORTB_11
static volatile unsigned char b_key_val = 0; // IO_PORTB_06
static volatile unsigned char x_key_val = 0; // IO_PORTB_09
static volatile unsigned char y_key_val = 0; // IO_PORTB_04

static volatile unsigned char R_rocker_io_key = 0; // IO_PORTB_02
static volatile unsigned char L_rocker_io_key = 0; // IO_PORTA_07

static volatile unsigned char L_1_io_key = 0; // IO_PORTB_05
static volatile unsigned char R_1_io_key = 0; // IO_PORTB_07

static volatile unsigned char start_io_key = 0;   // IO_PORTC_00
static volatile unsigned char back_io_key = 0;    // IO_PORTC_07
static volatile unsigned char XboxBar_io_key = 0; // IO_PORTC_01

static volatile unsigned char ps_key = 0;
#endif

#if LEFT_ROCKER
static volatile int my_rocker_ad_key_x = 0; // ADC1, IO_PORTA_05
static volatile int my_rocker_ad_key_y = 0; // ADC2, IO_PORTA_06
#endif

#if RIGHT_ROCKER
static volatile int R_rocker_ad_key_x = 0; // ADC5, IO_PORTB_01
static volatile int R_rocker_ad_key_y = 0; // ADC6, IO_PORTB_03
#endif

#if TRIGGER
#if LEFT_TRIGGER
static volatile int L_trigger_ad_key = 0; // ADC8, IO_PORTB_08
static volatile int L_trigger_temp = 0;
#endif /* left trigger */

#if RIGHT_TRIGGER
static volatile int R_trigger_ad_key = 0; // ADC9, IO_PORTB_10
static int R_trigger_temp = 0;
#endif /* right trigger */
#endif /* trigger */

#if PWM_MOTOR
#if LEFT_MOTOR
static int trigger_value_L = 0;
static int L_shake_time = 0;
#endif /* left motor */

#if RIGHT_MOTOR
static int trigger_value_R = 0;
static volatile int R_shake_time = 0;
#endif /* right motor */
#endif /* PWM motor */

#if SUCCESSIVE_PRESS
#define SUCCESSIVE_KEYS_NUMBER_ENABLE       (14)
#define SUCCESSIVE_OF_CYCLICALITY(a)        (250U / (unsigned int)a)
#define SUCCESSIVE_OF_HALF_CYCLICALITY(a)   (SUCCESSIVE_OF_CYCLICALITY(a) / 2) // 1000ms / x(Hz) = CYCLICALITY_TIMES, 该半周期值是近似值
static unsigned char successive_press_status = 0;
static unsigned int frequency = 8; // Hz
static unsigned char general_keys[12] = {0x00};     // 记录普通按键是否曾按下
static unsigned char successive_trigger[2] = {0x00};// 记录扳机是否曾按下
static unsigned char successive_IO_LOW_status_times = 0;
static unsigned char successive_press_keys[SUCCESSIVE_KEYS_NUMBER_ENABLE] = {0x00}; // 使能连点按键, 表示按键是否被使能连点, 0普通模式 | 1连发模式
#endif

#if RECORD_MOVEMENT
#define ONCE_MAX_RECORD_TIMES   (0xfffd)
static unsigned char key_start_press = 0;           // 有可记录的按键按下后开始记录
static unsigned char successive_temp[SUCCESSIVE_KEYS_NUMBER_ENABLE];    // 连点状态保存, 进入按键编程禁用连点, 连点容易导致记录的数组资源枯竭
static unsigned char records_movement_key;          // 记录键被按下, 进入开始记录状态
static unsigned char records_flag = 0;              // "record function" status
//static unsigned short RecordFunc_key_times = 0;  // 记录该功能按键按下时间, 单个按键完成功能切换时启用
//static volatile unsigned short record_times = 0; // 宏记录时长, Max time is 0xff
#if MY_ARRAY
#define CHAR_SIZE_NEXT          (4)
#define SHORT_SIZE_NEXT         (2)
#define LEN_RECORD_VM           (1)             // 用于保存记录长度
#define MAX_RECORD_ARRAY_LEN    (512)           // "记录"最大长度
#define TIMES_LIMIT             (30)            // 最长时间, 单位: 秒(s)
static int record_player_led;                   // player LED IO
static unsigned short PWM_temp;                 // 减少进入PWM占空比设置函数
static unsigned char time_flag = 1;             // 每次记录前都要记得将时间清零
static unsigned char ban_flag = 0;              // 不能直接记录(case 1)后跳转到播放记录(case 5)
static unsigned char reappear_record = 0;       // 复现按键当前状态记录
static unsigned char records_length = 0;        // 记录长度( 参与计算 )
static unsigned char records_length_temp = 0;   // 临时记录长度( 不参与计算 )
static unsigned short effective_record_ms = 0;  // 实际有效记录时长
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
#if MY_LIST
struct keys_info_node
{
    struct keys_info_node *next; // 2 points, 16-byte
    struct keys_info_node *prev;
    unsigned int record_times;  // 4-byte
    unsigned char keys_data[2]; // 2 char, 2-byte
};
struct keys_info_node key_info_head;
#endif
#if VM_TEST
//#define USER_VM_ID_MAX  (49)

#define NUM_FOR_VM_LEN  MAX_RECORD_ARRAY_LEN
static unsigned char VM_userID = 1; // VM user space, 1~49
static unsigned char VM_recordNUM = 0;
//static u32 record_buf[NUM_FOR_VM_LEN]  __attribute__((aligned(4)));   // GUN C support only, GUN C独有的, 尽可能按照4字节对齐
static void* buf_point;   // write and read
#endif
#endif



static volatile unsigned char io_key_status;    // merged io key
static unsigned char motor_flag = 1;            // 清除初始化的占空比值 Clear the initialised duty cycle value
static unsigned char player_IO_status = 1;      // 玩家led指示灯状态
static unsigned char exit_flicker = 0;
volatile unsigned char player_flicker_time = 0; // 玩家led指示灯闪烁时间

#if MY_LIST
static inline my_node_init(struct keys_info_node this_node)
{
    this_node.next = &this_node;
    this_node.prev = &this_node;
}
#endif

#if 0
volatile struct sys_event my_key_event;             //will send the key_event
void my_send_IOkey_event(void)
{
    my_key_event.type           = 0x0001;             //key_event 0x0001,
    my_key_event.u.key.event    = 1;
    my_key_event.u.key.type     = 3;
    my_key_event.u.key.value    = (unsigned int)all_io_key;
    sys_event_notify(&my_key_event);
}
#endif // disable send system event
static unsigned char motor_data[8];
static inline unsigned int USB_TX_data(const usb_dev usb_id, const u8 *buffer, unsigned int len)
{
    int ret = usb_g_intr_write(usb_id, 0x01, buffer, len); /* 0x01 is IN endpoint */
    return ret;
}

/* merge io key , 'key' value is one or zero only*/
static inline unsigned char merge_value(unsigned char all_key, unsigned char key, unsigned int bit)
{
    if (key != 1 && key != 0)
        return all_key;
    if (key == 1)
        return all_key |= (1 << bit);
    if (key == 0)
        return all_key &= ~(1 << bit);
}

static inline void send_data_to_host(void)
{
#if MOVEMENTS_SEND
    unsigned char send_flag = 0;
#endif
    unsigned int register_temp = JL_USB->CON0;
    register_temp = register_temp << 18;
    register_temp = register_temp >> 31; // 13th bit, SOF_PND
#if MOVEMENTS_SEND
    for (int i = 0; i < 20; i++)
    {
        if (data_send_to_host_temp[i] == data_send_to_host[i])
            send_flag++;
        if (data_send_to_host_temp[i] != data_send_to_host[i])
            send_flag--;
        if (send_flag == 20)
            return;
    }
    if (send_flag < 20)
    {
#endif
        if (register_temp)
            USB_TX_data(usbfd, data_send_to_host, sizeof(data_send_to_host));

#if MOVEMENTS_SEND
        for (int i = 0; i < 20; i++)
            data_send_to_host_temp[i] = data_send_to_host[i];
    }
#endif
}
static inline void ps3_send_to_host(void)
{
    extern unsigned char PS3_sendDATA_flag; // from hid.c
    unsigned int register_temp = JL_USB->CON0;
    register_temp = register_temp << 18;
    register_temp = register_temp >> 31; // 13th bit, SOF_PND
    if (register_temp && PS3_sendDATA_flag)
        USB_TX_data(usbfd, ps3_data_send_to_host, sizeof(ps3_data_send_to_host));
    else
    {
        if (register_temp)/* 爲了獲取開啓手柄的信號, 先發一次手柄信息 */
            USB_TX_data(usbfd, ps3_data_send_to_host, sizeof(ps3_data_send_to_host));
    }
#if PS3_KEY_PRINT
    //extern unsigned char ps3_read_ep[64];
    //if (motor_data[1] != ps3_read_ep[4])  // 會導致卡住卡死
    //{
    //    ps3_data_send_to_host[9];
    //    timer_send_flag = 1;
    //    //mem_stats();    // 实时查看RAM
    //    printf("[2]-> %x, [3]-> %x, [4]-> %x, %x %x  %x %x", ps3_data_send_to_host[2], ps3_data_send_to_host[3], ps3_data_send_to_host[4],
    //        ps3_read_ep[1], ps3_read_ep[2], ps3_read_ep[3], ps3_read_ep[4]);
    //    ps3_read_ep[2] = motor_data[0];
    //    ps3_read_ep[4] = motor_data[1];
    //}
    //unsigned char send_flag = 0;
    //unsigned int register_temp = JL_USB->CON0;
    //register_temp = ((register_temp << 18) >> 31); // 13th bit, SOF_PND
    //for (int i = 0; i < 49; i++)
    //{
    //    if (ps3_data_send_to_host_temp[i] == ps3_data_send_to_host[i])
    //        send_flag++;
    //    if (ps3_data_send_to_host_temp[i] != ps3_data_send_to_host[i])
    //        send_flag--;
    //    if (send_flag == 49)
    //        return;
    //}
    //if (send_flag < 49)
    //{
    //    if (register_temp)
    //        USB_TX_data(usbfd, ps3_data_send_to_host, sizeof(ps3_data_send_to_host));
    //}
#endif // PS3_KEY_PRINT
}

/***************************************** xbox360(PC) function *****************************************/
void my_read_key(void)
{
#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[2])
    {
        count_all_func[2] = 0;
        printf("------ %s -- start\n", __func__);
    }
    timer_send_flag = 0;
#endif

#if READ_KEY
    L_rocker_io_key = (gpio_read(IO_PORTA_07)) ^ 0x01;
    R_rocker_io_key = (gpio_read(IO_PORTB_02)) ^ 0x01;

    L_1_io_key = (gpio_read(IO_PORTB_05)) ^ 0x01;
    R_1_io_key = (gpio_read(IO_PORTB_07)) ^ 0x01;

    my_key_val_1 = (gpio_read(IO_PORTA_08)) ^ 0x01; // ↑
    my_key_val_2 = (gpio_read(IO_PORTA_11)) ^ 0x01; // ↓
    my_key_val_3 = (gpio_read(IO_PORTA_12)) ^ 0x01; // ←
    my_key_val_4 = (gpio_read(IO_PORTA_14)) ^ 0x01; // →

    a_key_val = (gpio_read(IO_PORTB_11)) ^ 0x01;
    b_key_val = (gpio_read(IO_PORTB_06)) ^ 0x01;
    x_key_val = (gpio_read(IO_PORTB_09)) ^ 0x01;
    y_key_val = (gpio_read(IO_PORTB_04)) ^ 0x01;

    start_io_key = (gpio_read(IO_PORTC_00)) ^ 0x01;
    back_io_key = (gpio_read(IO_PORTC_07)) ^ 0x01;
    XboxBar_io_key = (gpio_read(IO_PORTC_01)) ^ 0x01;

    // JL_PORTA->IN |= BIT(0);  // 输入选择
#endif

#if RECORD_MOVEMENT
    if (records_movement_key)
    {
        if (L_rocker_io_key || R_rocker_io_key || L_1_io_key || R_1_io_key ||
            my_key_val_1 || my_key_val_2 || my_key_val_3 || my_key_val_4 ||
            a_key_val || b_key_val || x_key_val || y_key_val)    // 可记录按键按下
        {
            key_start_press = 1;
        }
    }

#endif


#if SUCCESSIVE_PRESS

    if ((gpio_read(IO_PORTC_03)) == 0)  // 功能按键按下
    {
        if(my_key_val_1)
        {
            if (!general_keys[0])
            {
                successive_press_keys[0] ^= 0X01;
                general_keys[0] = 1;
            }
        }
        else
        {
            general_keys[0] = 0;
        }

        if (my_key_val_2)
        {
            if (!general_keys[1])
            {
                successive_press_keys[1] ^= 0X01;
                general_keys[1] = 1;
            }
        }
        else
        {
            general_keys[1] = 0;
        }

        if (my_key_val_3)
        {
            if (!general_keys[2])
            {
                successive_press_keys[2] ^= 0X01;
                general_keys[2] = 1;
            }
        }
        else
        {
            general_keys[2] = 0;
        }

        if (my_key_val_4)
        {
            if (!general_keys[3])
            {
                successive_press_keys[3] ^= 0X01;
                general_keys[3] = 1;
            }
        }
        else
        {
            general_keys[3] = 0;
        }

        if (a_key_val)
        {
            if (!general_keys[4])
            {
                successive_press_keys[4] ^= 0X01;
                general_keys[4] = 1;
            }
        }
        else
        {
            general_keys[4] = 0;
        }

        if (b_key_val)
        {
            if (!general_keys[5])
            {
                successive_press_keys[5] ^= 0X01;
                general_keys[5] = 1;
            }
        }
        else
        {
            general_keys[5] = 0;
        }

        if(x_key_val)
        {
            if (!general_keys[6])
            {
                successive_press_keys[6] ^= 0X01;
                general_keys[6] = 1;
            }
        }
        else
        {
            general_keys[6] = 0;
        }

        if (y_key_val)
        {
            if (!general_keys[7])
            {
                successive_press_keys[7] ^= 0X01;
                general_keys[7] = 1;
            }
        }
        else
        {
            general_keys[7] = 0;
        }

        if (L_1_io_key)
        {
            if (!general_keys[8])
            {
                successive_press_keys[8] ^= 0X01;
                general_keys[8] = 1;
            }
        }
        else
        {
            general_keys[8] = 0;
        }

        if (R_1_io_key)
        {
            if (!general_keys[9])
            {
                successive_press_keys[9] ^= 0X01;
                general_keys[9] = 1;
            }
        }
        else
        {
            general_keys[9] = 0;
        }

        if (L_rocker_io_key)
        {
           if (!general_keys[10])
            {
                successive_press_keys[10] ^= 0X01;
                general_keys[10] = 1;
            }
        }
        else
        {
            general_keys[10] = 0;
        }

        if (R_rocker_io_key)
        {
            if (!general_keys[11])
            {
                successive_press_keys[11] ^= 0X01;
                general_keys[11] = 1;
            }
        }
        else
        {
            general_keys[11] = 0;
        }

        /* printf("in the (if), 1>%d, 2>%d, 3>%d, 4>%d, 5>%d, 6>%d, 7>%d, 8>%d, 9>%d, 10>%d, 11>%d, 12>%d, ", successive_press_keys[0], successive_press_keys[1],
               successive_press_keys[2], successive_press_keys[3], successive_press_keys[4], successive_press_keys[5], successive_press_keys[6], successive_press_keys[7],
               successive_press_keys[8], successive_press_keys[9], successive_press_keys[10], successive_press_keys[11]); */
    }
    else
    {
        for(int i = 0; i < SUCCESSIVE_KEYS_NUMBER_ENABLE; i++)
            general_keys[i] = 0;
    }

    for (int i = 0; i < SUCCESSIVE_KEYS_NUMBER_ENABLE; i++)
    {
        successive_press_status = successive_press_keys[i];
        if (successive_press_status)
            break;
    }

    if (successive_press_status)    // 连发的按键输出其实类似于PWM波
    {
        if (my_key_val_1 && successive_press_keys[0] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))// 控制连发表示按键状态低时间
        {
            my_key_val_1 = 0x00;
        }
        if (my_key_val_2 && successive_press_keys[1] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            my_key_val_2 = 0x00;
        }
        if (my_key_val_3 && successive_press_keys[2] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            my_key_val_3 = 0x00;
        }
        if (my_key_val_4 && successive_press_keys[3] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            my_key_val_4 = 0x00;
        }

        if (a_key_val && successive_press_keys[4] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            a_key_val = 0x00;
        }
        if (b_key_val && successive_press_keys[5] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            b_key_val = 0x00;
        }
        if (x_key_val && successive_press_keys[6] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            x_key_val = 0x00;
        }
        if (y_key_val && successive_press_keys[7] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            y_key_val = 0x00;
        }

        if (L_1_io_key && successive_press_keys[8] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            L_1_io_key = 0x00;
        }
        if (R_1_io_key && successive_press_keys[9] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            R_1_io_key = 0x00;
        }

        if (L_rocker_io_key && successive_press_keys[10] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            L_rocker_io_key = 0x00;
        }
        if (R_rocker_io_key && successive_press_keys[11] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            R_rocker_io_key = 0x00;
        }

        if (SUCCESSIVE_OF_CYCLICALITY(frequency) <= successive_IO_LOW_status_times)
            successive_IO_LOW_status_times = 0;
        successive_IO_LOW_status_times++;
    }
#endif

#if MERGE_KEY                                                       /* IO key */
    io_key_status = 0x00;                                           /* using the variant have to set zero , in the after assign the value*/
    io_key_status = merge_value(io_key_status, my_key_val_3, 0);    // ↑up
    io_key_status = merge_value(io_key_status, my_key_val_2, 1);    // ↓dowm
    io_key_status = merge_value(io_key_status, my_key_val_1, 2);    // ←left
    io_key_status = merge_value(io_key_status, my_key_val_4, 3);    // →right
    io_key_status = merge_value(io_key_status, start_io_key, 4);    // start
    io_key_status = merge_value(io_key_status, back_io_key, 5);     // back
    io_key_status = merge_value(io_key_status, L_rocker_io_key, 6); //  left_3
    io_key_status = merge_value(io_key_status, R_rocker_io_key, 7); // right_3
    data_send_to_host[2] = io_key_status;

    io_key_status = 0x00;                                          /* using the variant have to set zero , in the after assign the value*/
    io_key_status = merge_value(io_key_status, L_1_io_key, 0);     //  left_1
    io_key_status = merge_value(io_key_status, R_1_io_key, 1);     // right_1
    io_key_status = merge_value(io_key_status, XboxBar_io_key, 2); // xbox home
    io_key_status = merge_value(io_key_status, a_key_val, 4);      // A
    io_key_status = merge_value(io_key_status, b_key_val, 5);      // B
    io_key_status = merge_value(io_key_status, x_key_val, 6);      // X
    io_key_status = merge_value(io_key_status, y_key_val, 7);      // Y
    data_send_to_host[3] = io_key_status;
#endif /* IO key */

#if MY_PRINTF
    if ((tcc_count % (1000 / MAIN_TCC_TIMER)) == 0)
        printf("---------- %s ----------", __func__);
#endif

#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[3])
    {
        count_all_func[3] = 0;
        printf("------ %s -- end\n", __func__);
    }
    timer_send_flag = 0;
#endif
}
void read_trigger_value(void)
{
#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[4])
    {
        count_all_func[4] = 0;
        printf("------ %s -- start\n", __func__);
    }
    timer_send_flag = 0;
#endif

#if TRIGGER /* trigger */
    L_trigger_ad_key = adc_get_value(8);
    R_trigger_ad_key = adc_get_value(9);

#if SUCCESSIVE_PRESS
    if ((gpio_read(IO_PORTC_03)) == 0)  // 功能按键按下
    {
        if(L_trigger_ad_key > TRIGGER_PRESS_VAL)
        {
            if (!successive_trigger[0])
            {
                successive_press_keys[12] ^= 0X01;
                successive_trigger[0] = 1;
            }
        }
        else
        {
            successive_trigger[0] = 0;
        }

        if(R_trigger_ad_key > TRIGGER_PRESS_VAL)
        {
            if (!successive_trigger[1])
            {
                successive_press_keys[13] ^= 0X01;
                successive_trigger[1] = 1;
            }
        }
        else
        {
            successive_trigger[1] = 0;
        }
    }
    else
    {
        for(int i = 0; i < 2; i++)
            successive_trigger[i] = 0;
    }

    for (int i = 0; i < SUCCESSIVE_KEYS_NUMBER_ENABLE; i++)
    {
       successive_press_status = successive_press_keys[i];
        if (successive_press_status)
            break;
    }

    if (successive_press_status)    // 连发的按键输出其实类似于PWM波
    {
        if ((L_trigger_ad_key > TRIGGER_PRESS_VAL) && successive_press_keys[12] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            L_trigger_ad_key = 0x00;
        }
        if ((R_trigger_ad_key > TRIGGER_PRESS_VAL) && successive_press_keys[13] && (successive_IO_LOW_status_times < SUCCESSIVE_OF_HALF_CYCLICALITY(frequency)))
        {
            R_trigger_ad_key = 0x00;
        }

        if (SUCCESSIVE_OF_CYCLICALITY(frequency) < successive_IO_LOW_status_times)
            successive_IO_LOW_status_times = 0;
        successive_IO_LOW_status_times++;
    }
#endif

#if LEFT_TRIGGER /* left */
    if (L_trigger_ad_key > TRIGGER_PRESS_VAL)
    {
        int send_condition = L_trigger_temp - (L_trigger_ad_key - TRIGGER_INIT_VAL); // jitter judgement
        if (send_condition > 4 || send_condition < (-4))
        {                                                                 // eliminate jitter
            data_send_to_host[4] = (L_trigger_ad_key - TRIGGER_INIT_VAL); // left trigger, L2
            L_trigger_temp = data_send_to_host[4];
        }

        if ((L_trigger_ad_key - TRIGGER_INIT_VAL) > 0xff)
        {
            data_send_to_host[4] = 0xff;
            L_trigger_temp = 0xff;
        }
    }
    else
    {
        data_send_to_host[4] = 0x00;
        L_trigger_temp = 0x00;
    }

#endif
#if RIGHT_TRIGGER /* right */
    if (R_trigger_ad_key > TRIGGER_PRESS_VAL)
    {
        int send_condition = R_trigger_temp - (R_trigger_ad_key - TRIGGER_INIT_VAL); // jitter judgement
        if (send_condition > 5 || send_condition < (-5))
        {                                                                 // eliminate jitter
            data_send_to_host[5] = (R_trigger_ad_key - TRIGGER_INIT_VAL); // right trigger, R2
            R_trigger_temp = data_send_to_host[5];
        }

        if ((R_trigger_ad_key - TRIGGER_INIT_VAL) > 0xff)
        {
            data_send_to_host[5] = 0xff;
            R_trigger_temp = 0xff;
        }
    }
    else
    {
        data_send_to_host[5] = 0x00;
        R_trigger_temp = 0x00;
    }

#endif
#endif /* trigger */

#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[5])
    {
        count_all_func[5] = 0;
        printf("------ %s -- end\n", __func__);
    }
    timer_send_flag = 0;
#endif
}
void left_read_rocker(void)
{
#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[6])
    {
        count_all_func[6] = 0;
        printf("------ %s -- start\n", __func__);
    }
    timer_send_flag = 0;
#endif

    /* Left */
    unsigned char L_X_plus = 0;  // X+
    unsigned char L_X_minus = 0; // X-
    unsigned char L_Y_plus = 0;  // Y+
    unsigned char L_Y_minus = 0; // Y-

#if LEFT_ROCKER

    /* parametric is adc CHANNEL */
    my_rocker_ad_key_x = adc_get_value(1); // ADC1
    my_rocker_ad_key_y = adc_get_value(2); // ADC2

    /* left rocker */
    if ((DEADBAND_X2 <= my_rocker_ad_key_x) || (DEADBAND_X1 >= my_rocker_ad_key_x))
    {
        if (DEADBAND_X2 <= my_rocker_ad_key_x) // X+
        {
            float temp_X_plus_f = (my_rocker_ad_key_x - DEADBAND_X2) / 3.48; // external deadband, right side of the axis
            L_X_plus = (unsigned char)temp_X_plus_f;
            L_X_minus = 0;
        }
        if (DEADBAND_X1 >= my_rocker_ad_key_x) // X-
        {
            float temp_X_minus_f = (DEADBAND_X1 - my_rocker_ad_key_x) / 3.7; // external deadband, left side of the axis
            L_X_minus = 0xff - (unsigned char)temp_X_minus_f;
            L_X_plus = 0;
        }
    }
    if ((DEADBAND_X2 <= my_rocker_ad_key_y) || (DEADBAND_X1 >= my_rocker_ad_key_y))
    {
        if (DEADBAND_X2 <= my_rocker_ad_key_y) // Y+
        {
            float temp_Y_plus_f = (my_rocker_ad_key_y - DEADBAND_X2) / 3.2; // external deadband, up side of the axis
            L_Y_plus = (unsigned char)temp_Y_plus_f;
            L_Y_minus = 0;
        }
        if (DEADBAND_X1 >= my_rocker_ad_key_y) // Y-
        {
            float temp_Y_minus_f = (DEADBAND_X1 - my_rocker_ad_key_y) / 3.8; // external deadband, down side of the axis
            L_Y_minus = 0xff - (unsigned char)temp_Y_minus_f;
            L_Y_plus = 0;
        }
    }
#endif /* left rocker */

#if LEFT_ROCKER_BUFFER /* left rocker value buffer */
#if LEFT_ROCKER_X_AXIS /* X axis */
    if (L_X_plus != 0 || L_X_minus != 0)
    {
        if (L_X_plus >= 127 && L_X_minus == 0) // X+
        {
            data_send_to_host[6] = 0xff; // left rocker
            data_send_to_host[7] = 0x7f;
        }
        if (L_X_plus < 127 && L_X_minus == 0)
        {
            data_send_to_host[6] = 0x00; // left rocker
            data_send_to_host[7] = L_X_plus;
        }

        if (L_X_plus == 0 && L_X_minus <= 0x80) // X-
        {
            data_send_to_host[6] = 0x00; // left rocker
            data_send_to_host[7] = 0x80;
        }
        if (L_X_plus == 0 && L_X_minus != 0 && L_X_minus > 0x80)
        {
            data_send_to_host[6] = 0x00; // left rocker
            data_send_to_host[7] = L_X_minus;
        }
    }
#endif

    if ((L_X_minus == 0 && L_X_plus == 0) && (L_Y_minus == 0 && L_Y_plus == 0)) // median value
    {
        data_send_to_host[6] = 0x00; // left rocker X
        data_send_to_host[7] = 0x00;
        data_send_to_host[8] = 0x00; // left rocker Y
        data_send_to_host[9] = 0Xff;
    }

#if LEFT_ROCKER_Y_AXIS /* Y axis */
    if (L_Y_plus != 0 || L_Y_minus != 0)
    {
        if (L_Y_plus >= 127 && L_Y_minus == 0)
        {
            data_send_to_host[8] = 0xff; // left rocker
            data_send_to_host[9] = 0x7f;
        }
        if (L_Y_plus < 127 && L_Y_minus == 0)
        {
            data_send_to_host[8] = 0x00; // left rocker
            data_send_to_host[9] = L_Y_plus;
        }

        if (L_Y_plus == 0 && L_Y_minus <= 0x80)
        {
            data_send_to_host[8] = 0x00; // left rocker
            data_send_to_host[9] = 0x80;
        }
        if (L_Y_plus == 0 && L_Y_minus != 0 && L_Y_minus > 0x80)
        {
            data_send_to_host[8] = 0x00; // left rocker
            data_send_to_host[9] = L_Y_minus;
        }
    }
#endif
#endif /* left rocker value buffer */

#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[7])
    {
        count_all_func[7] = 0;
        printf("------ %s -- end\n", __func__);
    }
    timer_send_flag = 0;
#endif
}
void right_read_rocker(void)
{
#if FUNC_TIMESTAMP
    while (count_all_func[8])
    {
        count_all_func[8] = 0;
        printf("------ %s -- start\n", __func__);
    }
#endif

    /* Right */
    unsigned char R_X_plus = 0;  // X+
    unsigned char R_X_minus = 0; // X-
    unsigned char R_Y_plus = 0;  // Y+
    unsigned char R_Y_minus = 0; // Y-

#if RIGHT_ROCKER
    R_rocker_ad_key_x = adc_get_value(5); // ADC5
    R_rocker_ad_key_y = adc_get_value(6); // ADC6

    /* right rocker */
    if ((DEADBAND_X2 <= R_rocker_ad_key_x) || (DEADBAND_X1 >= R_rocker_ad_key_x))
    {
        if (DEADBAND_X2 <= R_rocker_ad_key_x) // X+
        {
            float temp_X_plus_f = (R_rocker_ad_key_x - DEADBAND_X2) / 3.45; // external deadband, right side of the axis
            R_X_plus = (unsigned char)temp_X_plus_f;
            R_X_minus = 0;
        }
        if (DEADBAND_X1 >= R_rocker_ad_key_x) // X-
        {
            float temp_X_minus_f = (DEADBAND_X1 - R_rocker_ad_key_x) / 3.7; // external deadband, left side of the axis
            R_X_minus = 0xff - (unsigned char)temp_X_minus_f;
            R_X_plus = 0;
        }
    }
    if ((DEADBAND_X2 <= R_rocker_ad_key_y) || (DEADBAND_X1 >= R_rocker_ad_key_y))
    {
        if (DEADBAND_X2 <= R_rocker_ad_key_y) // Y+
        {
            float temp_Y_plus_f = (R_rocker_ad_key_y - DEADBAND_X2) / 3.2; // external deadband, up side of the axis
            R_Y_plus = (unsigned char)temp_Y_plus_f;
            R_Y_minus = 0;
        }
        if (DEADBAND_X1 >= R_rocker_ad_key_y) // Y-
        {
            float temp_Y_minus_f = (DEADBAND_X1 - R_rocker_ad_key_y) / 3.76; // external deadband, down side of the axis
            R_Y_minus = 0xff - (unsigned char)temp_Y_minus_f;
            R_Y_plus = 0;
        }
    }
#endif /* right rocker */

#if RIGHT_ROCKER_BUFFER /* right rocker value buffer */
#if RIGHT_ROCKER_X_AXIS /* X axis */
    if (R_X_plus != 0 || R_X_minus != 0)
    {
        if (R_X_plus >= 127 && R_X_minus == 0) // X+
        {
            data_send_to_host[10] = 0xff; // right rocker
            data_send_to_host[11] = 0x7f;
        }
        if (R_X_plus < 127 && R_X_minus == 0)
        {
            data_send_to_host[10] = 0x00; // right rocker
            data_send_to_host[11] = R_X_plus;
        }

        if (R_X_plus == 0 && R_X_minus <= 0x80) // X-
        {
            data_send_to_host[10] = 0x00; // right rocker
            data_send_to_host[11] = 0x80;
        }
        if (R_X_plus == 0 && R_X_minus != 0 && R_X_minus > 0x80)
        {
            data_send_to_host[10] = 0x00; // right rocker
            data_send_to_host[11] = R_X_minus;
        }
    }
#endif

    if ((R_X_minus == 0 && R_X_plus == 0) && (R_X_minus == 0 && R_Y_plus == 0)) // median value
    {
        data_send_to_host[10] = 0x00; // right rocker X
        data_send_to_host[11] = 0x00;
        data_send_to_host[12] = 0x00; // right rocker Y
        data_send_to_host[13] = 0Xff;
    }

#if RIGHT_ROCKER_Y_AXIS /* Y axis */
    if (R_Y_plus != 0 || R_Y_minus != 0)
    {
        if (R_Y_plus >= 127 && R_Y_minus == 0)
        {
            data_send_to_host[12] = 0xff; // right rocker
            data_send_to_host[13] = 0x7f;
        }
        if (R_Y_plus < 127 && R_Y_minus == 0)
        {
            data_send_to_host[12] = 0x00; // right rocker
            data_send_to_host[13] = R_Y_plus;
        }

        if (R_Y_plus == 0 && R_Y_minus <= 0x80)
        {
            data_send_to_host[12] = 0x00; // right rocker
            data_send_to_host[13] = 0x80;
        }
        if (R_Y_plus == 0 && R_Y_minus != 0 && R_Y_minus > 0x80)
        {
            data_send_to_host[12] = 0x00; // right rocker
            data_send_to_host[13] = R_Y_minus;
        }
    }
#endif
#endif /* right rocker value buffer */

#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[9])
    {
        count_all_func[9] = 0;
        printf("------ %s -- end\n", __func__);
    }
    timer_send_flag = 0;
#endif
}
void my_led_function(void)
{
#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[0])
    {
        count_all_func[0] = 0;
        printf("------ %s -- start\n", __func__);
    }
    timer_send_flag = 0;
#endif

    // 清除初始化的占空比值 Clear the initialised duty cycle value
    while (motor_flag)
    {
        unsigned int register_temp = JL_USB->CON0;
        register_temp = ((register_temp << 18) >> 31); // get USB_CON0 register, 13th bit, SOF_PND
        if (register_temp)
        {
            mcpwm_set_duty(pwm_ch0, pwm_timer0, 0);
            mcpwm_set_duty(pwm_ch1, pwm_timer1, 0);
            motor_flag = 0;
        }
    }

#if PWM_MOTOR  /* PWM Motor */
#if LEFT_MOTOR /* left motor */
    if (data_send_to_host[4] == trigger[0])
    {
        if (trigger[0] != 0)
            trigger_value_L = trigger[0]; // receive OUT packet form Xbox360input.exe
        else
            trigger_value_L = 0;

        float temp = trigger_value_L * 39.22;
        trigger_value_L = temp;
        if (trigger_value_L >= 10000)
            trigger_value_L = 10000;

        mcpwm_set_duty(pwm_ch0, pwm_timer0, trigger_value_L);

        // my_pwm_led_on_display(1, 0, trigger_value_L);
    }
#endif /* left motor */

#if RIGHT_MOTOR /* right motor */
    if (data_send_to_host[5] == trigger[1])
    {
        if (trigger[1] != 0)
            trigger_value_R = trigger[1]; // receive OUT packet form Xbox360input.exe
        else
            trigger_value_R = 0;

        float temp = trigger_value_R * 39.22;
        trigger_value_R = temp;
        if (trigger_value_R >= 10000)
            trigger_value_R = 10000;
        // my_pwm_led_on_display(1, 0, trigger_value_L);
        mcpwm_set_duty(pwm_ch1, pwm_timer1, trigger_value_R);
    }
#endif /* right motor */
#endif

    // printf("watch dog stutas : %x\n", p33_rx_1byte(P3_WDT_CON));  // use this function read watch dog status from this address

    /* Cannot be re-entered for a short time */
    // pwm_led_breathe_display(1, 500, BRIGHT, BRIGHT, 0, 100, 0);

#if MY_PRINTF
    if ((tcc_count % (1000 / LED_TCC_TIMER)) == 0)
        printf("---------- %s ----------", __func__);
#endif

#if FUNC_TIMESTAMP
    timer_send_flag = 1;
    while (count_all_func[1])
    {
        count_all_func[1] = 0;
        printf("------ %s -- end \n", __func__);
    }
    timer_send_flag = 0;
#endif
}
void xbox360_connect_flicker(void)
{
    if (player_flicker_time == 1 && player_IO_status == 1)
    {
        exit_flicker = 1;
        return;
    }

    switch (player_led)
    {
    case 1:
    { /* player 1 */
        if ((tcc_conut_led % (240 / LED_TCC_TIMER)) == 0)
        {
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_03, (int)player_IO_status);
            printf("____---- %s ----____\n", __func__);
#if RECORD_MOVEMENT
            record_player_led = IO_PORTA_03;
#endif
        }
    }
    break;
    case 2:
    {
        if ((tcc_conut_led % (240 / LED_TCC_TIMER)) == 0)
        {
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_02, (int)player_IO_status);
#if RECORD_MOVEMENT
            record_player_led = IO_PORTA_02;
#endif
        }
    }
    break;
    case 3:
    {
        if ((tcc_conut_led % (240 / LED_TCC_TIMER)) == 0)
        {
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_01, (int)player_IO_status);
#if RECORD_MOVEMENT
            record_player_led = IO_PORTA_01;
#endif
        }
    }
    break;
    case 4:
    {
        if ((tcc_conut_led % (240 / LED_TCC_TIMER)) == 0)
        {
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_04, (int)player_IO_status);
#if RECORD_MOVEMENT
            record_player_led = IO_PORTA_04;
#endif
        }
    }
    break;
    default:
    {
    }
    break;
    }
}
void records_movement(void)
{
#if RECORD_MOVEMENT
#if 1   // record_func key
    if ((gpio_read(IO_PORTC_05)) ^ 0x01)    // records start key
    {
        //RecordFunc_key_times++; // add seconds
        if((tcc_count % (800 / MAIN_TCC_TIMER)) == 0 && (gpio_read(IO_PORTC_05)) ^ 0x01)    // 延时检测按键
        {
            records_flag = 1;       // records start
            records_movement_key = 1;
            PWM_temp = 750 * 7;
            mcpwm_set_duty(pwm_ch0, pwm_timer0, PWM_temp);
            for (int i = 0; i < SUCCESSIVE_KEYS_NUMBER_ENABLE; i++) // 保存连点按键
                successive_temp[i] = successive_press_keys[i];
        }
    }
    if (gpio_read(IO_PORTC_02) ^ 0x01)   // records end key
    {
        records_flag = 3;   // records end
        PWM_temp = 750 * 7;
        mcpwm_set_duty(pwm_ch0, pwm_timer0, PWM_temp);
    }
    if (gpio_read(IO_PORTC_04) ^ 0x01)   // reappear record key
    {
        records_flag = 5;   // reappear record
        PWM_temp = 1250 * 6;
        mcpwm_set_duty(pwm_ch0, pwm_timer0, PWM_temp);
    }
#endif
    if (((gpio_read(IO_PORTC_05)) == 1) && ((gpio_read(IO_PORTC_02)) == 1) && ((gpio_read(IO_PORTC_04)) == 1)) // 松开按键执行功能
    {
        if(PWM_temp)
        {
            PWM_temp = 0;
            mcpwm_set_duty(pwm_ch0, pwm_timer0, PWM_temp);
        }

        switch (records_flag)
        {
        case 1: /* record */
        {
            for (int i = 0; i < SUCCESSIVE_KEYS_NUMBER_ENABLE; i++) // 禁用连点功能
                successive_press_keys[i] = 0;
            if (effective_record_ms >= (TIMES_LIMIT * 1000 / MAIN_TCC_TIMER)) // 限制记录时间
            {
                printf("------ Maximum recorded time reached ! ! ! %d (s) -------", TIMES_LIMIT);
                records_flag = 3;
                break;
            }
            ban_flag = 1;
            if ((tcc_count % (2000 / MAIN_TCC_TIMER)) == 0)
                printf("---- recording ----");

            if((tcc_count % (800 / MAIN_TCC_TIMER)) == 0)   // 闪烁指示灯, 表明状态
            {
                player_IO_status ^= 1;
                gpio_direction_output(record_player_led, (int)player_IO_status);
            }
            if( time_flag ) // 时间标志与存储时间的变量不为零, 需要清零
            {
                for(int i = 0; i < records_length_temp; i++)
                    (*((unsigned short *)records_keys_point + 1 + (2 * i))) = 0;
                time_flag = 0;
                printf("clear times");
            }

            if (data_send_to_host_temp[2] == data_send_to_host[2] && data_send_to_host_temp[3] == data_send_to_host[3] && time_flag == 0)
            {
                if (key_start_press)
                {
                    (*((unsigned short*)records_keys_point + 1 + (2 * records_length)))++; //  记录持续时间, record duration
                    if ((*((unsigned short*)records_keys_point + 1 + (2 * records_length))) == ONCE_MAX_RECORD_TIMES)  // 超出单次按键时间最大记录, 长度+1
                        goto length_add;
                    reappear_times_temp[records_length] = (*((unsigned short*)records_keys_point + 1 + (2 * records_length))); // 给临时时间记录赋值
                }

            }
            else
            {
                length_add:
                unsigned char temp = records_length;
                if (records_length < MAX_RECORD_ARRAY_LEN)  // 记录溢出前
                    records_length++;
                else    // 记录溢出时
                {
                    printf("Error ! ! !  Record overflow ! \nRecords have been saved...");
                    records_flag = 3;
                    break;
                }
                *((unsigned char *)records_keys_point + (CHAR_SIZE_NEXT * records_length)) = data_send_to_host[2],
                *((unsigned char *)records_keys_point + 1 + (CHAR_SIZE_NEXT * records_length)) = data_send_to_host[3],
                (*((unsigned short *)records_keys_point + 1 + (SHORT_SIZE_NEXT * records_length)))++; //  记录持续时间, record duration
                if (records_length != temp)
                {
                    printf("%x\n%x", *((unsigned char *)records_keys_point + (CHAR_SIZE_NEXT * records_length)),
                    *((unsigned char *)records_keys_point + 1 + (CHAR_SIZE_NEXT * records_length)));
                }
            }
            effective_record_ms++;
        }
        break;
        case 3: /* record end */
        {
            printf("---- record end ----");

            // 按键按下记录清零
            records_movement_key = 0;
            key_start_press = 0;

            gpio_direction_output(record_player_led, 1);    // 结束记录状态指示灯常亮
            if (effective_record_ms)
                effective_record_ms = 0;    // 总时长清零

            for (int i = 0; i < SUCCESSIVE_KEYS_NUMBER_ENABLE; i++) // 恢复连点功能
                successive_press_keys[i] = successive_temp[i];

            records_flag = 0;                       // records ready
            records_length_temp = records_length;   // 该临时变量不用作计数, 用作比较
            *(records_keys_point + MAX_RECORD_ARRAY_LEN) = records_length_temp; // 保存当前编程按键记录的长度
            if(ban_flag)     // 有过记录才运行该标志位置0, 放行case 5
                ban_flag = 0;

#if VM_TEST   // 读VM需耗时75ms, 写VM需耗时38ms, 超出了定时器限制会导致任务队列溢出, 需要限制任务队列的发送

            timer_send_flag = 1;    // VM开始进行读写操作
            unsigned char write_flag = 0;

            //int ret_r = syscfg_read(1, buf_point, NUM_FOR_VM_LEN * 4);
            //printf("VM read ret: %d", ret_r);

            try_again_write:
            int ret_w = syscfg_write(1, buf_point, (NUM_FOR_VM_LEN + LEN_RECORD_VM) * 4); // 此时buf_point是有记录的真实数据
            write_flag++;
            if (ret_w < 0 || write_flag >= 2)   // 写入失败, 失败次数大于3
            {
                if (write_flag < 2 && ret_w < 0)
                {
                    goto try_again_write;
                }
                else if(write_flag < 2 && ret_w > 0)
                {
                    goto write_success;
                }
                else
                {
                    printf("    VM write ret: %d, WRITE ERROR ! ! !", ret_w);
                }

            }
            write_success:
            timer_send_flag = 0;
#endif

        }
        break;
        case 0: /* record ready */
        {
            //player_flicker_time = 1;    // 避免上电闪灯与record指示灯闪灯冲突
            //RecordFunc_key_times = 0;   // 按键按下时长清零, 在此处清零, 确保长时间按下不会重复触发case 1, case 3
            records_length = 0;         // 初始化记录长度
            if(!time_flag)
                time_flag = 1;
        }
        break;
        case 5: /* reappear record */
        {
            if(ban_flag == 1)
            {
                printf(" WARNING !  >>> Record end !");
                records_flag = 3;   // 先跳转到结束(case 3)
                break;
            }
            if((records_length_temp - 1) == 0xffffffff && reappear_times_temp[records_length_temp] == 0) // 尚未有过历史记录
            {
                printf(" WARNING !  >>> No history record !");
                records_flag = 0;
                break;
            }
            if(!records_length_temp)    // 若该变量被提前清零, 但记录是有数据的
            {
                printf(" WARNING !  >>> Record cleared !");
                records_flag = 0;
                break;
            }

/* keys value reappear */
            for (; reappear_record < records_length_temp;)
            {
                data_send_to_host[2] = *((unsigned char *)records_keys_point + (CHAR_SIZE_NEXT * reappear_record));      // 赋键值
                data_send_to_host[3] = *((unsigned char *)records_keys_point + 1 + (CHAR_SIZE_NEXT * reappear_record));
                if (reappear_times_temp[reappear_record]-- != 0 && reappear_times_temp[reappear_record] < ONCE_MAX_RECORD_TIMES)
                    break;
                else
                    reappear_record++;  // 下一次按键状态
            }

            if (!(reappear_record < records_length_temp) && reappear_times_temp[records_length_temp - 1] == 0xffff)    //  计数完毕进入此判断
            {
                printf("---- !(reappear_record < records_length_temp) ---- %d < %d ", reappear_record, records_length_temp);
                for(int i = 0; i < records_length_temp; i++)
                {
                    reappear_times_temp[i] = (*((unsigned short *)records_keys_point + 1 + (SHORT_SIZE_NEXT * i)));
                }
                reappear_record = 0;
                records_flag = 0;
            }
        }
        break;
        default:
            break;
        }
    }
#endif
    return;
}
/*****************************************************************************************************/

/***************************************** PS3 function *****************************************/
void ps3_read_key(void)
{

#if READ_KEY
    a_key_val = (gpio_read(IO_PORTB_09)) ^ 0x01;
    b_key_val = (gpio_read(IO_PORTB_06)) ^ 0x01;
    x_key_val = (gpio_read(IO_PORTB_11)) ^ 0x01;
    y_key_val = (gpio_read(IO_PORTB_04)) ^ 0x01;

    my_key_val_4 = (gpio_read(IO_PORTA_08)) ^ 0x01; // ↑
    my_key_val_3 = (gpio_read(IO_PORTA_11)) ^ 0x01; // ↓
    my_key_val_1 = (gpio_read(IO_PORTA_12)) ^ 0x01; // ←
    my_key_val_2 = (gpio_read(IO_PORTA_14)) ^ 0x01; // →

    start_io_key = (gpio_read(IO_PORTC_00)) ^ 0x01;
    back_io_key = (gpio_read(IO_PORTC_07)) ^ 0x01;

    L_rocker_io_key = (gpio_read(IO_PORTA_07)) ^ 0x01;
    R_rocker_io_key = (gpio_read(IO_PORTB_02)) ^ 0x01;

    L_1_io_key = (gpio_read(IO_PORTB_05)) ^ 0x01;
    R_1_io_key = (gpio_read(IO_PORTB_07)) ^ 0x01;

    ps_key = (gpio_read(IO_PORTC_05)) ^ 0x01;
#endif

#if MERGE_KEY
    io_key_status = 0x00;                                           /* using the variant have to set zero , in the after assign the value*/
    io_key_status = merge_value(io_key_status, start_io_key, 0);    // start
    io_key_status = merge_value(io_key_status, L_rocker_io_key, 1);
    io_key_status = merge_value(io_key_status, R_rocker_io_key, 2);
    io_key_status = merge_value(io_key_status, back_io_key, 3);     // select
    io_key_status = merge_value(io_key_status, my_key_val_1, 4);    // ↑
    io_key_status = merge_value(io_key_status, my_key_val_2, 5);    // →
    io_key_status = merge_value(io_key_status, my_key_val_3, 6);    // ↓
    io_key_status = merge_value(io_key_status, my_key_val_4, 7);    // ←
    ps3_data_send_to_host[2] = io_key_status;
    ps3_data_send_to_host[14] = my_key_val_1 ? 0xff : 0;    // ↑ ADC value
    ps3_data_send_to_host[15] = my_key_val_2 ? 0xff : 0;    // → ADC value
    ps3_data_send_to_host[16] = my_key_val_3 ? 0xff : 0;    // ↓ ADC value
    ps3_data_send_to_host[17] = my_key_val_4 ? 0xff : 0;    // ← ADC value


    io_key_status = 0x00;                                           /* using the variant have to set zero , in the after assign the value*/
    L_trigger_ad_key = adc_get_value(8);
    int ret_left = (L_trigger_ad_key > TRIGGER_PRESS_VAL) ? 1 : 0;  // 左扳机有按下1, 没有按下0
    io_key_status = merge_value(io_key_status, ret_left, 0);

    R_trigger_ad_key = adc_get_value(9);
    int ret_right = (R_trigger_ad_key > TRIGGER_PRESS_VAL) ? 1 : 0; // 右扳机有按下1, 没有按下0
    io_key_status = merge_value(io_key_status, ret_right, 1);

    io_key_status = merge_value(io_key_status, L_1_io_key, 2);     //  left_1
    io_key_status = merge_value(io_key_status, R_1_io_key, 3);     // right_1
    io_key_status = merge_value(io_key_status, y_key_val, 4);      // △
    io_key_status = merge_value(io_key_status, b_key_val, 5);      // ○
    io_key_status = merge_value(io_key_status, x_key_val, 6);      // ×
    io_key_status = merge_value(io_key_status, a_key_val, 7);      // □
    ps3_data_send_to_host[3] = io_key_status;   // 只有键值同样也能识别, 区别可能在于在游戏中无法触发, 可能需要额外的ADC值
    ps3_data_send_to_host[20] = L_1_io_key ? 0xff : 0;  // L1, ADC value
    ps3_data_send_to_host[21] = R_1_io_key ? 0xff : 0;  // R1, ADC value
    ps3_data_send_to_host[22] = y_key_val ? 0xff : 0;   // △, ADC value
    ps3_data_send_to_host[23] = b_key_val ? 0xff : 0;   // ○, ADC value
    ps3_data_send_to_host[24] = x_key_val ? 0xff : 0;   // ×, ADC value
    ps3_data_send_to_host[25] = a_key_val ? 0xff : 0;   // □, ADC value


    io_key_status = 0x00;                                           /* using the variant have to set zero , in the after assign the value*/
    io_key_status = merge_value(io_key_status, ps_key, 0);
    ps3_data_send_to_host[4] = io_key_status;


    /*extern unsigned char ps3_player_ID;
    ps3_data_send_to_host[0] = ps3_player_ID;*/
#endif


}
void ps3_read_trigger(void)
{
#if TRIGGER
    L_trigger_ad_key = adc_get_value(8);
    R_trigger_ad_key = adc_get_value(9);
#if LEFT_TRIGGER /* left */
    if (L_trigger_ad_key > TRIGGER_PRESS_VAL)
    {
        int send_condition = L_trigger_temp - (L_trigger_ad_key - TRIGGER_INIT_VAL); // jitter judgement
        if (send_condition > 4 || send_condition < (-4))
        {                                                                 // eliminate jitter
            ps3_data_send_to_host[18] = (L_trigger_ad_key - TRIGGER_INIT_VAL); // left trigger, L2
            L_trigger_temp = ps3_data_send_to_host[18];
        }

        if ((L_trigger_ad_key - TRIGGER_INIT_VAL) > 0xff)
        {
            ps3_data_send_to_host[18] = 0xff;
            L_trigger_temp = 0xff;
        }
    }
    else
    {
        ps3_data_send_to_host[18] = 0x00;
        L_trigger_temp = 0x00;
    }

#endif
#if RIGHT_TRIGGER /* right */
    if (R_trigger_ad_key > TRIGGER_PRESS_VAL)
    {
        int send_condition = R_trigger_temp - (R_trigger_ad_key - TRIGGER_INIT_VAL); // jitter judgement
        if (send_condition > 5 || send_condition < (-5))
        {                                                                 // eliminate jitter
            ps3_data_send_to_host[19] = (R_trigger_ad_key - TRIGGER_INIT_VAL); // right trigger, R2
            R_trigger_temp = ps3_data_send_to_host[19];
        }

        if ((R_trigger_ad_key - TRIGGER_INIT_VAL) > 0xff)
        {
            ps3_data_send_to_host[19] = 0xff;
            R_trigger_temp = 0xff;
        }
    }
    else
    {
        ps3_data_send_to_host[19] = 0x00;
        R_trigger_temp = 0x00;
    }

#endif
#endif
}
void ps3_left_read_rocker(void)
{
    unsigned char L_X_plus = 0;  // X+
    unsigned char L_X_minus = 0; // X-
    unsigned char L_Y_plus = 0;  // Y+
    unsigned char L_Y_minus = 0; // Y-

#if LEFT_ROCKER

    /* parametric is adc CHANNEL */
    my_rocker_ad_key_x = adc_get_value(1); // ADC1
    my_rocker_ad_key_y = adc_get_value(2); // ADC2


    if ((PS3_DEADBAND_X2 <= my_rocker_ad_key_x) || (PS3_DEADBAND_X1 >= my_rocker_ad_key_x))
    {
#if LEFT_ROCKER_BUFFER /* left rocker value buffer */
        if (PS3_DEADBAND_X2 <= my_rocker_ad_key_x)  // 0x7f~0xff, X+
        {
            float temp_X_plus_f = my_rocker_ad_key_x / 4.0;     //1023/4=255.75
            L_X_plus = (unsigned char)temp_X_plus_f;
            L_X_minus = 0;
#if LEFT_ROCKER_X_AXIS /* X axis */
            ps3_data_send_to_host[6] = L_X_plus;
#endif
        }
        if (PS3_DEADBAND_X1 >= my_rocker_ad_key_x)  // 0x7f~0x00, X-
        {
            float temp_X_minus_f = my_rocker_ad_key_x / 3.59;    // 460/3.59=128.133...
            L_X_minus = (unsigned char)temp_X_minus_f;
            L_X_plus = 0;
#if LEFT_ROCKER_X_AXIS /* X axis */
            ps3_data_send_to_host[6] = L_X_minus;
#endif
        }
#endif
    }
    else
    {
        ps3_data_send_to_host[6] = 0x7f;
    }


    if ((PS3_DEADBAND_X2 <= my_rocker_ad_key_y) || (PS3_DEADBAND_X1 >= my_rocker_ad_key_y)) // PS3的Y轴输入与AD值反转
    {
#if LEFT_ROCKER_BUFFER /* left rocker value buffer */
        if (PS3_DEADBAND_X2 <= my_rocker_ad_key_y)  // Y-
        {
            float temp_Y_plus_f = my_rocker_ad_key_y / 4.0;
            L_Y_plus = (unsigned char)temp_Y_plus_f;
            L_Y_minus = 0;
#if LEFT_ROCKER_Y_AXIS /* Y axis */
            ps3_data_send_to_host[7] = 0xff - L_Y_plus;
#endif
        }
        if (PS3_DEADBAND_X1 >= my_rocker_ad_key_y)  // Y+
        {
            float temp_Y_minus_f = my_rocker_ad_key_y / 3.59;
            L_Y_minus = (unsigned char)temp_Y_minus_f;
            L_Y_plus = 0;
#if LEFT_ROCKER_Y_AXIS /* Y axis */
            ps3_data_send_to_host[7] = 0x7f + (0x80 - L_Y_minus);
#endif
        }
#endif /* left rocker value buffer */
    }
    else
    {
        ps3_data_send_to_host[7] = 0x7f;
    }

#endif
#if LEFT_ROCKER_PRINT
    if ((tcc_count % (500 / MAIN_TCC_TIMER)) == 0)
    {
        timer_send_flag = 1;
        printf("LY: %d, LX: %d    ADC_Y: %d, ADC_X: %d", ps3_data_send_to_host[7], ps3_data_send_to_host[6], my_rocker_ad_key_y, my_rocker_ad_key_x);
        timer_send_flag = 0;
    }
#endif
}
void ps3_right_read_rocker(void)
{
    unsigned char R_X_plus = 0;  // X+
    unsigned char R_X_minus = 0; // X-
    unsigned char R_Y_plus = 0;  // Y+
    unsigned char R_Y_minus = 0; // Y-

#if RIGHT_ROCKER

    /* parametric is adc CHANNEL */
    R_rocker_ad_key_x = adc_get_value(5); // ADC5
    R_rocker_ad_key_y = adc_get_value(6); // ADC6


    if ((PS3_OUTSIDE_DEADBAND_X2 >=  R_rocker_ad_key_x) && (PS3_DEADBAND_X2 <= R_rocker_ad_key_x)
        || (PS3_DEADBAND_X1 >= R_rocker_ad_key_x) && (PS3_OUTSIDE_DEADBAND_X1 <= R_rocker_ad_key_x))
    {
#if RIGHT_ROCKER_BUFFER /* left rocker value buffer */
        if (PS3_DEADBAND_X2 <= R_rocker_ad_key_x)  // 0x7f~0xff, X+
        {
            float temp_X_plus_f = R_rocker_ad_key_x / 3.8;     //1023/3.8=269.210...
            R_X_plus = (unsigned char)temp_X_plus_f;
            R_X_minus = 0;
#if RIGHT_ROCKER_X_AXIS /* X axis */
            if (R_X_plus >= 222)   // 溢出跳轉, 否則會在這個值定住然後又不會翻滾( 在游戲 god of war )
                goto X_plusOverflow;    // 該辦法是非綫性的, 只是唔太想全部重新映射了, 費時
            ps3_data_send_to_host[8] = R_X_plus;
#endif
        }
        if (PS3_DEADBAND_X1 >= R_rocker_ad_key_x)  // 0x7f~0x00, X-
        {
            float temp_X_minus_f = (R_rocker_ad_key_x - (R_rocker_ad_key_x / 7.0)) / 3.3;    // (460-460/10)/3.3=139.3939...
            R_X_minus = (unsigned char)temp_X_minus_f;
            R_X_plus = 0;
#if RIGHT_ROCKER_X_AXIS /* X axis */
            if (R_X_minus <= 60)    // 該辦法是非綫性的, 只是唔太想全部重新映射了, 費時
                goto XminusOverflow;
            ps3_data_send_to_host[8] = R_X_minus;   // 斜摇杆无法达到边界
#endif
        }
#endif
    }
    else if(R_rocker_ad_key_x > PS3_OUTSIDE_DEADBAND_X2)
    {
        X_plusOverflow:
        ps3_data_send_to_host[8] = 0xff;
    }
    else if (PS3_OUTSIDE_DEADBAND_X1 > R_rocker_ad_key_x)
    {
        XminusOverflow:
        ps3_data_send_to_host[8] = 0x00;
    }
    else
    {
        ps3_data_send_to_host[8] = 0x7f;
    }



    if ((PS3_DEADBAND_X2 <= R_rocker_ad_key_y) || (PS3_DEADBAND_X1 >= R_rocker_ad_key_y)) // PS3的Y轴输入与AD值反转
    {
#if RIGHT_ROCKER_BUFFER /* left rocker value buffer */
        if (PS3_DEADBAND_X2 <= R_rocker_ad_key_y)  // Y-
        {
            float temp_Y_plus_f = R_rocker_ad_key_y / 4.0;
            R_Y_plus = (unsigned char)temp_Y_plus_f;
            R_Y_minus = 0;
#if RIGHT_ROCKER_Y_AXIS /* Y axis */
            ps3_data_send_to_host[9] = 0xff - R_Y_plus;
#endif
        }
        if (PS3_DEADBAND_X1 >= R_rocker_ad_key_y)  // Y+
        {
            float temp_Y_minus_f = R_rocker_ad_key_y / 3.59;
            R_Y_minus = (unsigned char)temp_Y_minus_f;
            R_Y_plus = 0;
#if RIGHT_ROCKER_Y_AXIS /* Y axis */
            if ((0x7f + (0x80 - R_Y_minus)) > 230) // 解決不能到達邊界的問題, 預先計算的值跟實際的值出現了偏差
            {   // 該辦法是非綫性的, 只是唔太想全部重新映射了, 費時
                ps3_data_send_to_host[9] =0xff;
            }
            else
            {
                ps3_data_send_to_host[9] = 0x7f + (0x80 - R_Y_minus);
            }

#endif
        }
#endif /* left rocker value buffer */
    }
    else
    {
        ps3_data_send_to_host[9] = 0x7f;
    }

#endif
#if RIGHT_ROCKER_PRINT
    if ((tcc_count % (510 / MAIN_TCC_TIMER)) == 0)
    {
        timer_send_flag = 1;
        printf("RY: %d, RX: %d    ADC_Y: %d, ADC_X: %d", ps3_data_send_to_host[9], ps3_data_send_to_host[8], R_rocker_ad_key_y, R_rocker_ad_key_x);
        timer_send_flag = 0;
    }
#endif
}
void ps3_player_led(void)
{
    extern unsigned char ps3_player_ID;
    switch (ps3_player_ID)
    {
    case 1: {
        gpio_direction_output(IO_PORTA_03, 1);
        gpio_direction_output(IO_PORTA_02, 0);
        gpio_direction_output(IO_PORTA_01, 0);
        gpio_direction_output(IO_PORTA_04, 0);
    }break;
    case 2: {
        gpio_direction_output(IO_PORTA_03, 0);
        gpio_direction_output(IO_PORTA_02, 1);
        gpio_direction_output(IO_PORTA_01, 0);
        gpio_direction_output(IO_PORTA_04, 0);
    }break;
    case 3: {
        gpio_direction_output(IO_PORTA_03, 0);
        gpio_direction_output(IO_PORTA_02, 0);
        gpio_direction_output(IO_PORTA_01, 1);
        gpio_direction_output(IO_PORTA_04, 0);
    }break;
    case 4: {
        gpio_direction_output(IO_PORTA_03, 0);
        gpio_direction_output(IO_PORTA_02, 0);
        gpio_direction_output(IO_PORTA_01, 0);
        gpio_direction_output(IO_PORTA_04, 1);
    }break;
    case 5: {
        gpio_direction_output(IO_PORTA_03, 1);
        gpio_direction_output(IO_PORTA_02, 0);
        gpio_direction_output(IO_PORTA_01, 0);
        gpio_direction_output(IO_PORTA_04, 1);
    }break;
    case 6: {
        gpio_direction_output(IO_PORTA_03, 0);
        gpio_direction_output(IO_PORTA_02, 1);
        gpio_direction_output(IO_PORTA_01, 0);
        gpio_direction_output(IO_PORTA_04, 1);
    }break;
    case 7: {
        gpio_direction_output(IO_PORTA_03, 0);
        gpio_direction_output(IO_PORTA_02, 0);
        gpio_direction_output(IO_PORTA_01, 1);
        gpio_direction_output(IO_PORTA_04, 1);
    }break;
    default: {
        gpio_direction_output(IO_PORTA_03, 0);
        gpio_direction_output(IO_PORTA_02, 0);
        gpio_direction_output(IO_PORTA_01, 0);
        gpio_direction_output(IO_PORTA_04, 0);
    }break;
    }
}
void ps3_PWM_shake(void)/**/
{
    extern u8 ps3_out_DMA[64];  // 獲取主機所發送的震動值

    timer_send_flag = 0;    // 实际中出现了任务队列溢出的情况, 试图关闭定时器改善超时导致OS重启
    mcpwm_set_duty(pwm_ch1, pwm_timer1, (ps3_out_DMA[5] * 100));
    mcpwm_set_duty(pwm_ch0, pwm_timer0, (ps3_out_DMA[3] * 100));
    timer_send_flag = 0;
}
/*****************************************************************************************************/

static unsigned char change_host = 1;
extern void usbstack_exit();
void *my_task(void *p_arg)
{
    extern unsigned char usb_connect_timeout_flag;  // from task_pc.c
    int msg[8]; // in the array, recive task queue
    while (1)
    {
        /* receive task queue */
        int ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        //printf("====== OS_TASK: %d, USB->CON0: %x, msg[0]: %x", ret, JL_USB->CON0, msg[0]);
        if (ret != OS_TASKQ)    // 没有收到任务队列跳出一次循环
            continue;

        switch (msg[0])
        {
        case MAIN_TCC_TASK:
        {
            /*if ((tcc_count % (1000 / MAIN_TCC_TIMER)) == 0)
                printf("---------- what happend ? ? ? ---------- %d", usb_connect_timeout_flag);*/
            //gpio_direction_output(IO_PORTA_01, 1);
            if (usb_connect_timeout_flag == 1)
            {
                my_read_key();
                records_movement();
                left_read_rocker();
                right_read_rocker();
                read_trigger_value();
                send_data_to_host();
                if ((tcc_count % (1000 / MAIN_TCC_TIMER)) == 0)
                    printf("---------- %s ---------- %d", __func__, usb_connect_timeout_flag);
            }
            if (usb_connect_timeout_flag == 2)
            {

                ps3_read_key();
                ps3_left_read_rocker();
                ps3_right_read_rocker();
                ps3_read_trigger();
                ps3_player_led();
                timer_send_flag = 0;
                ps3_send_to_host();
                timer_send_flag = 0;
                ps3_PWM_shake();
                extern u8 ps3_out_DMA[64];
                if ((tcc_count % (1000 / MAIN_TCC_TIMER)) == 0)
                {
                    timer_send_flag = 1;
                    /*printf("OUT DMA DATA\n %x %x, %x %x, %x %x, %x %x, %x %x,\n %x %x, %x %x, %x %x, %x %x, %x %x,\n %x %x,\
 %x %x, %x %x, %x %x, %x %x,\n %x %x, %x %x, %x %x, %x %x, %x %x,\n %x %x, %x %x, %x %x, %x %x, %x %x", ps3_out_DMA[0], ps3_out_DMA[1],
            ps3_out_DMA[2], ps3_out_DMA[3], ps3_out_DMA[4], ps3_out_DMA[5], ps3_out_DMA[6], ps3_out_DMA[7], ps3_out_DMA[8], ps3_out_DMA[9],
            ps3_out_DMA[10], ps3_out_DMA[11], ps3_out_DMA[12], ps3_out_DMA[13], ps3_out_DMA[14], ps3_out_DMA[15], ps3_out_DMA[16], ps3_out_DMA[17],
            ps3_out_DMA[18], ps3_out_DMA[19], ps3_out_DMA[20], ps3_out_DMA[21], ps3_out_DMA[22], ps3_out_DMA[23], ps3_out_DMA[24], ps3_out_DMA[25],
            ps3_out_DMA[26], ps3_out_DMA[27], ps3_out_DMA[28], ps3_out_DMA[29], ps3_out_DMA[30], ps3_out_DMA[31], ps3_out_DMA[32], ps3_out_DMA[33],
            ps3_out_DMA[34], ps3_out_DMA[35], ps3_out_DMA[36], ps3_out_DMA[37], ps3_out_DMA[38], ps3_out_DMA[39], ps3_out_DMA[40], ps3_out_DMA[41],
            ps3_out_DMA[42], ps3_out_DMA[43], ps3_out_DMA[44], ps3_out_DMA[45], ps3_out_DMA[46], ps3_out_DMA[47], ps3_out_DMA[48], ps3_out_DMA[49]);*/
                    printf("---------- %s ---------- %d, LX>%d %d<LY, RX>%d %d<RY", __func__, usb_connect_timeout_flag, ps3_data_send_to_host[6],
                        ps3_data_send_to_host[7], ps3_data_send_to_host[8], ps3_data_send_to_host[9]);
                    timer_send_flag = 0;
                }
            }
            //gpio_direction_output(IO_PORTA_01, 0);
#if CHECK_MEMORY
            if ((tcc_count % (500 / MAIN_TCC_TIMER)) == 0)
            {
                timer_send_flag = 1;
                mem_stats();    // 实时查看RAM
                timer_send_flag = 0;
            }
#endif
        }break;
        case BREATHE_LED_TASK:
        {
            if (usb_connect_timeout_flag == 1)
            {
                my_led_function();
                if (exit_flicker)
                    break;
                xbox360_connect_flicker();
            }
            if (usb_connect_timeout_flag == 2)
            {

            }
        }break;
        case SPECIAL_FUNCTIONS:
        {
            extern unsigned char PS3_host_flag; // from hid.c
            if (!PS3_host_flag) // 判斷是否有接收到PS3主機的OUT包
            {
                timer_send_flag = 1;
                /*我都唔知道此處是否有用*/
                usb_stop();     // 停止
                usbstack_exit();// 退出
                /*我都唔知道此處是否有用*/
                //usb_iomode(0);  // 關閉
                gpio_set_direction(IO_PORT_DP, 0);  // 設置輸出
                gpio_set_direction(IO_PORT_DM, 0);
                gpio_set_pull_up(IO_PORT_DP, 0);    // 非上拉
                gpio_set_pull_up(IO_PORT_DM, 0);
                gpio_set_pull_down(IO_PORT_DP, 1);  // 下拉
                gpio_set_pull_down(IO_PORT_DM, 1);
                gpio_write(IO_PORT_DP, 1);          // 輸出1
                gpio_write(IO_PORT_DM, 1);
                //os_time_dly(250);
                //delay(10000);
                /************************/
                printf(" RUN USB release ! ! !");
                if (usb_connect_timeout_flag == 1)
                    usb_connect_timeout_flag = 2;   // 切換平臺所用的描述符
                else
                {
                    usb_connect_timeout_flag = 1;
                }
                printf("file:%s, line:%d", __FILE__, __LINE__);
                usbstack_init();
                usb_start();
                timer_send_flag = 0;
                change_host = 0;    // 更換平臺描述符后停止發送觸發當前任務的信號
            }
        }break;
        default:
            if ((tcc_count % (1000 / MAIN_TCC_TIMER)) == 0)
                printf("---------- %s ---------- %d", __func__, usb_connect_timeout_flag);
            break;
        }
    }
}

void my_button_init(void)
{
    /* Before using IOs, it is important to note whether any IOs have been and are occupied beforehand */
    printf("---------- %s ----------\n", __func__);

    /* set IO output */
    // gpio_set_direction(IO_PORTA_01, 0);         // ADC, PWM motor
    gpio_set_direction(IO_PORTB_00, 0); // PWM, ch1, high
    gpio_set_direction(IO_PORTA_00, 0); // PWM, ch0, high

    gpio_set_direction(IO_PORTA_02, 0); /* LED1 */
    gpio_set_direction(IO_PORTA_04, 0); /* LED3 */
    gpio_set_direction(IO_PORTA_01, 0); /* LED0 */
    gpio_set_direction(IO_PORTA_03, 0); /* LED2 */

    /* set IO input */
    gpio_set_direction(IO_PORTA_05, 1); // ADC, rocker L_X
    gpio_set_direction(IO_PORTA_06, 1); // ADC, rocker L_Y
    gpio_set_direction(IO_PORTA_07, 1); // IO,  rocker L_3

    gpio_set_direction(IO_PORTA_08, 1); /* ↑ */
    gpio_set_direction(IO_PORTA_11, 1); /* ↓ */
    gpio_set_direction(IO_PORTA_12, 1); /* ← */
    gpio_set_direction(IO_PORTA_14, 1); /* → */

    gpio_set_direction(IO_PORTB_09, 1); /* X */
    gpio_set_direction(IO_PORTB_11, 1); /* A */
    gpio_set_direction(IO_PORTB_04, 1); /* B */
    gpio_set_direction(IO_PORTB_06, 1); /* Y */

    gpio_set_direction(IO_PORTB_02, 1); // IO,  rocker R_3
    gpio_set_direction(IO_PORTB_01, 1); // ADC, rocker R_X
    gpio_set_direction(IO_PORTB_03, 1); // ADC, rocker R_Y

    gpio_set_direction(IO_PORTB_05, 1); // IO, L_1
    gpio_set_direction(IO_PORTB_07, 1); // IO, R_1

    gpio_set_direction(IO_PORTB_08, 1); // ADC, left trigger
    gpio_set_direction(IO_PORTB_10, 1); // ADC, right trigger

    gpio_set_direction(IO_PORTC_00, 1); // start key
    gpio_set_direction(IO_PORTC_01, 1); // Xbox Bar
    gpio_set_direction(IO_PORTC_03, 1); // successive press key
    gpio_set_direction(IO_PORTC_05, 1); // records movement key
    gpio_set_direction(IO_PORTC_07, 1); // back key

    gpio_set_direction(IO_PORTC_02, 1); /* record end */
    gpio_set_direction(IO_PORTC_04, 1); /* reappear record */

    printf("set output or input\n");

    /*************************************************************************************/

    adc_init();
    gpio_set_die(IO_PORTA_05, 0); // analogue input
    gpio_set_die(IO_PORTA_06, 0);
    gpio_set_die(IO_PORTA_07, 1); // digital input

    gpio_set_die(IO_PORTA_08, 1); /* ↑ */
    gpio_set_die(IO_PORTA_11, 1); /* ↓ */
    gpio_set_die(IO_PORTA_12, 1); /* ← */
    gpio_set_die(IO_PORTA_14, 1); /* → */

    gpio_set_die(IO_PORTB_09, 1); /* X */
    gpio_set_die(IO_PORTB_11, 1); /* A */
    gpio_set_die(IO_PORTB_04, 1); /* B */
    gpio_set_die(IO_PORTB_06, 1); /* Y */

    gpio_set_die(IO_PORTB_02, 1); // IO,  rocker R_3
    gpio_set_die(IO_PORTB_01, 0); // ADC, rocker R_X
    gpio_set_die(IO_PORTB_03, 0); // ADC, rocker R_Y

    gpio_set_die(IO_PORTB_05, 1); // IO, L_1
    gpio_set_die(IO_PORTB_07, 1); // IO, R_1

    gpio_set_die(IO_PORTB_08, 0); // ADC, left trigger
    gpio_set_die(IO_PORTB_10, 0); // ADC, right trigger

    gpio_set_die(IO_PORTC_00, 1); // start key
    gpio_set_die(IO_PORTC_01, 1); // Xbox Bar
    gpio_set_die(IO_PORTC_03, 1); // successive press key
    gpio_set_die(IO_PORTC_05, 1); // records movement key
    gpio_set_die(IO_PORTC_07, 1); // back key

    gpio_set_die(IO_PORTC_02, 1); /* record end */
    gpio_set_die(IO_PORTC_04, 1); /* reappear record */

    printf("set output mode\n");
    /*************************************************************************************/

    gpio_set_pull_down(IO_PORTA_05, 0); // no pull down
    gpio_set_pull_down(IO_PORTA_06, 0);
    gpio_set_pull_down(IO_PORTA_07, 0);

    gpio_set_pull_down(IO_PORTB_01, 0);
    gpio_set_pull_down(IO_PORTB_03, 0);
    gpio_set_pull_down(IO_PORTB_02, 0);

    gpio_set_pull_down(IO_PORTB_08, 0);
    gpio_set_pull_down(IO_PORTB_05, 0);

    gpio_set_pull_down(IO_PORTB_10, 0);
    gpio_set_pull_down(IO_PORTB_07, 0);

    gpio_set_pull_down(IO_PORTA_08, 0); /* ↑ */
    gpio_set_pull_down(IO_PORTA_11, 0); /* ↓ */
    gpio_set_pull_down(IO_PORTA_12, 0); /* ← */
    gpio_set_pull_down(IO_PORTA_14, 0); /* → */

    gpio_set_pull_down(IO_PORTB_09, 0); /* X */
    gpio_set_pull_down(IO_PORTB_11, 0); /* A */
    gpio_set_pull_down(IO_PORTB_04, 0); /* B */
    gpio_set_pull_down(IO_PORTB_06, 0); /* Y */

    gpio_set_pull_down(IO_PORTA_02, 0); /* LED1 */
    gpio_set_pull_down(IO_PORTA_04, 0); /* LED3 */
    gpio_set_pull_down(IO_PORTA_01, 0); /* LED0 */
    gpio_set_pull_down(IO_PORTA_03, 0); /* LED2 */

    gpio_set_pull_down(IO_PORTC_00, 0); // start key
    gpio_set_pull_down(IO_PORTC_01, 0); // Xbox Bar
    gpio_set_pull_down(IO_PORTC_03, 0); // successive press key
    gpio_set_pull_down(IO_PORTC_05, 0); // records movement key
    gpio_set_pull_down(IO_PORTC_07, 0); // back key

    gpio_set_pull_down(IO_PORTC_02, 0); /* record end */
    gpio_set_pull_down(IO_PORTC_04, 0); /* reappear record */

    printf("set pull down or no pull down\n");
    /*************************************************************************************/

    gpio_set_pull_up(IO_PORTA_05, 0); // no pull up
    gpio_set_pull_up(IO_PORTA_06, 0);
    gpio_set_pull_up(IO_PORTA_07, 1); // set pull up

    gpio_set_pull_up(IO_PORTB_01, 0);
    gpio_set_pull_up(IO_PORTB_03, 0);
    gpio_set_pull_up(IO_PORTB_02, 1);

    gpio_set_pull_up(IO_PORTB_08, 0);
    gpio_set_pull_up(IO_PORTB_05, 1);

    gpio_set_pull_up(IO_PORTB_10, 0);
    gpio_set_pull_up(IO_PORTB_07, 1);

    gpio_set_pull_up(IO_PORTA_08, 1); /* ↑ */
    gpio_set_pull_up(IO_PORTA_11, 1); /* ↓ */
    gpio_set_pull_up(IO_PORTA_12, 1); /* ← */
    gpio_set_pull_up(IO_PORTA_14, 1); /* → */

    gpio_set_pull_up(IO_PORTB_09, 1); /* X */
    gpio_set_pull_up(IO_PORTB_11, 1); /* A */
    gpio_set_pull_up(IO_PORTB_04, 1); /* B */
    gpio_set_pull_up(IO_PORTB_06, 1); /* Y */

    gpio_set_pull_up(IO_PORTA_02, 1); /* LED1 */
    gpio_set_pull_up(IO_PORTA_04, 1); /* LED3 */
    gpio_set_pull_up(IO_PORTA_01, 1); /* LED0 */
    gpio_set_pull_up(IO_PORTA_03, 1); /* LED2 */

    gpio_set_pull_up(IO_PORTC_00, 1); // start key
    gpio_set_pull_up(IO_PORTC_01, 1); // Xbox Bar
    gpio_set_pull_up(IO_PORTC_03, 1); // successive press key
    gpio_set_pull_up(IO_PORTC_05, 1); // records movement key
    gpio_set_pull_up(IO_PORTC_07, 1); // back key

    gpio_set_pull_up(IO_PORTC_02, 1); /* record end */
    gpio_set_pull_up(IO_PORTC_04, 1); /* reappear record */

    printf("set pull up or no pull up\n");
    /*************************************************************************************/

    /* add sample adc CHANNEL */
    adc_add_sample_ch(1); // ADC1
    adc_add_sample_ch(2); // ADC2

    adc_add_sample_ch(5); // ADC5
    adc_add_sample_ch(6); // ADC6

    adc_add_sample_ch(8); // ADC8, left trigger
    adc_add_sample_ch(9); // ADC9, right triger

    log_print(__LOG_INFO, NULL, "Init button input mode\n");
}

void my_PWM_output_init(void)
{
#if 0 /* pwm led, only a commone channel output  */
    struct led_platform_data led_init;
    led_init.io_mode = LED_ONE_IO_MODE;
    led_init.io_cfg.one_io.pin = IO_PORTA_01;
    pwm_led_init(&led_init);

    pwm_led_io_max_drive_set(3);

    /*enable PWM_model*/
    pwm_led_set_on();
    int ret = is_pwm_led_on();
    if(ret == 0)
        log_print(__LOG_ERROR, NULL, "this PWM_modle is not enable\r\n");
    else
        log_print(__LOG_INFO, NULL, "this PWM_modle enable\r\n");
#endif
    printf("---------- %s ----------\n", __func__);
#if PWM_INIT
    struct pwm_platform_data pwm_motor_output_init = {0X00};
#if PWM_INIT
    pwm_motor_output_init.complementary_en = 1;       // 两个IO输出波形同步
    pwm_motor_output_init.frequency = 10000;          // 10KHz
    pwm_motor_output_init.pwm_timer_num = pwm_timer0; // 时间基准, PWM定时器0
    pwm_motor_output_init.h_pin = IO_PORTA_00;        // PWM output high IO, h_pin_output_ch_num 有效, 必须配置
    pwm_motor_output_init.h_pin_output_ch_num = 0;
    pwm_motor_output_init.l_pin = -1;                          // 不初始化 PWM output low, l_pin_output_ch_num 无效，可不配置
    pwm_motor_output_init.duty = 2000;                         // 占空比 0.01%
    pwm_motor_output_init.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    pwm_motor_output_init.pwm_ch_num = pwm_ch0;                // channel_0
    printf("---------- >>> will init\n");
    mcpwm_init(&pwm_motor_output_init); // PS: channel >= 3, not support output_channel
    printf("---------- >>> mcpwm_init\n");
#endif
#if PWM_INIT
    pwm_motor_output_init.h_pin = IO_PORTB_00;
    pwm_motor_output_init.h_pin_output_ch_num = 1;
    pwm_motor_output_init.pwm_timer_num = pwm_timer1;          // 时间基准, PWM定时器1
    pwm_motor_output_init.complementary_en = 1;                // 两个IO输出波形同步
    pwm_motor_output_init.frequency = 10000;                   // 10KHz
    pwm_motor_output_init.l_pin = -1;                          // 不初始化 PWM output low
    pwm_motor_output_init.duty = 2000;                         // 占空比 0.01%
    pwm_motor_output_init.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    pwm_motor_output_init.pwm_ch_num = pwm_ch1;                // channel_1

    printf("---------- >>> will init\n");
    mcpwm_init(&pwm_motor_output_init);
#endif

#endif
    log_pwm_info(pwm_ch0, pwm_timer0);
    printf("---------- >>>>>>>>>> %s end <<<<<<<<<< ----------", __func__);
}

static inline void *my_timer_task(void *p_arg)
{
    extern u8 usb_connect_timeout_flag;         // from task_pc.c, 手柄平台判断
    extern unsigned char get_report_f7_FLAG;    // from hid.c, PS3报表发送判断
    int ret = 0;
    if ((!timer_send_flag) && get_report_f7_FLAG && usb_connect_timeout_flag == 2)    // timer_send_flag: 当VM在读写的时候使定时器不发送任务队列
        ret = os_taskq_post_type(MY_TASK_NAME, MAIN_TCC_TASK, 0, NULL);
    else if (usb_connect_timeout_flag == 1 && (!timer_send_flag))
        ret = os_taskq_post_type(MY_TASK_NAME, MAIN_TCC_TASK, 0, NULL);

    if (ret != OS_NO_ERR)   // 0~65, from "os_error.h"
        log_print(__LOG_ERROR, NULL, "FAIL ! ! !    MAIN_TCC_TASK return value : %d\n", ret);   // 搞掂要注釋任務隊列溢出的打印, 防止MCU重啓

    tcc_count++;
    return &ret;
}
static inline void *led_timer_task(void *p_arg)
{
    extern u8 usb_connect_timeout_flag;         // from task_pc.c, 手柄平台判断
    extern unsigned char get_report_f7_FLAG;    // from hid.c, PS3报表发送判断
    int ret = 0;
    if ((!timer_send_flag) && get_report_f7_FLAG && usb_connect_timeout_flag == 2)    // timer_send_flag: 当VM在读写的时候使定时器不发送任务队列
        ret = os_taskq_post_type(MY_TASK_NAME, BREATHE_LED_TASK, 0, NULL);
    else if (usb_connect_timeout_flag == 1 && (!timer_send_flag))
        ret = os_taskq_post_type(MY_TASK_NAME, BREATHE_LED_TASK, 0, NULL);

    if (ret != OS_NO_ERR)   // 0~65, from "os_error.h"
        log_print(__LOG_ERROR, NULL, "FAIL ! ! !    BREATHE_LED_TASK return value : %d\n", ret);

    tcc_conut_led++;
    return &ret;
}
static inline void *SpecialFunc_timer_task(void *p_arg)
{
    int ret = 0;    // 避免未發送隊列導致打印fail
    if (change_host)    // USB主機平臺更換標記
    {
        ret = os_taskq_post_type(MY_TASK_NAME, SPECIAL_FUNCTIONS, 0, NULL);
    }
    if (ret != OS_NO_ERR)
        log_print(__LOG_ERROR, NULL, "FAIL ! ! !    SPECIAL_FUNCTIONS return value : %d\n", ret);

    tcc_connt_SpecialFunc++;
    return &ret;
}

void my_task_init(void)
{
    printf("__________ %s __________, %x\n", __func__, JL_USB->CON0);
    int err;
    // gpio_direction_output(IO_PORTA_03, 1);//set this IO output

    my_button_init();
    my_PWM_output_init();
    gpio_direction_output(IO_PORTA_03, 0);
    gpio_direction_output(IO_PORTA_01, 0);
    /* PC */
    data_send_to_host[1] = 0x14;    // 长度是固定的
    buf_point = records_keys_point; // 指向要被写入VM的数据

    /* PS3 */
    ps3_data_send_to_host[0] = 0x01;
    ps3_data_send_to_host[29] = 0x03;
    ps3_data_send_to_host[31] = 0x14;
    ps3_data_send_to_host[32] = 0x00;
    ps3_data_send_to_host[33] = 0x00;
    ps3_data_send_to_host[34] = 0x00;
    ps3_data_send_to_host[35] = 0x00;
    ps3_data_send_to_host[36] = 0x23;
    ps3_data_send_to_host[37] = 0x6d;
    ps3_data_send_to_host[38] = 0x77;
    ps3_data_send_to_host[39] = 0x01;
    ps3_data_send_to_host[40] = 0x80;
    ps3_data_send_to_host[45] = 0x01;
    ps3_data_send_to_host[47] = 0x02;
    ps3_data_send_to_host[48] = 0x00;


#if SUCCESSIVE_PRESS
    float temp = frequency / 2.0;
    frequency = temp;
#endif

#if VM_TEST

    for(int i = 0; i < NUM_FOR_VM_LEN; i++) // clear read VM buffer
        *((unsigned int*)buf_point + i) = 0;

    //int ret_w = syscfg_write(1, buf_point, NUM_FOR_VM_LEN * 4);
    //printf("VM write ret: %d", ret_w);


    // 除非重新烧写, 否则即使掉电数据仍然存储在VM区域
    int ret_r = syscfg_read(1, buf_point, (NUM_FOR_VM_LEN + LEN_RECORD_VM) * 4);
    printf(" VM READ RET: %d", ret_r);
    if (ret_r < 0)
        printf(" NO RECORD ! ! !");
    records_length_temp = *(records_keys_point + MAX_RECORD_ARRAY_LEN); // 将从VM读取的数据, 保存当前按键编程记录的长度
    for (int i = 0; i < records_length_temp; i++)                       // 将时间记录赋值
        reappear_times_temp[i] = (*((unsigned short*)records_keys_point + 1 + (SHORT_SIZE_NEXT * i)));
#endif

#if RECORD_MOVEMENT
#if MY_LIST
    /* 初始化头节点, init node head */
    key_info_head.keys_data[0] = 0;
    key_info_head.keys_data[1] = 0;
    key_info_head.record_times = 0;
    my_node_init(key_info_head);
#endif
    unsigned int ep_temp = JL_USB->EP0_ADR;
    printf("[INFO] JL_USB->EP0_ADR: %X", ep_temp);


#endif

#if THREAD_CREATE
    /* create my task, remember do not set stack-size(stksize) too short. if send task queue ,do not set the queue-size(qsize) is zero */
    err = os_task_create(my_task, NULL, 1, 512, 32, MY_TASK_NAME);
    log_print(__LOG_INFO, NULL, "create my task ! ! !    ret = %d\n", err);

#if MAIN_TIMER
    /* create user timer */
    ret_id_timer = sys_hi_timer_add(NULL, my_timer_task, MAIN_TCC_TIMER);
    log_print(__LOG_INFO, NULL, "Main timer task ID : %d\n", ret_id_timer);
#endif

#if PWM_TIEMR
    /*同一定时器中最好不要连续发送两次任务消息, 发送任务消息的时间要错开, 否则会因为发送了任务消息, 而任务收到后尚未处理完成, 又收到任务导致队列溢出*/
    /*定时器发送事件时间内未完成任务会导致任务队列溢出*/
    /* Failure to complete a task within the timer send event time causes the task queue to overflow. */
    ret_id_timer_led = sys_hi_timer_add(NULL, led_timer_task, LED_TCC_TIMER);
    log_print(__LOG_INFO, NULL, "LED timer task ID : %d\n", ret_id_timer_led);
#endif

#if SPECIAL_FUNC_TIMER
    ret_id_timer_SpecialFunctions = sys_hi_timer_add(NULL, SpecialFunc_timer_task, SPECIAL_FUNC_TCC);   // 最好唔好少於500ms, 低于该值判断会不稳定, MCU容易触发RESET, 400ms以下无法判断
    log_print(__LOG_INFO, NULL, "Special_Functions timer task ID : %d\n", ret_id_timer_SpecialFunctions);
#endif
#endif
    /* printf pwm model info */
    // log_pwm_led_info();

#if FUNC_TIMESTAMP
    for (int i = 0; i < 10; i++)
    {
        if (i >= START_FUNC && i < (END_FUNC + 1))
            count_all_func[i] = RESET_VAL;
        else
            count_all_func[i] = 0;
    }
#endif
}

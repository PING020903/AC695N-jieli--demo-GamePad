#ifndef _MY_PUT_TEST_
#define _MY_PUT_TEST_

#include "os/os_api.h"
#include "typedef.h"
#include "os/os_error.h"
#include "asm/gpio.h"
#include "asm/pwm_led.h"
#include "generic/log.h"
#include "system/debug.h"
#include "system/timer.h"
#include "printf.h"
#include "usb/usb_phy.h"
#include "usb/device/usb_stack.h"
//#include "lbuf.h"



enum {
    MAIN_TCC_TASK       = (Q_USER + 1),
    BREATHE_LED_TASK,
    SPECIAL_FUNCTIONS,
};



void my_read_key(void);             // 读取IO按键按键的状态, 由于是要上拉, 故此读取后需取反
void read_trigger_value(void);      // 读取扳机的AD输入值, 该值由vbat对比而来
void left_read_rocker(void);        // 读取左摇杆的AD输入值
void right_read_rocker(void);       // 读取右摇杆的AD输入值

void my_led_function(void);         // 实际上是PWM输出函数, 试验完毕后懒得改名字, 用于处理motor的震动

void connect_flicker(void);         // 连接成功后player指示灯熄灭亮起4次

void records_movement(void);        // 记录按键行动

void my_button_init(void);          // IO初始化
void my_PWM_output_init(void);      // PWM输出初始化

/* void* my_timer_task(void* p_arg);   // 主要任务定时器
void* led_timer_task(void* p_arg);  // PWM输出任务定时器 */
// 由于两个定时器都是毫秒级, 现改为static inline修饰

void* my_task(void* p_arg);         // 主线程
void my_task_init(void);            // 线程初始化



#endif // _MY_PUT_TEST_

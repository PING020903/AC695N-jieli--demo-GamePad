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
    MAIN_TCC_TASK = (Q_USER + 1),
    BREATHE_LED_TASK= (Q_USER + 2),
    CHECK_KEY_TASK= (Q_USER + 3),
};

void my_send_ADkeyX_event(void);
void my_send_ADkeyY_event(void);
void my_send_IOkey_event(void);


void* my_timeout_task(void* p_arg);
void* my_task(void* p_arg);
void my_read_key(void);
void my_led_function(void);
void my_task_init(void);

/*merge io key*/
unsigned char abc(unsigned char all_key, unsigned char key, unsigned int bit);

void* input_task(void* p_arg);
void input_task_init(void);


#endif // _MY_PUT_TEST_

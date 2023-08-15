
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

#include "generic/gpio.h"
#include "generic/log.h"
#include "asm/mcpwm.h"

/**************define-switch, 宏开关**************/
#define READ_KEY            1
#define MERGE_KEY           1
#define MOVEMENTS_SEND      1

#define TRIGGER             1
#define LEFT_TRIGGER        1
#define RIGHT_TRIGGER       1
#define PWM_MOTOR           1
#define LEFT_MOTOR          1
#define RIGHT_MOTOR         1

#define LEFT_ROCKER         1
#define LEFT_ROCKER_BUFFER  1
#define LEFT_ROCKER_X_AXIS  1
#define LEFT_ROCKER_Y_AXIS  1

#define RIGHT_ROCKER        1
#define RIGHT_ROCKER_BUFFER 1
#define RIGHT_ROCKER_X_AXIS 1
#define RIGHT_ROCKER_Y_AXIS 1
/***********************************************/




#define MY_TASK_NAME "thursday"
#define BRIGHT              (500)
#define MAIN_TCC_TIMER      (4)
#define LED_TCC_TIMER       (8)

#define DEADBAND_X1         (490)       // internal deadband, minus side of the axis
#define DEADBAND_X2         (575)       // internal deadband, plus side of the axis
#define TRIGGER_INIT_VAL    (30)        // trigger initial value
#define TRIGGER_PRESS_VAL   (45)        // more than the value, so press the trigger


extern usb_dev usbfd;                       //form task_pc.c
extern unsigned char trigger[2];            //form uac1.c
extern unsigned char player_led;            //from uac1.c

extern void my_pwm_led_on_display(u8 led_index, u16 led0_bright, u16 led1_bright);
extern void mcpwm_init(struct pwm_platform_data *arg);
extern void mcpwm_set_duty(pwm_ch_num_type pwm_ch, pwm_timer_num_type timer_ch, u16 duty);
extern void log_pwm_info(pwm_ch_num_type pwm_ch, pwm_timer_num_type timer_ch);

volatile unsigned char the_io_val = 0;      //IO status
volatile unsigned int tcc_count = 0;
static int ret_id_timer;                    //timer ID
static int ret_id_timer_led;
static unsigned char data_send_to_host[20] = { 0x00 };  /* using the variant have to set zero , in the after assign the value */
static unsigned char data_send_to_host_temp[20] = { 0x00 };

static unsigned char count_all_func = 0;        // function start to end time
static unsigned char count_all_func_1 = 0;
static unsigned char count_all_func_2 = 0;
static unsigned char count_all_func_3 = 0;

#if READ_KEY
static volatile unsigned char my_key_val_1 = 0;         // IO_PORTA_08
static volatile unsigned char my_key_val_2 = 0;         // IO_PORTA_11
static volatile unsigned char my_key_val_3 = 0;         // IO_PORTA_12
static volatile unsigned char my_key_val_4 = 0;         // IO_PORTA_14

static volatile unsigned char a_key_val = 0;            // IO_PORTB_11
static volatile unsigned char b_key_val = 0;            // IO_PORTB_06
static volatile unsigned char x_key_val = 0;            // IO_PORTB_09
static volatile unsigned char y_key_val = 0;            // IO_PORTB_04

static volatile unsigned char R_rocker_io_key = 0;      // IO_PORTB_02
static volatile unsigned char L_rocker_io_key = 0;      // IO_PORTA_07

static volatile unsigned char L_1_io_key = 0;           // IO_PORTB_05
static volatile unsigned char R_1_io_key = 0;           // IO_PORTB_07

static volatile unsigned char start_io_key = 0;         // IO_PORTC_00
static volatile unsigned char back_io_key = 0;          // IO_PORTC_07
static volatile unsigned char XboxHome_io_key = 0;      // IO_PORTC_01
#endif

#if LEFT_ROCKER
static volatile int my_rocker_ad_key_x = 0;             // ADC1, IO_PORTA_05
static volatile int my_rocker_ad_key_y = 0;             // ADC2, IO_PORTA_06
#endif

#if RIGHT_ROCKER
static volatile int R_rocker_ad_key_x = 0;              // ADC3, IO_PORTB_01
static volatile int R_rocker_ad_key_y = 0;              // ADC5, IO_PORTB_03
#endif

#if TRIGGER
#if LEFT_TRIGGER
static volatile int L_trigger_ad_key = 0;               // ADC8, IO_PORTB_08
static volatile int L_trigger_temp = 0;
#endif  /* left trigger */

#if RIGHT_TRIGGER
static volatile int R_trigger_ad_key = 0;               // ADC9, IO_PORTB_10
static volatile int R_trigger_temp = 0;
#endif  /* right trigger */
#endif  /* trigger */

#if PWM_MOTOR
#if LEFT_MOTOR
static int io_count = 0;
#endif  /* left motor */

#if RIGHT_MOTOR
static int io_count_R = 0;
#endif  /* right motor */
#endif  /* PWM motor */

static volatile unsigned char io_key_status;            // merged io key
static unsigned char motor_flag = 1;                    // 清除初始化的占空比值 Clear the initialised duty cycle value
static unsigned char player_IO_status = 1;              // 玩家led指示灯状态
static volatile unsigned char player_flicker_time = 0;  // 玩家led指示灯闪烁时间


#if 0
volatile struct sys_event my_key_event;             //will send the key_event
void my_send_ADkeyX_event(void)
{
    my_key_event.type           = 0x0001;             //key_event 0x0001,
    my_key_event.u.key.event    = 1;
    my_key_event.u.key.type     = 0;
    if( (DEADBAND_P1 >= my_rocker_ad_key_x) || (DEADBAND_P2 <= my_rocker_ad_key_x) )
    {
        my_key_event.u.key.type     = 1;
        my_key_event.u.key.value    = my_rocker_ad_key_x;
        sys_event_notify(&my_key_event);
    }
}
void my_send_ADkeyY_event(void)
{
    my_key_event.type           = 0x0001;             //key_event 0x0001,
    my_key_event.u.key.event    = 1;
    my_key_event.u.key.type     = 0;
    if( (DEADBAND_P1 >= my_rocker_ad_key_y) || (DEADBAND_P2 <= my_rocker_ad_key_y) )
    {
        my_key_event.u.key.type = 2;
        my_key_event.u.key.value = my_rocker_ad_key_y;
        sys_event_notify(&my_key_event);
    }
}
void my_send_IOkey_event(void)
{
    my_key_event.type           = 0x0001;             //key_event 0x0001,
    my_key_event.u.key.event    = 1;
    my_key_event.u.key.type     = 3;
    my_key_event.u.key.value    = (unsigned int)all_io_key;
    sys_event_notify(&my_key_event);
}
#endif //disable send system event


static inline unsigned int xbox360_tx_data(const usb_dev usb_id, const u8 *buffer, unsigned int len)
{
    return usb_g_intr_write(usb_id, 0x01, buffer, len);/* 0x01 is IN endpoint */
}

/* merge io key , 'key' value is one or zero only*/
static inline unsigned char merge_value(unsigned char all_key, unsigned char key, unsigned int bit)
{
    if(key != 1 && key != 0)
        return 0;
    if(key == 1)
        return all_key |= (1 << bit);
    if(key == 0)
        return all_key &= ~(1 << bit);
}

void my_read_key(void)
{

    while (count_all_func){
        printf("\n------------------------------- start %s -------------------------------\n", __func__);
        count_all_func = 0;};
    if((tcc_count % (750 / MAIN_TCC_TIMER) ) == 0)
        printf("---------- %s ----------\n", __func__);

#if READ_KEY
    L_rocker_io_key = ( gpio_read(IO_PORTA_07) ) ^ 0x01;
    R_rocker_io_key = ( gpio_read(IO_PORTB_02) ) ^ 0x01;

    L_1_io_key = ( gpio_read(IO_PORTB_05) ) ^ 0x01;
    R_1_io_key = ( gpio_read(IO_PORTB_07) ) ^ 0x01;

    my_key_val_1 = ( gpio_read(IO_PORTA_08) ) ^ 0x01;
    my_key_val_2 = ( gpio_read(IO_PORTA_11) ) ^ 0x01;
    my_key_val_3 = ( gpio_read(IO_PORTA_12) ) ^ 0x01;
    my_key_val_4 = ( gpio_read(IO_PORTA_14) ) ^ 0x01;

    a_key_val = ( gpio_read(IO_PORTB_11) ) ^ 0x01;
    b_key_val = ( gpio_read(IO_PORTB_06) ) ^ 0x01;
    x_key_val = ( gpio_read(IO_PORTB_09) ) ^ 0x01;
    y_key_val = ( gpio_read(IO_PORTB_04) ) ^ 0x01;

    start_io_key = ( gpio_read(IO_PORTC_00) ) ^ 0x01;
    back_io_key = ( gpio_read(IO_PORTC_07) ) ^ 0x01;
    XboxHome_io_key = ( gpio_read(IO_PORTC_01) ) ^ 0x01;
#endif

#if MERGE_KEY   /* IO key */
        io_key_status = 0x00;   /* using the variant have to set zero , in the after assign the value*/
        io_key_status = merge_value(io_key_status, my_key_val_3, 0);                            // ↑up
        io_key_status = merge_value(io_key_status, my_key_val_2, 1);                            // ↓dowm
        io_key_status = merge_value(io_key_status, my_key_val_1, 2);                            // ←left
        io_key_status = merge_value(io_key_status, my_key_val_4, 3);                            // →right
        io_key_status = merge_value(io_key_status, start_io_key, 4);                            // start
        io_key_status = merge_value(io_key_status, back_io_key, 5);                             // back
        io_key_status = merge_value(io_key_status, L_rocker_io_key, 6);                         //  left_3
        io_key_status = merge_value(io_key_status, R_rocker_io_key, 7);                         // right_3
        data_send_to_host[2] = io_key_status;


        io_key_status = 0x00;      /* using the variant have to set zero , in the after assign the value*/
        io_key_status = merge_value(io_key_status, L_1_io_key, 0);                              //  left_1
        io_key_status = merge_value(io_key_status, R_1_io_key, 1);                              // right_1
        io_key_status = merge_value(io_key_status, XboxHome_io_key, 2);                         // xbox home
        io_key_status = merge_value(io_key_status, a_key_val, 4);                               // A
        io_key_status = merge_value(io_key_status, b_key_val, 5);                               // B
        io_key_status = merge_value(io_key_status, x_key_val, 6);                               // X
        io_key_status = merge_value(io_key_status, y_key_val, 7);                               // Y
        data_send_to_host[3] = io_key_status;

#endif  /* IO key */
    while(count_all_func_1){
        printf("\n------------------------------- end %s -------------------------------\n", __func__);
        count_all_func_1 = 0;};
}

void read_trigger_value(void)
{
    L_trigger_ad_key = adc_get_value(8);
    R_trigger_ad_key = adc_get_value(9);

#if TRIGGER   /* trigger */
#if LEFT_TRIGGER   /* left */
    if(L_trigger_ad_key > TRIGGER_PRESS_VAL)
    {
        int send_condition = L_trigger_temp - (L_trigger_ad_key - TRIGGER_INIT_VAL);// jitter judgement
        if( send_condition > 4 || send_condition < (-4)){                         // eliminate jitter
            data_send_to_host[4] = (L_trigger_ad_key - TRIGGER_INIT_VAL);       // left trigger, L2
            L_trigger_temp = data_send_to_host[4];
        }

        if( (L_trigger_ad_key - TRIGGER_INIT_VAL) > 0xff ){
            data_send_to_host[4] = 0xff;
            L_trigger_temp = 0xff;
        }
    }
    else{
        data_send_to_host[4] = 0x00;
        L_trigger_temp = 0x00;
    }

#endif
#if RIGHT_TRIGGER   /* right */
    if(R_trigger_ad_key > TRIGGER_PRESS_VAL)
    {
        int send_condition = R_trigger_temp - (R_trigger_ad_key - TRIGGER_INIT_VAL);// jitter judgement
        if( send_condition > 5 || send_condition < (-5)){                         // eliminate jitter
            data_send_to_host[5] = (R_trigger_ad_key - TRIGGER_INIT_VAL);       // right trigger, R2
            R_trigger_temp = data_send_to_host[5];
        }

        if( (R_trigger_ad_key - TRIGGER_INIT_VAL) > 0xff ){
            data_send_to_host[5] = 0xff;
            R_trigger_temp = 0xff;
        }
    }
    else{
        data_send_to_host[5] = 0x00;
        R_trigger_temp = 0x00;
    }

#endif
#endif  /* trigger */
}

void left_read_rocker(void)
{
    /* Left */
    unsigned char L_X_plus = 0;      //X+
    unsigned char L_X_minus = 0;     //X-
    unsigned char L_Y_plus = 0;      //Y+
    unsigned char L_Y_minus = 0;     //Y-

    /* parametric is adc CHANNEL */
    my_rocker_ad_key_x = adc_get_value(1);  //ADC1
    my_rocker_ad_key_y = adc_get_value(2);  //ADC2

#if LEFT_ROCKER   /* left rocker */
    if( (DEADBAND_X2 <= my_rocker_ad_key_x) || (DEADBAND_X1 >= my_rocker_ad_key_x) )
    {
        if(DEADBAND_X2 <= my_rocker_ad_key_x)   // X+
        {
            float temp_X_plus_f = (my_rocker_ad_key_x - DEADBAND_X2) / 3.48;     // external deadband, right side of the axis
            L_X_plus = (unsigned char)temp_X_plus_f;
            L_X_minus = 0;
        }
        if(DEADBAND_X1 >= my_rocker_ad_key_x)   // X-
        {
            float temp_X_minus_f = (DEADBAND_X1 - my_rocker_ad_key_x) / 3.7;    // external deadband, left side of the axis
            L_X_minus = 0xff - (unsigned char)temp_X_minus_f;
            L_X_plus = 0;
        }
    }
    if( (DEADBAND_X2 <= my_rocker_ad_key_y) || (DEADBAND_X1 >= my_rocker_ad_key_y) )
    {
        if(DEADBAND_X2 <= my_rocker_ad_key_y)   // Y+
        {
            float temp_Y_plus_f = (my_rocker_ad_key_y - DEADBAND_X2) / 3.2;     // external deadband, up side of the axis
            L_Y_plus = (unsigned char)temp_Y_plus_f;
            L_Y_minus = 0;
        }
        if(DEADBAND_X1 >= my_rocker_ad_key_y)   // Y-
        {
            float temp_Y_minus_f = (DEADBAND_X1 - my_rocker_ad_key_y) / 3.8;    // external deadband, down side of the axis
            L_Y_minus = 0xff - (unsigned char)temp_Y_minus_f;
            L_Y_plus = 0;
        }
    }
#endif  /* left rocker */

#if LEFT_ROCKER_BUFFER   /* left rocker value buffer */
#if LEFT_ROCKER_X_AXIS   /* X axis */
        if(L_X_plus != 0 || L_X_minus != 0)
        {
            if(L_X_plus >= 127 && L_X_minus == 0)      // X+
            {
                data_send_to_host[6] = 0xff;    //left rocker
                data_send_to_host[7] = 0x7f;
            }
            if(L_X_plus < 127 && L_X_minus == 0)
            {
                data_send_to_host[6] = 0x00;    //left rocker
                data_send_to_host[7] = L_X_plus;
            }

            if(L_X_plus == 0 && L_X_minus <= 0x80)    // X-
            {
                data_send_to_host[6] = 0x00;    //left rocker
                data_send_to_host[7] = 0x80;;
            }
            if(L_X_plus == 0 && L_X_minus != 0 && L_X_minus > 0x80)
            {
                data_send_to_host[6] = 0x00;    //left rocker
                data_send_to_host[7] = L_X_minus;
            }
        }
#endif

        if( (L_X_minus == 0 && L_X_plus == 0) && (L_Y_minus == 0 && L_Y_plus == 0) )       // median value
        {
            data_send_to_host[6] = 0x00;        //left rocker X
                data_send_to_host[7] = 0x00;
            data_send_to_host[8] = 0x00;        //left rocker Y
                data_send_to_host[9] = 0Xff;
        }

#if LEFT_ROCKER_Y_AXIS   /* Y axis */
        if(L_Y_plus != 0 || L_Y_minus != 0)
        {
            if(L_Y_plus >= 127 && L_Y_minus == 0)
            {
                data_send_to_host[8] = 0xff;    //left rocker
                data_send_to_host[9] = 0x7f;
            }
            if(L_Y_plus < 127 && L_Y_minus == 0)
            {
                data_send_to_host[8] = 0x00;    //left rocker
                data_send_to_host[9] = L_Y_plus;
            }

            if(L_Y_plus == 0 && L_Y_minus <= 0x80)
            {
                data_send_to_host[8] = 0x00;    //left rocker
                data_send_to_host[9] = 0x80;
            }
            if(L_Y_plus == 0 && L_Y_minus != 0 && L_Y_minus > 0x80)
            {
                data_send_to_host[8] = 0x00;    //left rocker
                data_send_to_host[9] = L_Y_minus;
            }
        }
#endif
#endif  /* left rocker value buffer */
}

void right_read_rocker(void)
{
    /* Right */
    unsigned char R_X_plus = 0;      //X+
    unsigned char R_X_minus = 0;     //X-
    unsigned char R_Y_plus = 0;      //Y+
    unsigned char R_Y_minus = 0;     //Y-

    R_rocker_ad_key_x = adc_get_value(5);   //ADC5
    R_rocker_ad_key_y = adc_get_value(6);   //ADC6


#if RIGHT_ROCKER
   /* right rocker */
    if( (DEADBAND_X2 <= R_rocker_ad_key_x) || (DEADBAND_X1 >= R_rocker_ad_key_x) )
    {
        if(DEADBAND_X2 <= R_rocker_ad_key_x)   // X+
        {
            float temp_X_plus_f = (R_rocker_ad_key_x - DEADBAND_X2) / 3.45;     // external deadband, right side of the axis
            R_X_plus = (unsigned char)temp_X_plus_f;
            R_X_minus = 0;
        }
        if(DEADBAND_X1 >= R_rocker_ad_key_x)   // X-
        {
            float temp_X_minus_f = (DEADBAND_X1 - R_rocker_ad_key_x) / 3.7;    // external deadband, left side of the axis
            R_X_minus = 0xff - (unsigned char)temp_X_minus_f;
            R_X_plus = 0;
        }
    }
    if( (DEADBAND_X2 <= R_rocker_ad_key_y) || (DEADBAND_X1 >= R_rocker_ad_key_y) )
    {
        if(DEADBAND_X2 <= R_rocker_ad_key_y)   // Y+
        {
            float temp_Y_plus_f = (R_rocker_ad_key_y - DEADBAND_X2) / 3.2;     // external deadband, up side of the axis
            R_Y_plus = (unsigned char)temp_Y_plus_f;
            R_Y_minus = 0;
        }
        if(DEADBAND_X1 >= R_rocker_ad_key_y)   // Y-
        {
            float temp_Y_minus_f = (DEADBAND_X1 - R_rocker_ad_key_y) / 3.76;    // external deadband, down side of the axis
            R_Y_minus = 0xff - (unsigned char)temp_Y_minus_f;
            R_Y_plus = 0;
        }
    }
#endif  /* right rocker */

#if RIGHT_ROCKER_BUFFER   /* right rocker value buffer */
#if RIGHT_ROCKER_X_AXIS   /* X axis */
        if(R_X_plus != 0 || R_X_minus != 0)
        {
            if(R_X_plus >= 127 && R_X_minus == 0)      // X+
            {
                data_send_to_host[10] = 0xff;   //right rocker
                data_send_to_host[11] = 0x7f;
            }
            if(R_X_plus < 127 && R_X_minus == 0)
            {
                data_send_to_host[10] = 0x00;   //right rocker
                data_send_to_host[11] = R_X_plus;
            }

            if(R_X_plus == 0 && R_X_minus <= 0x80)    // X-
            {
                data_send_to_host[10] = 0x00;   //right rocker
                data_send_to_host[11] = 0x80;
            }
            if(R_X_plus == 0 && R_X_minus != 0 && R_X_minus > 0x80)
            {
                data_send_to_host[10] = 0x00;   //right rocker
                data_send_to_host[11] = R_X_minus;
            }
        }
#endif

        if( (R_X_minus == 0 && R_X_plus == 0) && (R_X_minus == 0 && R_Y_plus == 0) )       // median value
        {
            data_send_to_host[10] = 0x00;        //right rocker X
                data_send_to_host[11] = 0x00;
            data_send_to_host[12] = 0x00;        //right rocker Y
                data_send_to_host[13] = 0Xff;
        }

#if RIGHT_ROCKER_Y_AXIS   /* Y axis */
        if(R_Y_plus != 0 || R_Y_minus != 0)
        {
            if(R_Y_plus >= 127 && R_Y_minus == 0)
            {
                data_send_to_host[12] = 0xff;   //right rocker
                data_send_to_host[13] = 0x7f;
            }
            if(R_Y_plus < 127 && R_Y_minus == 0)
            {
                data_send_to_host[12] = 0x00;   //right rocker
                data_send_to_host[13] = R_Y_plus;
            }

            if(R_Y_plus == 0 && R_Y_minus <= 0x80)
            {
                data_send_to_host[12] = 0x00;   //right rocker
                data_send_to_host[13] = 0x80;
            }
            if(R_Y_plus == 0 && R_Y_minus != 0 && R_Y_minus > 0x80)
            {
                data_send_to_host[12] = 0x00;   //right rocker
                data_send_to_host[13] = R_Y_minus;
            }
        }
#endif
#endif  /* right rocker value buffer */
}

void my_led_function(void)
{
    while (count_all_func_2){
        printf("\n------------------------------- start %s -------------------------------\n", __func__);
        count_all_func_2 = 0;};
    tcc_count++;

    // 清除初始化的占空比值 Clear the initialised duty cycle value
    while(motor_flag)
    {
        unsigned int register_temp = JL_USB->CON0;
        register_temp = ((register_temp << 18) >> 31);      //13th bit, SOF_PND
        if(register_temp)
        {
            mcpwm_set_duty(pwm_ch0, pwm_timer0, 0);
            mcpwm_set_duty(pwm_ch1, pwm_timer1, 0);
            motor_flag = 0;
        }
    }


#if PWM_MOTOR    /* PWM Motor */
#if LEFT_MOTOR   /* left motor */
    if(data_send_to_host[4] == trigger[0])
    {
        if(trigger[0] != 0)
            io_count = trigger[0];      // receive OUT packet form Xbox360input.exe
        else
            io_count = 0;

        float temp = io_count * 39.22;
        io_count = temp;
        if(io_count >= 10000)
            io_count = 10000;
        mcpwm_set_duty(pwm_ch0, pwm_timer0, io_count);

        //my_pwm_led_on_display(1, 0, io_count);
    }
#endif  /* left motor */

#if RIGHT_MOTOR   /* right motor */
    if(data_send_to_host[5] == trigger[1])
    {
        if(trigger[1] != 0)
            io_count_R = trigger[1];      // receive OUT packet form Xbox360input.exe
        else
            io_count_R = 0;

        float temp = io_count_R * 39.22;
        io_count_R = temp;
        if(io_count_R >= 10000)
            io_count_R = 10000;
        //my_pwm_led_on_display(1, 0, io_count);
        mcpwm_set_duty(pwm_ch1, pwm_timer1, io_count_R);
    }
#endif  /* right motor */
#endif

    if( (tcc_count % (750 / MAIN_TCC_TIMER) ) == 0 )
    {
        printf("USB : %X \n", JL_USB->CON0);            // get USB_CON0 register
        /* the_io_val ^= 1;
        gpio_direction_output(IO_PORTA_03, the_io_val );//invert the state */
        printf("---------- %s ----------\n", __func__);
        
        //printf("watch dog stutas : %x\n", p33_rx_1byte(P3_WDT_CON));  // use this function read watch dog status from this address
        if( tcc_count == 12000 )
        {
            /* Cannot be re-entered for a short time */
            //pwm_led_breathe_display(1, 500, BRIGHT, BRIGHT, 0, 100, 0);
            
            tcc_count = 0;
        }
    }
    while(count_all_func_3){
        printf("\n------------------------------- end %s -------------------------------\n", __func__);
        count_all_func_3 = 0;} ;
}

static inline void send_data_to_host(void)
{
#if MOVEMENTS_SEND
    unsigned char send_flag = 0;
#endif
    unsigned int register_temp = JL_USB->CON0;
    register_temp = ((register_temp << 18) >> 31);      //13th bit, SOF_PND
#if MOVEMENTS_SEND
    for(int i = 0; i < 20; i++)
    {
        if(data_send_to_host_temp[i] == data_send_to_host[i])
            send_flag++;
        if(data_send_to_host_temp[i] != data_send_to_host[i])
            send_flag--;
        if(send_flag == 20)
            return;
    }
    if(send_flag < 20)
    {
#endif
        if(register_temp)
            xbox360_tx_data(usbfd, data_send_to_host, 20);
#if MOVEMENTS_SEND
        for(int i = 0; i < 20; i++)
            data_send_to_host_temp[i] = data_send_to_host[i];
    }
#endif
}

void connect_flicker(void)
{
    if(player_flicker_time >= 8)
        return;
    switch (player_led)
    {
    case 1:{    /* player 1 */
        if( (tcc_count % (240 / MAIN_TCC_TIMER) ) == 0 ){
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_03, (int)player_IO_status );
            player_flicker_time++;
        }
    }break;

    case 2:{
        if( (tcc_count % (240 / MAIN_TCC_TIMER) ) == 0 ){
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_02, (int)player_IO_status );
            player_flicker_time++;
        }
    }break;
    case 3:{
        if( (tcc_count % (240 / MAIN_TCC_TIMER) ) == 0 ){
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_01, (int)player_IO_status );
            player_flicker_time++;
        }
    }break;
    case 4:{
        if( (tcc_count % (240 / MAIN_TCC_TIMER) ) == 0 ){
            player_IO_status ^= 1;
            gpio_direction_output(IO_PORTA_04, (int)player_IO_status );
            player_flicker_time++;
        }
    }break;
    default:
        break;
    }
}

void* my_task(void* p_arg)
{
    int msg[8];     // in the array, recive task queue
    while(1)
    {
        /* receive task queue */
        int ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        if (ret != OS_TASKQ)
            continue;

        switch(msg[0])
        {
            case MAIN_TCC_TASK:
            {
                my_read_key();
                left_read_rocker();
                right_read_rocker();
                read_trigger_value();
                send_data_to_host();
            }
            break;
            case BREATHE_LED_TASK:
            {
                my_led_function();
                connect_flicker();
            }
            break;

        default:
            break;
        }
    }
}

void my_button_init(void)
{
    /* Before using IOs, it is important to note whether any IOs have been and are occupied beforehand */
    printf("---------- %s ----------\n", __func__);

    /* set IO output */
    //gpio_set_direction(IO_PORTA_01, 0);         // ADC, PWM motor
    gpio_set_direction(IO_PORTB_00, 0);         // PWM, ch1, high
    gpio_set_direction(IO_PORTA_00, 0);         // PWM, ch0, high

    gpio_set_direction(IO_PORTA_02, 0);         /* LED1 */
    gpio_set_direction(IO_PORTA_04, 0);         /* LED3 */
    gpio_set_direction(IO_PORTA_01, 0);         /* LED0 */
    gpio_set_direction(IO_PORTA_03, 0);         /* LED2 */

    /*set IO input*/
    gpio_set_direction(IO_PORTA_05, 1);         // ADC, rocker L_X
    gpio_set_direction(IO_PORTA_06, 1);         // ADC, rocker L_Y
    gpio_set_direction(IO_PORTA_07, 1);         // IO,  rocker L_3

    gpio_set_direction(IO_PORTA_08, 1);         /* ↑ */
    gpio_set_direction(IO_PORTA_11, 1);         /* ↓ */
    gpio_set_direction(IO_PORTA_12, 1);         /* ← */
    gpio_set_direction(IO_PORTA_14, 1);         /* → */

    gpio_set_direction(IO_PORTB_09, 1);         /* A */
    gpio_set_direction(IO_PORTB_11, 1);         /* B */
    gpio_set_direction(IO_PORTB_13, 1);         /* X */
    gpio_set_direction(IO_PORTB_15, 1);         /* Y */

    gpio_set_direction(IO_PORTB_02, 1);         // IO,  rocker R_3
    gpio_set_direction(IO_PORTB_01, 1);         // ADC, rocker R_X
    gpio_set_direction(IO_PORTB_03, 1);         // ADC, rocker R_Y

    gpio_set_direction(IO_PORTB_05, 1);         // IO, L_1
    gpio_set_direction(IO_PORTB_07, 1);         // IO, R_1

    gpio_set_direction(IO_PORTB_08, 1);         // ADC, left trigger
    gpio_set_direction(IO_PORTB_10, 1);         // ADC, right trigger

    gpio_set_direction(IO_PORTC_00, 1);
    gpio_set_direction(IO_PORTC_01, 1);
    gpio_set_direction(IO_PORTC_03, 1);
    gpio_set_direction(IO_PORTC_05, 1);
    gpio_set_direction(IO_PORTC_07, 1);

    printf("set output or input\n");

/*************************************************************************************/

    adc_init();
    gpio_set_die(IO_PORTA_05, 0);           // analogue input
    gpio_set_die(IO_PORTA_06, 0);
    gpio_set_die(IO_PORTA_07, 1);           // digital input

    gpio_set_die(IO_PORTA_08, 1);           /* ↑ */
    gpio_set_die(IO_PORTA_11, 1);           /* ↓ */
    gpio_set_die(IO_PORTA_12, 1);           /* ← */
    gpio_set_die(IO_PORTA_14, 1);           /* → */

    gpio_set_die(IO_PORTB_09, 1);           /* X */
    gpio_set_die(IO_PORTB_11, 1);           /* A */
    gpio_set_die(IO_PORTB_04, 1);           /* B */
    gpio_set_die(IO_PORTB_06, 1);           /* Y */

    gpio_set_die(IO_PORTB_02, 1);           // IO,  rocker R_3
    gpio_set_die(IO_PORTB_01, 0);           // ADC, rocker R_X
    gpio_set_die(IO_PORTB_03, 0);           // ADC, rocker R_Y

    gpio_set_die(IO_PORTB_05, 1);           // IO, L_1
    gpio_set_die(IO_PORTB_07, 1);           // IO, R_1

    gpio_set_die(IO_PORTB_08, 0);           // ADC, left trigger
    gpio_set_die(IO_PORTB_10, 0);           // ADC, right trigger

    gpio_set_die(IO_PORTC_00, 1);
    gpio_set_die(IO_PORTC_01, 1);
    gpio_set_die(IO_PORTC_03, 1);
    gpio_set_die(IO_PORTC_05, 1);
    gpio_set_die(IO_PORTC_07, 1);

    printf("set output mode\n");
/*************************************************************************************/

    gpio_set_pull_down(IO_PORTA_05, 0);         // no pull down
        gpio_set_pull_down(IO_PORTA_06, 0);
            gpio_set_pull_down(IO_PORTA_07, 0);

    gpio_set_pull_down(IO_PORTB_01, 0);
        gpio_set_pull_down(IO_PORTB_03, 0);
            gpio_set_pull_down(IO_PORTB_02, 0);

    gpio_set_pull_down(IO_PORTB_08, 0);
        gpio_set_pull_down(IO_PORTB_05, 0);

    gpio_set_pull_down(IO_PORTB_10, 0);
        gpio_set_pull_down(IO_PORTB_07, 0);

    gpio_set_pull_down(IO_PORTA_08, 0);         /* ↑ */
    gpio_set_pull_down(IO_PORTA_11, 0);         /* ↓ */
    gpio_set_pull_down(IO_PORTA_12, 0);         /* ← */
    gpio_set_pull_down(IO_PORTA_14, 0);         /* → */

    gpio_set_pull_down(IO_PORTB_09, 0);         /* X */
    gpio_set_pull_down(IO_PORTB_11, 0);         /* A */
    gpio_set_pull_down(IO_PORTB_04, 0);         /* B */
    gpio_set_pull_down(IO_PORTB_06, 0);         /* Y */

    gpio_set_pull_down(IO_PORTA_02, 0);         /* LED1 */
    gpio_set_pull_down(IO_PORTA_04, 0);         /* LED3 */
    gpio_set_pull_down(IO_PORTA_01, 0);         /* LED0 */
    gpio_set_pull_down(IO_PORTA_03, 0);         /* LED2 */

    gpio_set_pull_down(IO_PORTC_00, 0);
    gpio_set_pull_down(IO_PORTC_01, 0);
    gpio_set_pull_down(IO_PORTC_03, 0);
    gpio_set_pull_down(IO_PORTC_05, 0);
    gpio_set_pull_down(IO_PORTC_07, 0);

    printf("set pull down or no pull down\n");
/*************************************************************************************/

    gpio_set_pull_up(IO_PORTA_05, 0);           // no pull up
        gpio_set_pull_up(IO_PORTA_06, 0);
            gpio_set_pull_up(IO_PORTA_07, 1);   // set pull up

    gpio_set_pull_up(IO_PORTB_01, 0);
        gpio_set_pull_up(IO_PORTB_03, 0);
            gpio_set_pull_up(IO_PORTB_02, 1);

    gpio_set_pull_up(IO_PORTB_08, 0);
        gpio_set_pull_up(IO_PORTB_05, 1);

    gpio_set_pull_up(IO_PORTB_10, 0);
        gpio_set_pull_up(IO_PORTB_07, 1);

    gpio_set_pull_up(IO_PORTA_08, 1);           /* ↑ */
    gpio_set_pull_up(IO_PORTA_11, 1);           /* ↓ */
    gpio_set_pull_up(IO_PORTA_12, 1);           /* ← */
    gpio_set_pull_up(IO_PORTA_14, 1);           /* → */

    gpio_set_pull_up(IO_PORTB_09, 1);           /* X */
    gpio_set_pull_up(IO_PORTB_11, 1);           /* A */
    gpio_set_pull_up(IO_PORTB_04, 1);           /* B */
    gpio_set_pull_up(IO_PORTB_06, 1);           /* Y */

    gpio_set_pull_up(IO_PORTA_02, 1);          /* LED1 */
    gpio_set_pull_up(IO_PORTA_04, 1);          /* LED3 */
    gpio_set_pull_up(IO_PORTA_01, 1);          /* LED0 */
    gpio_set_pull_up(IO_PORTA_03, 1);          /* LED2 */

    gpio_set_pull_up(IO_PORTC_00, 1);
    gpio_set_pull_up(IO_PORTC_01, 1);
    gpio_set_pull_up(IO_PORTC_03, 1);
    gpio_set_pull_up(IO_PORTC_05, 1);
    gpio_set_pull_up(IO_PORTC_07, 1);

    printf("set pull up or no pull up\n");
/*************************************************************************************/

    /* add sample adc CHANNEL */
    adc_add_sample_ch(1);// ADC1
    adc_add_sample_ch(2);// ADC2

    adc_add_sample_ch(5);// ADC5
    adc_add_sample_ch(6);// ADC6

    adc_add_sample_ch(8);// ADC8, left trigger
    adc_add_sample_ch(9);// ADC9, right triger

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
#if 1
    struct pwm_platform_data pwm_motor_output_init = {0X00};
#if 1
    pwm_motor_output_init.complementary_en = 1;                     // 两个IO输出波形同步
    pwm_motor_output_init.frequency = 10000;                        // 10KHz
    pwm_motor_output_init.pwm_timer_num = pwm_timer0;               // 时间基准, PWM定时器0
    pwm_motor_output_init.h_pin = IO_PORTA_00;                      // PWM output high IO, h_pin_output_ch_num 有效, 必须配置
    pwm_motor_output_init.h_pin_output_ch_num = 0;
    pwm_motor_output_init.l_pin = -1;                               // 不初始化 PWM output low, l_pin_output_ch_num 无效，可不配置
    pwm_motor_output_init.duty = 2000;                                 // 占空比 0.01%
    pwm_motor_output_init.pwm_aligned_mode = pwm_edge_aligned;      // 边沿对齐
    pwm_motor_output_init.pwm_ch_num = pwm_ch0;                     // channel_0
    printf("---------- >>> will init\n");
    mcpwm_init(&pwm_motor_output_init);                             // PS: channel >= 3, not support output_channel
    printf("---------- >>> mcpwm_init\n");
#endif
#if 1
    pwm_motor_output_init.h_pin = IO_PORTB_00;
    pwm_motor_output_init.h_pin_output_ch_num = 1;
    pwm_motor_output_init.pwm_timer_num = pwm_timer1;               // 时间基准, PWM定时器1
    pwm_motor_output_init.complementary_en = 1;                     // 两个IO输出波形同步
    pwm_motor_output_init.frequency = 10000;                        // 10KHz
    pwm_motor_output_init.l_pin = -1;                               // 不初始化 PWM output low
    pwm_motor_output_init.duty = 2000;                                 // 占空比 0.01%
    pwm_motor_output_init.pwm_aligned_mode = pwm_edge_aligned;      // 边沿对齐
    pwm_motor_output_init.pwm_ch_num = pwm_ch1;                     // channel_1

    printf("---------- >>> will init\n");
    mcpwm_init(&pwm_motor_output_init);
#endif

#endif
    log_pwm_info(pwm_ch0, pwm_timer0);
    printf("---------- >>>>>>>>>> %s end <<<<<<<<<< ----------", __func__);
}

void* my_timer_task(void* p_arg)
{

    int ret = os_taskq_post_type(MY_TASK_NAME, MAIN_TCC_TASK, 0, NULL);
    if(ret != OS_NO_ERR)
        log_print(__LOG_ERROR, NULL, "FAIL ! ! !    MAIN_TCC_TASK return value : %d\n", ret);

    return &ret;
}
void* led_timer_task(void* p_arg)
{
    int ret = os_taskq_post_type(MY_TASK_NAME, BREATHE_LED_TASK, 0, NULL);
    if(ret != OS_NO_ERR)
        log_print(__LOG_ERROR, NULL, "FAIL ! ! !    MAIN_TCC_TASK return value : %d\n", ret);

    return &ret;
}

void my_task_init(void)
{
    printf("__________ %s __________\n", __func__);
    int err;
    //gpio_direction_output(IO_PORTA_03, 1);//set this IO output

    my_button_init();
    my_PWM_output_init();
    data_send_to_host[1] = 0x14;
#if 1
    /* create my task, remember do not set stack-size(stksize) too short. if send task queue ,do not set the queue-size(qsize) is zero */
    err = os_task_create(my_task, NULL, 1, 256, 32, MY_TASK_NAME);
    log_print(__LOG_INFO, NULL, "create my task ! ! !    ret = %d\n", err);

    /* create user timer */
    ret_id_timer = sys_hi_timer_add(NULL, my_timer_task, MAIN_TCC_TIMER);
    log_print(__LOG_INFO, NULL, "Main timer task ID : %d\n", ret_id_timer);

    /*同一定时器中最好不要连续发送两次任务消息, 发送任务消息的时间要错开, 否则会因为发送了任务消息, 而任务收到后尚未处理完成, 又收到任务导致队列溢出*/
    /*定时器发送事件时间内未完成任务会导致任务队列溢出*/
    /* Failure to complete a task within the timer send event time causes the task queue to overflow. */
    ret_id_timer_led = sys_hi_timer_add(NULL, led_timer_task, LED_TCC_TIMER);
    log_print(__LOG_INFO, NULL, "LED timer task ID : %d\n", ret_id_timer_led);
#endif
    /* printf pwm model info */
    //log_pwm_led_info();
}

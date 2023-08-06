#ifndef APP_ACTION_H
#define APP_ACTION_H


#define ACTION_EARPHONE_MAIN    0x0001
#define ACTION_IDLE_MAIN    	0x0002
#define ACTION_DONGLE_MAIN    	0x0002

#define APP_NAME_BT										"earphone"
#define APP_NAME_IDLE									"idle"
#define APP_NAME_DONGLE									"dongle"

extern void task_switch(const char *name, int action);
#define task_switch_to_bt()     task_switch(APP_NAME_BT, ACTION_EARPHONE_MAIN)


#endif


#include "key_event_deal.h"
#include "key_driver.h"
#include "app_config.h"
#include "board_config.h"


u8 __attribute__((weak))dongle_key_event_get(struct key_event *key)
{
    if(key->event == 0)
        return KEY_NULL;
    else
        return key->event;
}


#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/*
 *  板级配置选择
 */
#define CONFIG_BOARD_AC695X_DEMO
// #define CONFIG_BOARD_AC6953D_USB

#include "board_ac695x_demo_cfg.h"
#include "board_ac6953d_usb.h"

#define DUT_AUDIO_DAC_LDO_VOLT		DACVDD_LDO_2_70V

#endif

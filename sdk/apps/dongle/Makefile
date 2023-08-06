
includes += \
	-I$(ROOT)/apps/$(APP_CASE)/include/ \
	-I$(ROOT)/apps/$(APP_CASE)/include/common/ \
	-I$(ROOT)/apps/common/ \
	-I$(ROOT)/apps/common/include/\
	-I$(ROOT)/apps/common/config/include/\
	-I$(ROOT)/apps/$(APP_CASE)/board/$(CPU)/ \
	-I$(ROOT)/cpu/$(CPU)/ \
	-I$(ROOT)/cpu/$(CPU)/audio_common \
	-I$(ROOT)/cpu/$(CPU)/audio_dec \
	-I$(ROOT)/cpu/$(CPU)/audio_enc \
	-I$(ROOT)/cpu/$(CPU)/audio_effect \
	-I$(ROOT)/cpu/$(CPU)/audio_mic \
    -I$(ROOT)/apps/common/dev_manager/ \
	-I$(ROOT)/apps/common/device/ \
	-I$(ROOT)/apps/common/audio/ \
	-I$(ROOT)/apps/common/audio/stream/ \

#     $(AR_DIR)/libc.a \


objs += \
	$(ROOT)/apps/common/key/iokey.o \
	$(ROOT)/apps/common/key/irkey.o \
	$(ROOT)/apps/common/key/adkey.o \
	$(ROOT)/apps/common/key/touch_key.o \
	$(ROOT)/apps/common/key/key_driver.o \
	$(ROOT)/apps/$(APP_CASE)/app_main.o \
	$(ROOT)/apps/$(APP_CASE)/app_dongle.o \
	$(ROOT)/apps/$(APP_CASE)/app_idle.o \
	$(ROOT)/apps/$(APP_CASE)/btstack_demo.o \
	$(ROOT)/apps/$(APP_CASE)/especial_demo.o \
	$(ROOT)/apps/$(APP_CASE)/hci_cmds.o \
	$(ROOT)/apps/$(APP_CASE)/misc.o \
	$(ROOT)/apps/$(APP_CASE)/version.o \
	$(ROOT)/apps/$(APP_CASE)/user_cfg.o \
	$(ROOT)/apps/$(APP_CASE)/my_put_test.o \
	$(ROOT)/apps/$(APP_CASE)/dongle_key.o \

#$(ROOT)/apps/$(APP_CASE)/audio_dev.o \
	$(ROOT)/apps/$(APP_CASE)/sbc_codec.o \
	$(ROOT)/apps/$(APP_CASE)/aec/$(CPU)/audio_aec.o \



#objs += \
	$(ROOT)/apps/$(APP_CASE)/usbstack_demo.o \
	$(ROOT)/apps/$(APP_CASE)/usb_audio_demo.o \

-include $(ROOT)/apps/common/usb/Makefile.inc


objs += \
	$(ROOT)/apps/$(APP_CASE)/log_config/lib_btctrler_config.o \
	$(ROOT)/apps/$(APP_CASE)/log_config/lib_driver_config.o \
	$(ROOT)/apps/$(APP_CASE)/log_config/lib_system_config.o \
	$(ROOT)/apps/$(APP_CASE)/log_config/lib_update_config.o \
	$(ROOT)/apps/$(APP_CASE)/log_config/lib_btstack_config.o \
	$(ROOT)/apps/$(APP_CASE)/log_config/lib_media_config.o \
	$(ROOT)/apps/$(APP_CASE)/log_config/app_config.o


ifeq ($(AUDIO_LITE_VERSION),y)
objs += \
        $(ROOT)/apps/common/audio/decode/decode.o
endif




#------------- EQ config-----------------#
# objs += \
	$(ROOT)/apps/$(APP_CASE)/app_eq.o \

# objs += \
	$(ROOT)/apps/config/app_config.o \
	$(ROOT)/apps/config/ci_transport_uart.o \
	$(ROOT)/apps/config/bt_profile_config.o \




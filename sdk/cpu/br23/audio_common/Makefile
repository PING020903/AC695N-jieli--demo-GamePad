

ifeq ($(APP_CASE),earphone)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o \

else ifeq ($(APP_CASE),soundbox)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_link.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_fmtx.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_iis.o \

else ifeq ($(APP_CASE),dongle)

ifeq ($(AUDIO_LITE_VERSION),n)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o
endif

else ifeq ($(APP_CASE),gamebox)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o \

else ifeq ($(APP_CASE),watch)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_link.o \

else ifeq ($(APP_CASE),ICrecorder)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_link.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_iis.o \

else ifeq ($(APP_CASE),adapter)
objs += \
	  $(ROOT)/cpu/$(CPU)/audio_common/app_audio.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_link.o \
	  $(ROOT)/cpu/$(CPU)/audio_common/audio_iis.o \


endif

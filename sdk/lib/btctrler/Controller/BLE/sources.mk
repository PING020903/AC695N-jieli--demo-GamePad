
ifeq ($(CONFIG_BB_BLE_V2), y)
# ifeq ($(CPU), br36)

CC_ARGS += \
-DBLE5_FRAME_DEBUG \

# endif
endif


ifeq ($(CONFIG_LL_V2), n)
includes += \
	-I$(abspath include) \
	-I$(abspath Controller/include/) \
	-I$(abspath Controller/include/common/) \

SRCS_C += \
	$(wildcard Controller/BLE/*.c) \

else
includes += \
	-I$(abspath include) \
	-I$(abspath Controller/include_v2/) \
	-I$(abspath Controller/include_v2/common/) \

SRCS_C += \
	$(wildcard Controller/BLE_v2/*.c) \

endif

ifeq ($(CONFIG_BB_BLE_V2), n)
includes += \
	-I$(abspath port/$(CPU)/baseband/ble) \

#le controller part
SRCS_C += \
	$(wildcard port/$(CPU)/baseband/ble/*.c) \


# SRCS_C += \
	$(wildcard Controller/BLE/sch/*.c) \
	$(wildcard Controller/BLE/bb/*.c) \

else

includes += \
	-I$(abspath port/$(CPU)/baseband/ble_v2) \

# bluetooth 4.x
# SRCS_C += \
	port/$(CPU)/baseband/ble_v2/analog_ble.c \
	port/$(CPU)/baseband/ble_v2/direct_test_dut.c \
	port/$(CPU)/baseband/ble_v2/bb_le_regs.c \
	port/$(CPU)/baseband/ble_v2/bb_le_frame.c \
	port/$(CPU)/baseband/ble_v2/bb_le_pdu.c \
	port/$(CPU)/baseband/ble_v2/bb_le_encrypt.c \
	port/$(CPU)/baseband/ble_v2/bb_le_adv.c \
	port/$(CPU)/baseband/ble_v2/bb_le_scan.c \
	port/$(CPU)/baseband/ble_v2/bb_le_init.c \
	port/$(CPU)/baseband/ble_v2/bb_le_link.c \
	port/$(CPU)/baseband/ble_v2/bb_le_conn.c \
	port/$(CPU)/baseband/ble_v2/bb_le_clk.c \
	port/$(CPU)/baseband/ble_v2/bb_le_schedule.c \
	port/$(CPU)/baseband/ble_v2/bb_le_power.c \
	port/$(CPU)/baseband/ble_v2/bb_le_dut.c \

# bluetooth 5.0
# SRCS_C += \
	port/$(CPU)/baseband/ble_v2/bb_le_ext_frame.c \
	port/$(CPU)/baseband/ble_v2/bb_le_phy.c \
	port/$(CPU)/baseband/ble_v2/bb_le_ext_adv.c \
	port/$(CPU)/baseband/ble_v2/bb_le_ext_scan.c \
	port/$(CPU)/baseband/ble_v2/bb_le_ext_init.c \
	port/$(CPU)/baseband/ble_v2/bb_le_ext_link.c \
	port/$(CPU)/baseband/ble_v2/bb_le_ext_regs.c \

# bluetooth 5.1
# SRCS_C += \
	port/$(CPU)/baseband/ble_v2/bb_le_cte.c \

# bluetooth 5.2
# SRCS_C += \
	port/$(CPU)/baseband/ble_v2/bb_le_iso_pdu.c \
	port/$(CPU)/baseband/ble_v2/bb_le_cis.c \
	port/$(CPU)/baseband/ble_v2/bb_le_bis.c \

SRCS_C += \
	$(wildcard port/$(CPU)/baseband/ble_v2/*.c)

SRCS_C:= $(abspath $(SRCS_C))

endif

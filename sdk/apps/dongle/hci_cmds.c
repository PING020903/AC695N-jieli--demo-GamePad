/*********************************************************************************************
    *   Filename        : hci_cmds.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : caibingquan@zh-jieli.com

    *   Last modifiled  : 2019-05-11 15:09

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "bluetooth.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"

#define OPCODE(ogf, ocf) (ocf | ogf << 10)


/**
 *  Link Control Commands
 */

/**
 * @param lap
 * @param inquiry_length
 * @param num_responses
 */
const hci_cmd_t hci_inquiry = {
    OPCODE(OGF_LINK_CONTROL, 0x01), "311"
};

/**
 */
const hci_cmd_t hci_inquiry_cancel = {
    OPCODE(OGF_LINK_CONTROL, 0x02), ""
};

/**
 * @param bd_addr
 * @param packet_type
 * @param page_scan_repetition_mode
 * @param reserved
 * @param clock_offset
 * @param allow_role_switch
 */
const hci_cmd_t hci_create_connection = {
    OPCODE(OGF_LINK_CONTROL, 0x05), "B21121"
};

/**
 * @param handle
 * @param reason (0x05, 0x13-0x15, 0x1a, 0x29, see Errors Codes in BT Spec Part D)
 */
const hci_cmd_t hci_disconnect = {
    OPCODE(OGF_LINK_CONTROL, 0x06), "H1"
};

#if 1
/**
 * @param bd_addr
 */
const hci_cmd_t hci_create_connection_cancel = {
    OPCODE(OGF_LINK_CONTROL, 0x08), "B"
};

/**
 * @param bd_addr
 * @param role (become master, stay slave)
 */
const hci_cmd_t hci_accept_connection_request = {
    OPCODE(OGF_LINK_CONTROL, 0x09), "B1"
};

/**
 * @param bd_addr
 * @param reason (e.g. CONNECTION REJECTED DUE TO LIMITED RESOURCES (0x0d))
 */
const hci_cmd_t hci_reject_connection_request = {
    OPCODE(OGF_LINK_CONTROL, 0x0a), "B1"
};

/**
 * @param bd_addr
 * @param link_key
 */
const hci_cmd_t hci_link_key_request_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x0b), "BP"
};

/**
 * @param bd_addr
 */
const hci_cmd_t hci_link_key_request_negative_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x0c), "B"
};

/**
 * @param bd_addr
 * @param pin_length
 * @param pin (c-string)
 */
const hci_cmd_t hci_pin_code_request_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x0d), "B1P"
};

/**
 * @param bd_addr
 */
const hci_cmd_t hci_pin_code_request_negative_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x0e), "B"
};

/**
 * @param handle
 * @param packet_type
 */
const hci_cmd_t hci_change_connection_packet_type = {
    OPCODE(OGF_LINK_CONTROL, 0x0f), "H2"
};

/**
 * @param handle
 */
const hci_cmd_t hci_authentication_requested = {
    OPCODE(OGF_LINK_CONTROL, 0x11), "H"
};

/**
 * @param handle
 * @param encryption_enable
 */
const hci_cmd_t hci_set_connection_encryption = {
    OPCODE(OGF_LINK_CONTROL, 0x13), "H1"
};

/**
 * @param handle
 */
const hci_cmd_t hci_change_connection_link_key = {
    OPCODE(OGF_LINK_CONTROL, 0x15), "H"
};

/**
 * @param bd_addr
 * @param page_scan_repetition_mode
 * @param reserved
 * @param clock_offset
 */
const hci_cmd_t hci_remote_name_request = {
    OPCODE(OGF_LINK_CONTROL, 0x19), "B112"
};


/**
 * @param bd_addr
 */
const hci_cmd_t hci_remote_name_request_cancel = {
    OPCODE(OGF_LINK_CONTROL, 0x1A), "B"
};

/**
 * @param handle
 */
const hci_cmd_t hci_read_remote_supported_features_command = {
    OPCODE(OGF_LINK_CONTROL, 0x1B), "H"
};

const hci_cmd_t hci_read_remote_version_information = {
    OPCODE(OGF_LINK_CONTROL, 0x1d), "2"
};

/**
 * @param handle
 * @param transmit_bandwidth 8000(64kbps)
 * @param receive_bandwidth  8000(64kbps)
 * @param max_latency        >= 7ms for eSCO, 0xFFFF do not care
 * @param voice_settings     e.g. CVSD, Input Coding: Linear, Input Data Format: 2’s complement, data 16bit: 00011000000 == 0x60
 * @param retransmission_effort  e.g. 0xFF do not care
 * @param packet_type        at least EV3 for eSCO
 */
const hci_cmd_t hci_setup_sco_connection = {
    OPCODE(OGF_LINK_CONTROL, 0x0028), "H442212"
};

/**
 * @param bd_addr
 * @param transmit_bandwidth
 * @param receive_bandwidth
 * @param max_latency
 * @param voice_settings
 * @param retransmission_effort
 * @param packet_type
 */
const hci_cmd_t hci_accept_sco_connection_request = {
    OPCODE(OGF_LINK_CONTROL, 0x0029), "B442212"
};

/**
 * @param bd_addr
 * @param reason
 */
const hci_cmd_t hci_reject_sco_connection_request = {
    OPCODE(OGF_LINK_CONTROL, 0x002a), "B1"
};

/**
 * @param bd_addr
 * @param IO_capability
 * @param OOB_data_present
 * @param authentication_requirements
 */
const hci_cmd_t hci_io_capability_request_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x2b), "B111"
};

/**
 * @param bd_addr
 */
const hci_cmd_t hci_user_confirmation_request_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x2c), "B"
};

/**
 * @param bd_addr
 */
const hci_cmd_t hci_user_confirmation_request_negative_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x2d), "B"
};

/**
 * @param bd_addr
 * @param numeric_value
 */
const hci_cmd_t hci_user_passkey_request_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x2e), "B4"
};

/**
 * @param bd_addr
 */
const hci_cmd_t hci_user_passkey_request_negative_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x2f), "B"
};

/**
 * @param bd_addr
 * @param c Simple Pairing Hash C
 * @param r Simple Pairing Randomizer R
 */
const hci_cmd_t hci_remote_oob_data_request_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x30), "BPP"
};

/**
 * @param bd_addr
 */
const hci_cmd_t hci_remote_oob_data_request_negative_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x33), "B"
};

/**
 * @param bd_addr
 * @param reason (Part D, Error codes)
 */
const hci_cmd_t hci_io_capability_request_negative_reply = {
    OPCODE(OGF_LINK_CONTROL, 0x34), "B1"
};

/**
 * @param handle
 * @param transmit_bandwidth
 * @param receive_bandwidth
 * @param transmit_coding_format_type
 * @param transmit_coding_format_company
 * @param transmit_coding_format_codec
 * @param receive_coding_format_type
 * @param receive_coding_format_company
 * @param receive_coding_format_codec
 * @param transmit_coding_frame_size
 * @param receive_coding_frame_size
 * @param input_bandwidth
 * @param output_bandwidth
 * @param input_coding_format_type
 * @param input_coding_format_company
 * @param input_coding_format_codec
 * @param output_coding_format_type
 * @param output_coding_format_company
 * @param output_coding_format_codec
 * @param input_coded_data_size
 * @param outupt_coded_data_size
 * @param input_pcm_data_format
 * @param output_pcm_data_format
 * @param input_pcm_sample_payload_msb_position
 * @param output_pcm_sample_payload_msb_position
 * @param input_data_path
 * @param output_data_path
 * @param input_transport_unit_size
 * @param output_transport_unit_size
 * @param max_latency
 * @param packet_type
 * @param retransmission_effort
 */
const hci_cmd_t hci_enhanced_setup_synchronous_connection = {
    OPCODE(OGF_LINK_CONTROL, 0x3d), "H4412212222441221222211111111221"
};

/**
 * @param bd_addr
 * @param transmit_bandwidth
 * @param receive_bandwidth
 * @param transmit_coding_format_type
 * @param transmit_coding_format_company
 * @param transmit_coding_format_codec
 * @param receive_coding_format_type
 * @param receive_coding_format_company
 * @param receive_coding_format_codec
 * @param transmit_coding_frame_size
 * @param receive_coding_frame_size
 * @param input_bandwidth
 * @param output_bandwidth
 * @param input_coding_format_type
 * @param input_coding_format_company
 * @param input_coding_format_codec
 * @param output_coding_format_type
 * @param output_coding_format_company
 * @param output_coding_format_codec
 * @param input_coded_data_size
 * @param outupt_coded_data_size
 * @param input_pcm_data_format
 * @param output_pcm_data_format
 * @param input_pcm_sample_payload_msb_position
 * @param output_pcm_sample_payload_msb_position
 * @param input_data_path
 * @param output_data_path
 * @param input_transport_unit_size
 * @param output_transport_unit_size
 * @param max_latency
 * @param packet_type
 * @param retransmission_effort
 */
const hci_cmd_t hci_enhanced_accept_synchronous_connection = {
    OPCODE(OGF_LINK_CONTROL, 0x3e), "B4412212222441221222211111111221"
};

const hci_cmd_t hci_set_connectionless_slave_broadcast = {
    OPCODE(OGF_LINK_CONTROL, 0x41), ""
};
const hci_cmd_t hci_set_connectionless_slave_broadcast_receive = {
    OPCODE(OGF_LINK_CONTROL, 0x42), ""
};
const hci_cmd_t hci_start_synchronization_train = {
    OPCODE(OGF_LINK_CONTROL, 0x43), "B21"
};
const hci_cmd_t hci_receive_synchronization_train = {
    OPCODE(OGF_LINK_CONTROL, 0x44), "1"
};

const hci_cmd_t hci_send_keypress_notification = {
    OPCODE(OGF_LINK_CONTROL, 0x60), "B1"
};

/**
 *  Link Policy Commands
 */

/**
 * @param handle
 * @param sniff_max_interval
 * @param sniff_min_interval
 * @param sniff_attempt
 * @param sniff_timeout
 */
const hci_cmd_t hci_sniff_mode = {
    OPCODE(OGF_LINK_POLICY, 0x03), "H2222"
};
const hci_cmd_t hci_exit_sniff_mode = {
    OPCODE(OGF_LINK_POLICY, 0x04), "H"
// handle, Sniff_Max_Interval, Sniff_Min_Interval, Sniff_Attempt, Sniff_Timeout
};
/**
 * @param handle
 * @param flags
 * @param service_type
 * @param token_rate (bytes/s)
 * @param peak_bandwith (bytes/s)
 * @param latency (us)
 * @param delay_variation (us)
 */
const hci_cmd_t hci_qos_setup = {
    OPCODE(OGF_LINK_POLICY, 0x07), "H114444"
};

/**
 * @param handle
 */
const hci_cmd_t hci_role_discovery = {
    OPCODE(OGF_LINK_POLICY, 0x09), "H"
};

/**
 * @param bd_addr
 * @param role (0=master,1=slave)
 */
const hci_cmd_t hci_switch_role_command = {
    OPCODE(OGF_LINK_POLICY, 0x0b), "B1"
};

/**
 * @param handle
 */
const hci_cmd_t hci_read_link_policy_settings = {
    OPCODE(OGF_LINK_POLICY, 0x0c), "H"
};

/**
 * @param handle
 * @param settings
 */
const hci_cmd_t hci_write_link_policy_settings = {
    OPCODE(OGF_LINK_POLICY, 0x0d), "H2"
};
const hci_cmd_t hci_write_default_link_policy_settings = {
    OPCODE(OGF_LINK_POLICY, 0x0f), "2"
// settings
};

/**
 *  Controller & Baseband Commands
 */

/**
 * @param event_mask_lover_octets
 * @param event_mask_higher_octets
 */
const hci_cmd_t hci_set_event_mask = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x01), "44"
};
#endif
/**
 */
const hci_cmd_t hci_reset = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x03), ""
};
/**
 * @param handle
 */
const hci_cmd_t hci_flush = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x09), "H"
};
/**
 * @param bd_addr
 * @param delete_all_flags
 */
const hci_cmd_t hci_delete_stored_link_key = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x12), "B1"
};

/**
 * @param local_name (UTF-8, Null Terminated, max 248 octets)
 */
const hci_cmd_t hci_write_local_name = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x13), "N"
};
/**
 */
const hci_cmd_t hci_read_local_name = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x14), ""
};
/**
 * @param page_timeout (* 0.625 ms)
 */
const hci_cmd_t hci_write_page_timeout = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x18), "2"
};

/**
 * @param scan_enable (no, inq, page, inq+page)
 */
const hci_cmd_t hci_write_scan_enable = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x1A), "1"
};

const hci_cmd_t HCI_Write_Page_Scan_Activity = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x1c), "22"

};

const hci_cmd_t hci_write_inquiry_scan_activity = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x1e), "22"

};
/**
 * @param authentication_enable
 */
const hci_cmd_t hci_write_authentication_enable = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x20), "1"
};

/**
 * @param class_of_device
 */
const hci_cmd_t hci_write_class_of_device = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x24), "3"
};
const hci_cmd_t hci_read_voice_setting = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x25), ""
};
const hci_cmd_t hci_write_voice_setting = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x26), "2"
};
/**
 */
const hci_cmd_t hci_read_num_broadcast_retransmissions = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x29), ""
};

/**
 * @param num_broadcast_retransmissions (e.g. 0 for a single broadcast)
 */
const hci_cmd_t hci_write_num_broadcast_retransmissions = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x2a), "1"
};

/**
 * @param synchronous_flow_control_enable - if yes, num completed packet everts are sent for SCO packets
 */
const hci_cmd_t hci_write_synchronous_flow_control_enable = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x2f), "1"
};
const hci_cmd_t hci_flow_control_enable = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x31), "1"
// status
};
/**
 * @param host_acl_data_packet_length
 * @param host_synchronous_data_packet_length
 * @param host_total_num_acl_data_packets
 * @param host_total_num_synchronous_data_packets
 */
const hci_cmd_t hci_host_buffer_size = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x33), "2122"
};
const hci_cmd_t hci_host_num_of_completed = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x35), "1H2"
// Host_ACL_Data_Packet_Length:, Host_Synchronous_Data_Packet_Length:, Host_Total_Num_ACL_Data_Packets:, Host_Total_Num_Synchronous_Data_Packets:
};
/**
 * @param handle
 */
const hci_cmd_t hci_read_link_supervision_timeout = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x36), "H"
};

/**
 * @param handle
 * @param timeout (0x0001 - 0xFFFF Time -> Range: 0.625ms - 40.9 SEC)
 */
const hci_cmd_t hci_write_link_supervision_timeout = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x37), "H2"
};




/**
 * @param inquiry_mode (0x00 = standard, 0x01 = with RSSI, 0x02 = extended)
 */
const hci_cmd_t hci_write_inquiry_mode = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x45), "1"
};
const hci_cmd_t hci_write_page_scan_type = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x47), "1"

};
/**
 * @param fec_required
 * @param exstended_inquiry_response
 */
const hci_cmd_t hci_write_extended_inquiry_response = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x52), "1E"
};

/**
 * @param mode (0 = off, 1 = on)
 */
const hci_cmd_t hci_write_simple_pairing_mode = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x56), "1"
};
const hci_cmd_t hci_read_local_oob_data = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x57), ""
//
};
/**
 * @param mode (0 = off, 1 = on)
 */
const hci_cmd_t hci_write_default_erroneous_data_reporting = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x5B), "1"
};
/**
 */
const hci_cmd_t hci_read_le_host_supported = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x6c), ""
// return: status, le supported host, simultaneous le host
};

/**
 * @param le_supported_host
 * @param simultaneous_le_host
 */
const hci_cmd_t hci_write_le_host_supported = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x6d), "11"
// return: status
};

/**
 */
const hci_cmd_t hci_read_local_extended_ob_data = {
    OPCODE(OGF_CONTROLLER_BASEBAND, 0x7d), ""
// return status, C_192, R_192, R_256, C_256
};


/**
 * Testing Commands
 */

#if HCI_TESTING_MODE
/**
 */
const hci_cmd_t hci_read_loopback_mode = {
    OPCODE(OGF_TESTING, 0x01), ""
// return: status, loopback mode (0 = off, 1 = local loopback, 2 = remote loopback)
};

/**
 * @param loopback_mode
 */
const hci_cmd_t hci_write_loopback_mode = {
    OPCODE(OGF_TESTING, 0x02), "1"
// return: status
};

/**
 */
const hci_cmd_t hci_enable_device_under_test_mode = {
    OPCODE(OGF_TESTING, 0x03), ""
// return: status
};

/**
 * @param simple_pairing_debug_mode
 */
const hci_cmd_t hci_write_simple_pairing_debug_mode = {
    OPCODE(OGF_TESTING, 0x04), "1"
// return: status
};

/**
 * @param handle
 * @param dm1_acl_u_mode
 * @param esco_loopback_mode
 */
const hci_cmd_t hci_write_secure_connections_test_mode = {
    OPCODE(OGF_TESTING, 0x0a), "H11"
// return: status
};

#endif
/**
 * Informational Parameters
 */

const hci_cmd_t hci_read_local_version_information = {
    OPCODE(OGF_INFORMATIONAL_PARAMETERS, 0x01), ""
};

const hci_cmd_t hci_read_local_supported_commands = {
    OPCODE(OGF_INFORMATIONAL_PARAMETERS, 0x02), ""
};
const hci_cmd_t hci_read_local_supported_features = {
    OPCODE(OGF_INFORMATIONAL_PARAMETERS, 0x03), ""
};
const hci_cmd_t hci_read_buffer_size = {
    OPCODE(OGF_INFORMATIONAL_PARAMETERS, 0x05), ""
};
const hci_cmd_t hci_read_bd_addr = {
    OPCODE(OGF_INFORMATIONAL_PARAMETERS, 0x09), ""
};

/**
 * Status Paramters
 */

/**
 * @param handle
 */
const hci_cmd_t hci_read_rssi = {
    OPCODE(OGF_STATUS_PARAMETERS, 0x05), "H"
// no params
};
/*vendor command*/
const hci_cmd_t hci_write_local_address = {
    OPCODE(OGF_VENDOR, 0x1), "B"
// 自定义命令，用于设置蓝牙地址
};
const hci_cmd_t hci_set_local_info = {
    /**
     * @param STEREO_INDICATE_NAME_EN
     * @param stereo_indicate ,2 byte
     * @param LMP_WORK_MODE
     * @param SUPERTIMEOUT_VAL
     * @param CTRL_CONN_NUM
     * @param ESCO_PACKET_TYPE
     * @param SNIFF_MODE_flag
     * @param lmp_stereo_en_flag
     * @param lmp_device_role
     * @param phone_conn_filt
     */
    OPCODE(OGF_VENDOR, 0x2), "1M12111111"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_set_pin_code = {
    OPCODE(OGF_VENDOR, 0x3), "P"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_cancel_page = {
    OPCODE(OGF_VENDOR, 0x4), ""
// 自定义命令，用于设置蓝牙信息
};
#define HCI_WRITE_TRANSMIT_POWER_LEVEL            0x0005
const hci_cmd_t hci_set_power_cmd = {
    OPCODE(OGF_VENDOR, HCI_WRITE_TRANSMIT_POWER_LEVEL), "1"
};
#define HCI_WRITE_INTERNAL_OSC            0x0006
const hci_cmd_t hci_set_internal_osc_cmd = {
    OPCODE(OGF_VENDOR, HCI_WRITE_INTERNAL_OSC), "11"
};
const hci_cmd_t hci_restart_sbc = {
    OPCODE(OGF_VENDOR, 0x0a), "B1"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_sync_cmd_fun = {
    OPCODE(OGF_VENDOR, 0x0b), "B1111111"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_sync_src = {
    OPCODE(OGF_VENDOR, 0x0c), "B11111111"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_sync_sbc = {
    OPCODE(OGF_VENDOR, 0x0d), "B1"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_tws_send_cmd = {
    OPCODE(OGF_VENDOR, 0x0f), "1411"
// 自定义命令，用于设置蓝牙信息
};
const hci_cmd_t hci_test_key_cmd = {
    OPCODE(OGF_VENDOR, 0x10), "1H"
};
const hci_cmd_t hci_close_bredr_cmd = {
    OPCODE(OGF_VENDOR, 0x11), "1"
};
const hci_cmd_t hci_set_work_time_cmd = {
    OPCODE(OGF_VENDOR, 0x12), "B"
};
const hci_cmd_t hci_clr_work_time_cmd = {
    OPCODE(OGF_VENDOR, 0x13), "B"
};



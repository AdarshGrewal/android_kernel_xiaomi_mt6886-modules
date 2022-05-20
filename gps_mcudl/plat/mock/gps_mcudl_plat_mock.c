/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#include "gps_dl_config.h"
#include "gps_mcudl_config.h"
#include "gps_mcudl_log.h"
#include "gps_mcudl_each_link.h"
#include "gps_mcudl_plat_api.h"
#include "gps_mcudl_data_pkt_host_api.h"
#include "gps_mcudl_xlink.h"
#include "gps_mcudl_ylink.h"
#include "gps_mcusys_fsm.h"
#if GPS_DL_ON_LINUX
#include "gps_dl_subsys_reset.h"
#include "gps_dl_linux_reserved_mem_v2.h"
#endif
#include "gps_dl_hal.h"
#include "gps_dl_time_tick.h"
#include "gps_mcu_hif_api.h"
#include "gps_mcu_hif_host.h"
#if GPS_DL_HAS_MCUDL_FW
#include "gps_mcudl_fw_code.h"
#endif


struct gps_mcudl_ystate {
	bool open;
	gpsmdl_u32 xstate_bitmask;
};

struct gps_mcudl_ystate g_gps_mcudl_ystate_list[GPS_MDLY_CH_NUM];

int gps_mcudl_mock_do_stp_ctrl(enum gps_mcudl_yid yid, bool open)
{
	int retval = 0;

	if (yid == GPS_MDLY_NORMAL) {
		if (open)
			retval = gps_mcudl_stpgps1_open();
		else
			retval = gps_mcudl_stpgps1_close();
	}

	MDL_LOGYI(yid, "open=%d, ret=%d", open, retval);
	return retval;
}

unsigned int gps_dl_util_get_u32(const unsigned char *p_buffer)
{
	return ((unsigned int)(*p_buffer) +
		((unsigned int)(*(p_buffer + 1)) << 8) +
		((unsigned int)(*(p_buffer + 2)) << 16) +
		((unsigned int)(*(p_buffer + 3)) << 24));
}

bool gps_mcudl_link_drv_on_recv_mgmt_data(const unsigned char *p_data, unsigned int data_len)
{
	unsigned char cmd;
	unsigned char status = 0xFF;
	unsigned int addr, bytes, value;

	/*TODO:*/
	MDL_LOGW("data_len=%d, data0=0x%x", data_len, p_data[0]);

	cmd = p_data[0];
	if (data_len >= 2)
		status = p_data[1];

	switch (cmd) {
	case 5:
		if (data_len >= 16) {
			addr = gps_dl_util_get_u32(&p_data[4]);
			bytes = gps_dl_util_get_u32(&p_data[8]);
			value = gps_dl_util_get_u32(&p_data[12]);
			MDL_LOGW("mcu reg read: stat=%d, addr=0x%08x, bytes=%d, value[0]=0x%08x",
				status, addr, bytes, value);
		}
		break;
	default:
		break;
	}

	return true;
}

bool gps_mcudl_link_drv_on_recv_normal_data(const unsigned char *p_data, unsigned int data_len)
{
	MDL_LOGW("data_len=%d, data0=0x%x", data_len, p_data[0]);
	gps_mcudl_stpgps1_read_proc2(p_data, data_len);
	return true;
}

int gps_mcudl_hal_link_power_ctrl(enum gps_mcudl_xid xid, int op)
{
	enum gps_mcudl_yid yid;
	struct gps_mcudl_ystate *p_ystate;
	gpsmdl_u32 old_xbitmask, new_xbitmask;
	bool do_stp_ctrl = false;
	int stp_ctrl_ret = 0;

	yid = GPS_MDL_X2Y(xid);
	p_ystate = &g_gps_mcudl_ystate_list[yid];
	old_xbitmask = p_ystate->xstate_bitmask;
	new_xbitmask = old_xbitmask;
	if (op)
		new_xbitmask |= (1UL << xid);
	else
		new_xbitmask &= ~(1UL << xid);

	if (new_xbitmask == old_xbitmask) {
		/* do nothing */
		;
	} else if (op && old_xbitmask == 0) {
		/* turn on */
		do_stp_ctrl = true;

		MDL_LOGYI(yid, "gps_mcu_hif_init");
		gps_mcu_hif_init();
		MDL_LOGYI(yid, "gps_mcudl_ap2mcu_context_init");
		gps_mcudl_ap2mcu_context_init(yid);
		gps_mcusys_mnlbin_fsm(GPS_MCUSYS_MNLBIN_SYS_ON);
		gps_mcusys_gpsbin_state_set(GPS_MCUSYS_GPSBIN_PRE_ON);
		MDL_LOGYI(yid, "gps_mcudl_may_do_fw_loading, before");
		gps_mcudl_may_do_fw_loading();
		MDL_LOGYI(yid, "gps_mcudl_may_do_fw_loading, after");
		stp_ctrl_ret = gps_mcudl_mock_do_stp_ctrl(yid, true);
		gps_mcusys_gpsbin_state_set(GPS_MCUSYS_GPSBIN_POST_ON);
	} else if (!op && new_xbitmask == 0) {
		/* turn off */
		do_stp_ctrl = true;
		gps_mcusys_gpsbin_state_set(GPS_MCUSYS_GPSBIN_PRE_OFF);
		stp_ctrl_ret = gps_mcudl_mock_do_stp_ctrl(yid, false);
		MDL_LOGYI(yid, "gps_mcudl_clear_fw_loading_done_flag");
		gps_mcusys_gpsbin_state_set(GPS_MCUSYS_GPSBIN_POST_OFF);
		gps_mcudl_clear_fw_loading_done_flag();
	}

	if (stp_ctrl_ret == 0 || !op)
		p_ystate->xstate_bitmask = new_xbitmask;

	MDL_LOGYI(yid, "xid=%d, op=%d, xbitmask: 0x%08x -> 0x%08x, stp_ctrl: do=%d, ret=%d",
		xid, op, old_xbitmask, new_xbitmask, do_stp_ctrl, stp_ctrl_ret);
	return stp_ctrl_ret;
}

unsigned int g_conn_xuser;
bool g_gps_mcudl_ever_do_coredump;

/*TODO:*/
int gps_mcudl_hal_conn_power_ctrl(enum gps_mcudl_xid xid, int op)
{
	MDL_LOGXI_ONF(xid,
		"sid = %d, op = %d, user = 0x%x,%d, tia_on = %d",
		gps_mcudl_each_link_get_session_id(xid),
		op, g_conn_xuser, g_gps_conninfa_on, g_gps_tia_on);

	if (1 == op) {
		if (g_conn_xuser == 0) {
			g_gps_mcudl_ever_do_coredump = false;
			gps_dl_log_info_show();
			if (!gps_dl_hal_conn_infra_driver_on())
				return -1;

			gps_dl_hal_load_clock_flag();
#if GPS_DL_HAS_PLAT_DRV
			/*gps_dl_wake_lock_hold(true);*/
#if GPS_DL_USE_TIA
			/*gps_dl_tia_gps_ctrl(true);*/
			/*g_gps_tia_on = true;*/
#endif
#endif
		}
		g_conn_xuser |= (1UL << xid);
	} else if (0 == op) {
		g_conn_xuser &= ~(1UL << xid);
		if (g_conn_xuser == 0) {
#if GPS_DL_HAS_PLAT_DRV
#if GPS_DL_USE_TIA
			/*if (g_gps_tia_on) {*/
			/*	gps_dl_tia_gps_ctrl(false);*/
			/*	g_gps_tia_on = false;*/
			/*}*/
#endif
			/*gps_dl_wake_lock_hold(false);*/
#endif
			gps_dl_hal_conn_infra_driver_off();
		}
	}

	return 0;
}

void gps_mcudl_stpgps1_event_cb(void)
{
	enum gps_mcudl_yid y_id = GPS_MDLY_NORMAL;
	bool to_notify = true;

	to_notify = !gps_mcudl_ap2mcu_get_wait_read_flag(y_id);
	if (to_notify) {
		gps_mcudl_ap2mcu_set_wait_read_flag(y_id, true);
		gps_mcudl_ylink_event_send(y_id, GPS_MCUDL_YLINK_EVT_ID_RX_DATA_READY);
	}
	MDL_LOGYD(GPS_MDLY_NORMAL, "ntf=%d", to_notify);
}

void gps_mcudl_stpgps1_read_proc(void)
{
	enum gps_mcudl_yid y_id;
	gpsmdl_u8 tmp_buf[2048];
	int ret_len;

	y_id = GPS_MDLY_NORMAL;
	MDL_LOGYD(y_id, "");

	gps_mcudl_ap2mcu_set_wait_read_flag(y_id, false);
	do {
		ret_len = gps_mcudl_stpgps1_read_nonblock(&tmp_buf[0], 2048);
		MDL_LOGYD(y_id, "read: len=%d", ret_len);
		if (ret_len > 0)
			gps_mcudl_ap2mcu_ydata_recv(y_id, &tmp_buf[0], ret_len);
	} while (ret_len > 0);
}

void gps_mcudl_stpgps1_read_proc2(const unsigned char *p_data, unsigned int data_len)
{
	enum gps_mcudl_yid y_id;

	y_id = GPS_MDLY_NORMAL;
	gps_mcudl_ap2mcu_set_wait_read_flag(y_id, false);
	MDL_LOGYD(y_id, "read: len=%u", data_len);
	if (data_len > 0)
		gps_mcudl_ap2mcu_ydata_recv(y_id, (gpsmdl_u8 *)p_data, data_len);
}

void gps_mcudl_stpgps1_reset_start_cb(void)
{
	enum gps_mcudl_yid y_id = GPS_MDLY_NORMAL;

	MDL_LOGI("");
	gps_mcudl_clear_fw_loading_done_flag();
	/*gps_dl_on_pre_connsys_reset();*/
	gps_mcudl_ylink_event_send(y_id, GPS_MCUDL_YLINK_EVT_ID_MCU_RESET_START);
}

void gps_mcudl_stpgps1_reset_end_cb(void)
{
	enum gps_mcudl_yid y_id = GPS_MDLY_NORMAL;

	MDL_LOGI("");
	/*gps_dl_on_post_connsys_reset();*/
	gps_mcudl_ylink_event_send(y_id, GPS_MCUDL_YLINK_EVT_ID_MCU_RESET_END);
}

int gps_mcudl_stpgps1_open(void)
{
	bool is_okay;
#if (GPS_DL_HAS_MCUDL_FW && GPS_DL_HAS_MCUDL_HAL)
	gps_mcudl_xlink_on(&c_gps_mcudl_rom_only_fw_list);
#endif

	gps_dl_sleep_us(100*1000, 200*1000);

	gps_mcu_hif_recv_listen_start(GPS_MCU_HIF_CH_DMALESS_MGMT,
		&gps_mcudl_link_drv_on_recv_mgmt_data);
	gps_mcu_hif_recv_listen_start(GPS_MCU_HIF_CH_DMA_NORMAL,
		&gps_mcudl_link_drv_on_recv_normal_data);
	MDL_LOGI("add listeners, done");

	is_okay = gps_mcu_hif_send(GPS_MCU_HIF_CH_DMALESS_MGMT, "\x01", 1);
	MDL_LOGW("write cmd1, is_ok=%d", is_okay);

	gps_dl_sleep_us(100*1000, 200*1000);
	MDL_LOGI("have some wait, done");
	return 0;
}

int gps_mcudl_stpgps1_close(void)
{
	bool is_okay;

	is_okay = gps_mcu_hif_send(GPS_MCU_HIF_CH_DMALESS_MGMT, "\x02", 1);
	MDL_LOGW("write cmd2, is_ok=%d", is_okay);

	gps_dl_sleep_us(100*1000, 200*1000);

	gps_mcu_hif_recv_listen_stop(GPS_MCU_HIF_CH_DMA_NORMAL);
	gps_mcu_hif_recv_listen_stop(GPS_MCU_HIF_CH_DMALESS_MGMT);
	MDL_LOGI("remove listeners, done");

#if GPS_DL_HAS_MCUDL_HAL
	gps_mcudl_xlink_off();
#endif
	return 0;
}

int gps_mcudl_stpgps1_write(const unsigned char *kbuf, unsigned int count)
{
	bool is_okay;

	is_okay = gps_mcu_hif_send(GPS_MCU_HIF_CH_DMA_NORMAL, kbuf, count);
	MDL_LOGW("write count=%d, is_ok=%d", count, is_okay);
	if (!is_okay)
		return 0;
	return count;
}

int gps_mcudl_stpgps1_read_nonblock(unsigned char *kbuf, unsigned int count)
{
	return 0;
}

void gps_nv_emi_clear(void)
{
	struct gps_mcudl_emi_layout *p_layout;

	p_layout = gps_dl_get_conn_emi_layout_ptr();
	memset_io(&p_layout->gps_nv_emi[0], 0, sizeof(p_layout->gps_nv_emi));
}

void *gps_emi_get_nv_mem_ptr(void)
{
	struct gps_mcudl_emi_layout *p_layout;

	p_layout = gps_dl_get_conn_emi_layout_ptr();
	return (void *)&p_layout->gps_nv_emi[0];
}



/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include <linux/arm-smccc.h>
#include <linux/soc/mediatek/mtk_sip_svc.h>


enum conn_smc_opid {
	/* gps_hw_ops */
	SMC_GPS_WAKEUP_CONNINFRA_TOP_OFF_OPID = 1,
	SMC_GPS_COMMON_ON_PART1_OPID,
	SMC_GPS_COMMON_ON_PART2_OPID,
	SMC_GPS_COMMON_ON_PART3_OPID,
	SMC_GPS_COMMON_ON_PART4_OPID,
	SMC_GPS_COMMON_ON_FAIL_HANDLER_OPID,
	SMC_GPS_COMMON_ON_PART5_OPID,
	SMC_GPS_DL_HW_GPS_PWR_STAT_CTRL_OPID,
	SMC_GPS_DL_SET_DSP_ON_AND_POLL_ACK_OPID,
	SMC_GPS_DL_HW_USRT_CTRL_OPID,
	SMC_GPS_DL_SET_CFG_DSP_MEM_AND_DSP_OFF_OPID,
};

bool gps_dl_hw_gps_force_wakeup_conninfra_top_off(bool enable);


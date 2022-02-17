/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */
#ifndef _GPS_DL_HW_DEP_MACRO_H
#define _GPS_DL_HW_DEP_MACRO_H

#include "gps_dl_hw_ver.h"
#include "conn_infra/conn_infra_cfg.h"
#include "conn_infra/conn_host_csr_top.h"
#include "conn_infra/conn_wt_slp_ctl_reg.h"
#include "conn_infra/conn_infra_rgu.h"
#include "conn_infra/conn_semaphore.h"

#include "gps/bgf_gps_cfg.h"
#include "gps/bgf_gps_rgu_on.h"
#include "gps/bgf_gps_cfg_on.h"

#if GPS_DL_HAS_PTA
#include "conn_infra/conn_uart_pta.h"
#include "conn_infra/conn_pta.h"
#include "conn_infra/conn_clkgen_on_top.h"
#endif

#define GDL_HW_SUPPORT_LIST "SUPPORT:MT6877"

#define GDL_HW_CHECK_CONN_INFRA_VER(p_poll_okay, p_poll_ver)             \
	GDL_HW_POLL_ENTRY_VERBOSE(GPS_DL_CONN_INFRA_BUS,                 \
		CONN_INFRA_CFG_CONN_HW_VER_RO_CONN_HW_VERSION,           \
		p_poll_okay, p_poll_ver, POLL_DEFAULT, (                 \
			(*p_poll_ver == GDL_HW_CONN_INFRA_VER_MT6877))   \
	)

#define GDL_HW_SET_EMI_REMAP_FIELD \
	CONN_HOST_CSR_TOP_CONN2AP_REMAP_GPS_EMI_BASE_ADDR_CONN2AP_REMAP_GPS_EMI_BASE_ADDR

#define GDL_HW_SET_CONN2GPS_SLP_PROT_RX_VAL(val) \
	GDL_HW_SET_CONN_INFRA_ENTRY( \
		CONN_INFRA_CFG_CONN_INFRA_CONN2GPS_SLP_CTRL_CFG_CONN2GPS_GALS_RX_SLP_PROT_SW_EN, val)

#define GDL_HW_POLL_CONN2GPS_SLP_PROT_RX_UNTIL_VAL(val, timeout, p_is_okay) \
	GDL_HW_POLL_CONN_INFRA_ENTRY( \
		CONN_CFG_GALS_CONN2GPS_SLP_STATUS_CONN2GPS_GALS_CTRL_PROT_RX_RDY, \
		val, timeout, p_is_okay)


#define GDL_HW_SET_CONN2GPS_SLP_PROT_TX_VAL(val) \
	GDL_HW_SET_CONN_INFRA_ENTRY( \
		CONN_INFRA_CFG_CONN_INFRA_CONN2GPS_SLP_CTRL_CFG_CONN2GPS_GALS_TX_SLP_PROT_SW_EN, val)

#define GDL_HW_POLL_CONN2GPS_SLP_PROT_TX_UNTIL_VAL(val, timeout, p_is_okay) \
	GDL_HW_POLL_CONN_INFRA_ENTRY( \
		CONN_CFG_GALS_CONN2GPS_SLP_STATUS_CONN2GPS_GALS_CTRL_PROT_TX_RDY, \
		val, timeout, p_is_okay)


#define GDL_HW_SET_GPS2CONN_SLP_PROT_RX_VAL(val) \
	GDL_HW_SET_CONN_INFRA_ENTRY( \
		CONN_INFRA_CFG_CONN_INFRA_GPS2CONN_SLP_CTRL_CFG_GPS2CONN_GALS_RX_SLP_PROT_SW_EN, val)

#define GDL_HW_POLL_GPS2CONN_SLP_PROT_RX_UNTIL_VAL(val, timeout, p_is_okay) \
	GDL_HW_POLL_CONN_INFRA_ENTRY( \
		CONN_CFG_GALS_GPS2CONN_SLP_STATUS_GPS2CONN_GALS_CTRL_PROT_RX_RDY, \
		val, timeout, p_is_okay)


#define GDL_HW_SET_GPS2CONN_SLP_PROT_TX_VAL(val) \
	GDL_HW_SET_CONN_INFRA_ENTRY( \
		CONN_INFRA_CFG_CONN_INFRA_GPS2CONN_SLP_CTRL_CFG_GPS2CONN_GALS_TX_SLP_PROT_SW_EN, val)

#define GDL_HW_POLL_GPS2CONN_SLP_PROT_TX_UNTIL_VAL(val, timeout, p_is_okay) \
	GDL_HW_POLL_CONN_INFRA_ENTRY( \
		CONN_CFG_GALS_GPS2CONN_SLP_STATUS_GPS2CONN_GALS_CTRL_PROT_TX_RDY, \
		val, timeout, p_is_okay)


/* For MT6877, no need to wait */
#define GDL_HW_MAY_WAIT_CONN_INFRA_SLP_PROT_DISABLE_ACK(p_poll_okay) \
	   { *p_poll_okay = true; }


/* For MT6877, the dump address list as below */
#define GDL_HW_DUMP_SLP_RPOT_STATUS() do { \
	gps_dl_bus_rd_opt(GPS_DL_CONN_INFRA_BUS, \
		CONN_INFRA_CFG_GALS_CONN2GPS_SLP_STATUS_ADDR, \
		BMASK_RW_FORCE_PRINT); \
	gps_dl_bus_rd_opt(GPS_DL_CONN_INFRA_BUS, \
		CONN_INFRA_CFG_GALS_GPS2CONN_SLP_STATUS_ADDR, \
		BMASK_RW_FORCE_PRINT); \
} while (0)

/* For MT6877, the address is 0x18000028 */
#define GDL_HW_SET_CONN_INFRA_BGF_EN(val) do { \
	unsigned int tmp_val; \
	tmp_val = GDL_HW_RD_CONN_INFRA_REG(CONN_INFRA_RGU_BGFYS_ON_TOP_PWR_CTL_1_ADDR); \
	if (val == 0) { \
		tmp_val = (0x42540000 | (tmp_val & 0xFFFF)) & ~0x80; \
		} \
	else { \
		tmp_val = (0x42540000 | (tmp_val & 0xFFFF)) | 0x80; \
	    } \
	GDL_HW_WR_CONN_INFRA_REG(CONN_INFRA_RGU_BGFYS_ON_TOP_PWR_CTL_1_ADDR, tmp_val); \
	GDL_HW_RD_CONN_INFRA_REG(CONN_INFRA_RGU_BGFYS_ON_TOP_PWR_CTL_1_ADDR); \
} while (0)


#define	GDL_HW_GET_CONN_INFRA_ADIE()

#define GDL_HW_SET_GPS_FUNC_EN(val) \
	GDL_HW_SET_CONN_INFRA_ENTRY(CONN_INFRA_CFG_GPS_PWRCTRL0_GPS_FUNCTION_EN, val)

/* For MT6877, GPS has dedicate entry for top clk control */
#define GDL_HW_ADIE_TOP_CLK_EN(val, p_poll_okay) do { \
	GDL_HW_SET_CONN_INFRA_ENTRY( \
		CONN_WT_SLP_CTL_REG_WB_SLP_TOP_CK_5_WB_SLP_TOP_CK_5, val); \
	GDL_HW_POLL_CONN_INFRA_ENTRY( \
		CONN_WT_SLP_CTL_REG_WB_SLP_TOP_CK_5_WB_SLP_TOP_CK_5_BSY, 0, POLL_DEFAULT, p_poll_okay); \
} while (0)

#define GDL_HW_RD_SPI_GPS_STATUS() do { \
	GDL_HW_RD_CONN_INFRA_REG(CONN_RF_SPI_MST_ADDR_SPI_STA_ADDR); \
	GDL_HW_RD_CONN_INFRA_REG(CONN_RF_SPI_MST_ADDR_SPI_GPS_GPS_ADDR_ADDR); \
	GDL_HW_RD_CONN_INFRA_REG(CONN_RF_SPI_MST_ADDR_SPI_GPS_GPS_WDAT_ADDR); \
	GDL_HW_RD_CONN_INFRA_REG(CONN_RF_SPI_MST_ADDR_SPI_GPS_GPS_RDAT_ADDR); \
	GDL_HW_RD_CONN_INFRA_REG(CONN_RF_SPI_MST_ADDR_SPI_STA_ADDR); \
} while (0)


/* For MT6877:
 * 8: HW TICK H/L, BG tick H/L, TX_END/TX_RD, RX_END/RX_WR
 * 3: PC, GALMAN CNT, WRHOST CNT
 */
#define GPS_DSP_REG_POLL_MAX (11)
#define GPS_L1_REG_POLL_LIST { \
	0x5028, 0x5029, 0x0100, 0x0101, 0x4882, 0x4883, 0x4886, 0x4887, \
	0x9FF0, 0x9FF1, 0x9FF2, }

#define GPS_L5_REG_POLL_LIST { \
	0x5014, 0x5015, 0x0100, 0x0101, 0x4882, 0x4883, 0x4886, 0x4887, \
	0x9FF0, 0x9FF1, 0x9FF2, }


/* For MT6877:
 * 9: PC, GALMAN CNT, WRHOST CNT, DBTT CNT, NEXT CNT, BG TICK H/L, HW TICK H/L
 * 11: USRT CTL, STA, TX_END/RD/MAX, RX_MAX/END/WR, TX_CNT, RX_CNT, MISC
 */
#define GPS_DSP_REG_DBG_POLL_MAX (20)
#define GPS_L1_REG_DBG_POLL_LIST  { \
	0x9FF0, 0x9FF1, 0x9FF2, 0x9FF3, 0x9FF4, 0x0100, 0x0101, 0x5028, 0x5029, \
	0x4880, 0x4881, 0x4882, 0x4883, 0x4884, 0x4885, 0x4886, 0x4887, 0x4888, 0x4889, 0x488a, }

#define GPS_L5_REG_DBG_POLL_LIST { \
	0x9FF0, 0x9FF1, 0x9FF2, 0x9FF3, 0x9FF4, 0x0100, 0x0101, 0x5014, 0x5015, \
	0x4880, 0x4881, 0x4882, 0x4883, 0x4884, 0x4885, 0x4886, 0x4887, 0x4888, 0x4889, 0x488a, }


/* For for COS_SEMA index definition, see:
 * conninfra/platform/mt6877/include/mt6877.h
 */
#if GPS_DL_USE_BGF_SEL_SEMA
/* COS_SEMA_BGF_SEL_INDEX = 0, GPS use M5 */
#define COS_SEMA_BGF_SEL_STA_ENTRY_FOR_GPS \
	CONN_SEMAPHORE_CONN_SEMA00_M5_OWN_STA_CONN_SEMA00_M5_OWN_STA

#define COS_SEMA_BGF_SEL_REL_ENTRY_FOR_GPS \
	CONN_SEMAPHORE_CONN_SEMA00_M5_OWN_REL_CONN_SEMA00_M5_OWN_REL
#endif

#if GPS_DL_HAS_PTA
/* COS_SEMA_COEX_INDEX = 6, GPS use M3 */
#define COS_SEMA_COEX_STA_ENTRY_FOR_GPS \
	CONN_SEMAPHORE_CONN_SEMA06_M3_OWN_STA_CONN_SEMA06_M3_OWN_STA

#define COS_SEMA_COEX_REL_ENTRY_FOR_GPS \
	CONN_SEMAPHORE_CONN_SEMA06_M3_OWN_REL_CONN_SEMA06_M3_OWN_REL
#endif

/* COS_SEMA_RFSPI_INDEX = 12, GPS use M3 */
#define COS_SEMA_RFSPI_STA_ENTRY_FOR_GPS \
	CONN_SEMAPHORE_CONN_SEMA12_M3_OWN_STA_CONN_SEMA12_M3_OWN_STA

#define COS_SEMA_RFSPI_REL_ENTRY_FOR_GPS \
	CONN_SEMAPHORE_CONN_SEMA12_M3_OWN_REL_CONN_SEMA12_M3_OWN_REL


#endif /* _GPS_DL_HW_DEP_MACRO_H */


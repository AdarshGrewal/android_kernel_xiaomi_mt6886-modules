/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

/*
 * The header is provided for connfem kernel module internal used.
 */

#ifndef __CONNFEM_CFG_H__
#define __CONNFEM_CFG_H__

#include "connfem_api.h"

/*******************************************************************************
 *				M A C R O S
 ******************************************************************************/

/*******************************************************************************
 *			    D A T A   T Y P E S
 ******************************************************************************/
struct cfm_config {
	struct connfem_epaelna_flags_bt cfm_cfg_flags_bt;
	struct connfem_epaelna_flags_common cfm_cfg_flags_cm;
};

/*******************************************************************************
 *			    P U B L I C   D A T A
 ******************************************************************************/


/*******************************************************************************
 *			      F U N C T I O N S
 ******************************************************************************/
extern void cfm_cfg_process(char *filename);

#endif /* __CONNFEM_H__ */

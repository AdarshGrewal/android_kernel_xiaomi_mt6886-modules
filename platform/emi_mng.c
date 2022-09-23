/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */
/*! \file
*    \brief  Declaration of library functions
*
*    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/

#include <linux/version.h>
#include <linux/of_reserved_mem.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/of.h>
#include "osal.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
#include <soc/mediatek/emi.h>
#else
#include <memory/mediatek/emi.h>
#endif

#include "emi_mng.h"

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))
#define ALLOCATE_CONNSYS_EMI_FROM_KO 1
#endif

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
#ifdef ALLOCATE_CONNSYS_EMI_FROM_KO
phys_addr_t gConEmiPhyBase;
EXPORT_SYMBOL(gConEmiPhyBase);
unsigned long long gConEmiSize;
EXPORT_SYMBOL(gConEmiSize);
#else
extern unsigned long long gConEmiSize;
extern phys_addr_t gConEmiPhyBase;
#endif

const struct consys_platform_emi_ops* consys_platform_emi_ops = NULL;

struct consys_emi_addr_info connsys_emi_addr_info = {
	.emi_ap_phy_addr = 0,
	.emi_size = 0,
	.md_emi_phy_addr = 0,
	.md_emi_size = 0,
};

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#if 0
/* Define weak function in case emi mpu ko is not inserted */
#if IS_ENABLED(CONFIG_MEDIATEK_EMI) || IS_ENABLED(CONFIG_MTK_EMI)
__weak int mtk_emimpu_init_region(struct emimpu_region_t *rg_info, unsigned int rg_num)
{
	pr_info("%s is weak function\n", __func__);
	return 0;
}

__weak int mtk_emimpu_set_addr(struct emimpu_region_t *rg_info,
	unsigned long long start, unsigned long long end)
{
	pr_info("%s is weak function\n", __func__);
	return 0;
}

__weak int mtk_emimpu_set_apc(struct emimpu_region_t *rg_info,
	unsigned int d_num, unsigned int apc)
{
	pr_info("%s is weak function\n", __func__);
	return 0;
}

__weak int mtk_emimpu_set_protection(struct emimpu_region_t *rg_info)
{
	pr_info("%s is weak function\n", __func__);
	return 0;
}

__weak int mtk_emimpu_free_region(struct emimpu_region_t *rg_info)
{
	pr_info("%s is weak function\n", __func__);
	return 0;
}
#endif
#endif

int emi_mng_set_region_protection(void)
{
	if (consys_platform_emi_ops &&
		consys_platform_emi_ops->consys_ic_emi_mpu_set_region_protection)
		return consys_platform_emi_ops->consys_ic_emi_mpu_set_region_protection();
	return -1;
}

int emi_mng_set_remapping_reg(void)
{
	if (consys_platform_emi_ops &&
		consys_platform_emi_ops->consys_ic_emi_set_remapping_reg)
		return consys_platform_emi_ops->consys_ic_emi_set_remapping_reg(
			connsys_emi_addr_info.emi_ap_phy_addr,
			connsys_emi_addr_info.md_emi_phy_addr);
	return -1;
}

struct consys_emi_addr_info* emi_mng_get_phy_addr(void)
{
	return &connsys_emi_addr_info;
}

#ifdef ALLOCATE_CONNSYS_EMI_FROM_KO
static int emi_mng_allocate_connsys_emi(struct platform_device *pdev)
{
	struct device_node *np;
	struct reserved_mem *rmem;

	np = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
	if (!np) {
		pr_info("no memory-region, np is NULL\n");
		return -1;
	}

	rmem = of_reserved_mem_lookup(np);
	of_node_put(np);

	if (!rmem) {
		pr_info("no memory-region\n");
		return -1;
	}

	gConEmiPhyBase = rmem->base;
	gConEmiSize = rmem->size;

	return 0;
}
#endif


int emi_mng_init(struct platform_device *pdev, const struct conninfra_plat_data* plat_data)
{
#ifdef ALLOCATE_CONNSYS_EMI_FROM_KO
	emi_mng_allocate_connsys_emi(pdev);
#endif
	if (consys_platform_emi_ops == NULL)
		consys_platform_emi_ops = (const struct consys_platform_emi_ops*)plat_data->platform_emi_ops;

	pr_info("[emi_mng_init] gConEmiPhyBase = [0x%llx] size = [%llx] ops=[%p]",
			gConEmiPhyBase, gConEmiSize, consys_platform_emi_ops);

	if (gConEmiPhyBase) {
		connsys_emi_addr_info.emi_ap_phy_addr = gConEmiPhyBase;
		connsys_emi_addr_info.emi_size = gConEmiSize;
	} else {
		pr_err("consys emi memory address gConEmiPhyBase invalid\n");
	}

	if (consys_platform_emi_ops &&
		consys_platform_emi_ops->consys_ic_emi_get_md_shared_emi)
		consys_platform_emi_ops->consys_ic_emi_get_md_shared_emi(
			&connsys_emi_addr_info.md_emi_phy_addr,
			&connsys_emi_addr_info.md_emi_size);

	if (consys_platform_emi_ops &&
		consys_platform_emi_ops->consys_ic_emi_mpu_set_region_protection)
		consys_platform_emi_ops->consys_ic_emi_mpu_set_region_protection();

	return 0;
}

int emi_mng_deinit(void)
{
	consys_platform_emi_ops = NULL;
	return 0;
}


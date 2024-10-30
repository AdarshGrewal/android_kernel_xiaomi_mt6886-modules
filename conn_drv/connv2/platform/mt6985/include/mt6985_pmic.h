/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#ifndef _PLATFORM_MT6985_PMIC_H_
#define _PLATFORM_MT6985_PMIC_H_

#define MT6363_BUCK_VS2_VOTER_CON0_SET_ADDR         0x149b
#define MT6363_BUCK_VS2_VOTER_CON0_CLR_ADDR         0x149c

#define MT6363_RG_LDO_VRFIO18_MON_ADDR              0x1bd1
#define MT6363_RG_LDO_VRFIO18_RC6_OP_EN_ADDR        0x1bd2
#define MT6363_RG_LDO_VRFIO18_RC6_OP_CFG_ADDR       0x1bd5
#define MT6363_RG_LDO_VRFIO18_RC6_OP_MODE_ADDR      0x1bd8
#define MT6363_RG_LDO_VRFIO18_RC7_OP_EN_ADDR        0x1bd2
#define MT6363_RG_LDO_VRFIO18_RC7_OP_CFG_ADDR       0x1bd5
#define MT6363_RG_LDO_VRFIO18_RC7_OP_MODE_ADDR      0x1bd8
#define MT6363_RG_LDO_VRFIO18_RC8_OP_EN_ADDR        0x1bd3
#define MT6363_RG_LDO_VRFIO18_RC8_OP_CFG_ADDR       0x1bd6
#define MT6363_RG_LDO_VRFIO18_RC8_OP_MODE_ADDR      0x1bd9
#define MT6363_RG_LDO_VRFIO18_RC9_OP_EN_ADDR        0x1bd3
#define MT6363_RG_LDO_VRFIO18_RC9_OP_CFG_ADDR       0x1bd6
#define MT6363_RG_LDO_VRFIO18_RC9_OP_MODE_ADDR      0x1bd9
#define MT6363_RG_LDO_VRFIO18_HW0_OP_EN_ADDR        0x1bd4
#define MT6363_RG_LDO_VRFIO18_HW0_OP_CFG_ADDR       0x1bd7
#define MT6363_RG_LDO_VRFIO18_HW0_OP_MODE_ADDR      0x1bda

#define MT6363_RG_LDO_VCN13_MON_ADDR                0x1d0b
#define MT6363_RG_LDO_VCN13_RC6_OP_EN_ADDR          0x1d14
#define MT6363_RG_LDO_VCN13_RC6_OP_CFG_ADDR         0x1d17
#define MT6363_RG_LDO_VCN13_RC6_OP_MODE_ADDR        0x1d1a
#define MT6363_RG_LDO_VCN13_RC7_OP_EN_ADDR          0x1d14
#define MT6363_RG_LDO_VCN13_RC7_OP_CFG_ADDR         0x1d17
#define MT6363_RG_LDO_VCN13_RC7_OP_MODE_ADDR        0x1d1a
#define MT6363_RG_LDO_VCN13_RC8_OP_EN_ADDR          0x1d15
#define MT6363_RG_LDO_VCN13_RC8_OP_CFG_ADDR         0x1d18
#define MT6363_RG_LDO_VCN13_RC8_OP_MODE_ADDR        0x1d1b
#define MT6363_RG_LDO_VCN13_RC9_OP_EN_ADDR          0x1d15
#define MT6363_RG_LDO_VCN13_RC9_OP_CFG_ADDR         0x1d18
#define MT6363_RG_LDO_VCN13_RC9_OP_MODE_ADDR        0x1d1b
#define MT6363_RG_LDO_VCN13_HW0_OP_EN_ADDR          0x1d16
#define MT6363_RG_LDO_VCN13_HW0_OP_CFG_ADDR         0x1d19
#define MT6363_RG_LDO_VCN13_HW0_OP_MODE_ADDR        0x1d1c

#define MT6373_RG_LDO_VCN33_1_MON_ADDR              0x1c0b
#define MT6373_RG_LDO_VCN33_1_RC7_OP_EN_ADDR        0x1c0c
#define MT6373_RG_LDO_VCN33_1_RC7_OP_CFG_ADDR       0x1c0f
#define MT6373_RG_LDO_VCN33_1_RC7_OP_MODE_ADDR      0x1c12
#define MT6373_RG_LDO_VCN33_1_RC8_OP_EN_ADDR        0x1c0d
#define MT6373_RG_LDO_VCN33_1_RC8_OP_CFG_ADDR       0x1c10
#define MT6373_RG_LDO_VCN33_1_RC8_OP_MODE_ADDR      0x1c13
#define MT6373_RG_LDO_VCN33_1_HW0_OP_EN_ADDR        0x1c0e
#define MT6373_RG_LDO_VCN33_1_HW0_OP_CFG_ADDR       0x1c11
#define MT6373_RG_LDO_VCN33_1_HW0_OP_MODE_ADDR      0x1c14

#define MT6373_RG_LDO_VCN33_2_MON_ADDR              0x1c19
#define MT6373_RG_LDO_VCN33_2_RC8_OP_EN_ADDR        0x1c1b
#define MT6373_RG_LDO_VCN33_2_RC8_OP_CFG_ADDR       0x1c1e
#define MT6373_RG_LDO_VCN33_2_RC8_OP_MODE_ADDR      0x1c21
#define MT6373_RG_LDO_VCN33_2_HW0_OP_EN_ADDR        0x1c1c
#define MT6373_RG_LDO_VCN33_2_HW0_OP_CFG_ADDR       0x1c1f
#define MT6373_RG_LDO_VCN33_2_HW0_OP_MODE_ADDR      0x1c22

#define MT6373_RG_LDO_VANT18_MON_ADDR               0x1c8b
#define MT6373_RG_LDO_VANT18_RC6_OP_EN_ADDR         0x1c8c
#define MT6373_RG_LDO_VANT18_RC6_OP_CFG_ADDR        0x1c8f
#define MT6373_RG_LDO_VANT18_RC6_OP_MODE_ADDR       0x1c92
#define MT6373_RG_LDO_VANT18_RC10_OP_EN_ADDR        0x1c8d
#define MT6373_RG_LDO_VANT18_RC10_OP_CFG_ADDR       0x1c90
#define MT6373_RG_LDO_VANT18_RC10_OP_MODE_ADDR      0x1c93
#define MT6373_RG_LDO_VANT18_HW0_OP_EN_ADDR         0x1c8e
#define MT6373_RG_LDO_VANT18_HW0_OP_CFG_ADDR        0x1c91
#define MT6373_RG_LDO_VANT18_HW0_OP_MODE_ADDR       0x1c94

#define MT6363_RG_VRFIO18_VOCAL_ADDR                0x1f2b

#endif /* _PLATFORM_MT6985_PMIC_H_ */

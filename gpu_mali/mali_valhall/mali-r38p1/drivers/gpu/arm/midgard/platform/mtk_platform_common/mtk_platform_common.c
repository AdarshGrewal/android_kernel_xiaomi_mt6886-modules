// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include <mali_kbase.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sysfs.h>
#include <platform/mtk_platform_common.h>
#include <mtk_gpufreq.h>
#include <ged_dvfs.h>

#if IS_ENABLED(CONFIG_MALI_MIDGARD_DVFS) && IS_ENABLED(CONFIG_MALI_MTK_DVFS_POLICY)
#include <platform/mtk_platform_common/mtk_platform_dvfs.h>
#endif /* CONFIG_MALI_MIDGARD_DVFS && CONFIG_MALI_MTK_DVFS_POLICY */

#if IS_ENABLED(CONFIG_PROC_FS)
#include <linux/proc_fs.h>
#endif /* CONFIG_PROC_FS */

#if IS_ENABLED(CONFIG_MALI_MTK_DEVFREQ_GOVERNOR)
#include <platform/mtk_platform_common/mtk_platform_devfreq_governor.h>
#endif /* CONFIG_MALI_MTK_DEVFREQ_GOVERNOR */

#if IS_ENABLED(CONFIG_MALI_MTK_DEBUG)
#include <platform/mtk_platform_common/mtk_platform_debug.h>
#endif /* CONFIG_MALI_MTK_DEBUG*/

#if IS_ENABLED(CONFIG_MALI_MTK_LOG_BUFFER)
#include <platform/mtk_platform_common/mtk_platform_logbuffer.h>
#endif /* CONFIG_MALI_MTK_LOG_BUFFER */

#if IS_ENABLED(CONFIG_PROC_FS)
/* name of the proc root dir */
#define	PROC_ROOT "mtk_mali"
static struct proc_dir_entry *proc_root;
#endif /* CONFIG_PROC_FS */

static bool mfg_powered;
static DEFINE_MUTEX(mfg_pm_lock);
static DEFINE_MUTEX(common_debug_lock);
static struct kbase_device *mali_kbdev;

struct kbase_device *mtk_common_get_kbdev(void)
{
	return mali_kbdev;
}

bool mtk_common_pm_is_mfg_active(void)
{
	return mfg_powered;
}

void mtk_common_pm_mfg_active(void)
{
	mutex_lock(&mfg_pm_lock);
	mfg_powered = true;
	mutex_unlock(&mfg_pm_lock);
}

void mtk_common_pm_mfg_idle(void)
{
	mutex_lock(&mfg_pm_lock);
	mfg_powered = false;
	mutex_unlock(&mfg_pm_lock);
}

void mtk_common_debug(enum mtk_common_debug_types type, int pid, u64 hook_point)
{
	struct kbase_device *kbdev = (struct kbase_device *)mtk_common_get_kbdev();

	if (IS_ERR_OR_NULL(kbdev))
		return;

	if (type == MTK_COMMON_DBG_DUMP_DB_BY_SETTING)
	{
		return;
	}

#if IS_ENABLED(CONFIG_MALI_MTK_DEBUG)
	lockdep_off();

	mutex_lock(&common_debug_lock);

	switch (type) {
	case MTK_COMMON_DBG_DUMP_INFRA_STATUS:
		if (!mtk_common_gpufreq_bringup() && kbdev->pm.backend.gpu_powered) {
#if defined(CONFIG_MTK_GPUFREQ_V2)
			gpufreq_dump_infra_status();
#else
			mt_gpufreq_dump_infra_status();
#endif /* CONFIG_MTK_GPUFREQ_V2 */
		}
		break;
	case MTK_COMMON_DBG_DUMP_PM_STATUS:
		mtk_debug_dump_pm_status(kbdev);
		break;
#if IS_ENABLED(CONFIG_MALI_CSF_SUPPORT)
	case MTK_COMMON_DBG_CSF_DUMP_GROUPS_QUEUES:
#if IS_ENABLED(CONFIG_MALI_MTK_FENCE_DEBUG)
		mtk_debug_csf_dump_groups_and_queues(kbdev, pid);
#endif /* CONFIG_MALI_MTK_FENCE_DEBUG */
		break;
#endif /* CONFIG_MALI_CSF_SUPPORT */
	case MTK_COMMON_DBG_DUMP_FULL_DB:
		break;
	case MTK_COMMON_DBG_TRIGGER_KERNEL_EXCEPTION:
		break;
	case MTK_COMMON_DBG_TRIGGER_WARN_ON:
		WARN_ON(1);
		break;
	case MTK_COMMON_DBG_TRIGGER_BUG_ON:
		BUG_ON(1);
		break;
	default:
		dev_info(kbdev->dev, "@%s: unsupported type (%d)", __func__, type);
		break;
	}

	mutex_unlock(&common_debug_lock);

	lockdep_on();
#else
	return;
#endif /* CONFIG_MALI_MTK_DEBUG */
}

int mtk_common_gpufreq_bringup(void)
{
	static int bringup = -1;

	if (bringup == -1) {
#if defined(CONFIG_MTK_GPUFREQ_V2)
		bringup = gpufreq_bringup();
#else
		bringup = mt_gpufreq_bringup();
#endif
	}

	return bringup;
}

int mtk_common_gpufreq_commit(int opp_idx)
{
	int ret = -1;

	mutex_lock(&mfg_pm_lock);
	if (opp_idx >= 0 && mtk_common_pm_is_mfg_active()) {
#if defined(CONFIG_MTK_GPUFREQ_V2)
		ret = mtk_common_gpufreq_bringup() ?
			-1 : gpufreq_commit(TARGET_DEFAULT, opp_idx);
#else
		ret = mtk_common_gpufreq_bringup() ?
			-1 : mt_gpufreq_target(opp_idx, KIR_POLICY);
#endif /* CONFIG_MTK_GPUFREQ_V2 */
	}
	mutex_unlock(&mfg_pm_lock);

	return ret;
}

int mtk_common_ged_dvfs_get_last_commit_idx(void)
{
#if IS_ENABLED(CONFIG_MALI_MIDGARD_DVFS) && IS_ENABLED(CONFIG_MALI_MTK_DVFS_POLICY)
	return (int)ged_dvfs_get_last_commit_idx();
#else
	return -1;
#endif
}

#if IS_ENABLED(CONFIG_PROC_FS)
static void mtk_common_procfs_init(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev))
		return;

  	proc_root = proc_mkdir(PROC_ROOT, NULL);
  	if (!proc_root) {
		dev_info(kbdev->dev, "@%s: Cann't create /proc/%s", __func__, PROC_ROOT);
  		return;
  	}

#if IS_ENABLED(CONFIG_MALI_MTK_LOG_BUFFER)
	mtk_logbuffer_procfs_init(kbdev, proc_root);
#endif /* CONFIG_MALI_MTK_LOG_BUFFER */

#if IS_ENABLED(CONFIG_MALI_MIDGARD_DVFS) && IS_ENABLED(CONFIG_MALI_MTK_DVFS_POLICY)
	mtk_dvfs_procfs_init(kbdev, proc_root);
#endif /* CONFIG_MALI_MIDGARD_DVFS && CONFIG_MALI_MTK_DVFS_POLICY */
}

static void mtk_common_procfs_term(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev))
		return;

#if IS_ENABLED(CONFIG_MALI_MIDGARD_DVFS) && IS_ENABLED(CONFIG_MALI_MTK_DVFS_POLICY)
	mtk_dvfs_procfs_term(kbdev, proc_root);
#endif /* CONFIG_MALI_MIDGARD_DVFS && CONFIG_MALI_MTK_DVFS_POLICY */

#if IS_ENABLED(CONFIG_MALI_MTK_LOG_BUFFER)
	mtk_logbuffer_procfs_term(kbdev, proc_root);
#endif /* CONFIG_MALI_MTK_LOG_BUFFER */

	proc_root = NULL;
	remove_proc_entry(PROC_ROOT, NULL);
}
#endif /* CONFIG_PROC_FS */

#if IS_ENABLED(CONFIG_MALI_MTK_DEBUG_FS)
void mtk_common_debugfs_init(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev))
		return;
}

#if IS_ENABLED(CONFIG_MALI_CSF_SUPPORT)
void mtk_common_csf_debugfs_init(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev))
		return;

#if IS_ENABLED(CONFIG_MALI_MTK_DEBUG)
	mtk_debug_csf_debugfs_init(kbdev);
#endif /* CONFIG_MALI_MTK_DEBUG */
}
#endif /* CONFIG_MALI_CSF_SUPPORT */
#endif /* CONFIG_MALI_MTK_DEBUG_FS */

int mtk_common_device_init(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev)) {
		dev_info(kbdev->dev, "@%s: invalid kbdev", __func__);
		return -1;
	}

	mali_kbdev = kbdev;

	if (mtk_platform_pm_init(kbdev)) {
		dev_info(kbdev->dev, "@%s: Failed to init Platform PM", __func__);
		return -1;
	}

#if IS_ENABLED(CONFIG_MALI_MTK_DEBUG)
	mtk_debug_init(kbdev);
#endif /* CONFIG_MALI_MTK_DEBUG */

#if IS_ENABLED(CONFIG_MALI_MTK_DEVFREQ_GOVERNOR)
	mtk_devfreq_governor_init(kbdev);
#endif /* CONFIG_MALI_MTK_DEVFREQ_GOVERNOR */

#if IS_ENABLED(CONFIG_MALI_MTK_LOG_BUFFER)
	mtk_logbuffer_init(kbdev);
#endif /* CONFIG_MALI_MTK_LOG_BUFFER */

#if IS_ENABLED(CONFIG_MALI_MIDGARD_DVFS) && IS_ENABLED(CONFIG_MALI_MTK_DVFS_POLICY)
	mtk_dvfs_init(kbdev);
#endif /* CONFIG_MALI_MIDGARD_DVFS && CONFIG_MALI_MTK_DVFS_POLICY */

#if IS_ENABLED(CONFIG_MALI_MTK_PROC_FS)
	mtk_common_procfs_init(kbdev);
#endif /* CONFIG_MALI_MTK_PROC_FS */

	return 0;
}

void mtk_common_device_term(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev)) {
		dev_info(kbdev->dev, "@%s: invalid kbdev", __func__);
		return;
	}

#if IS_ENABLED(CONFIG_MALI_MTK_DEBUG)
	mtk_debug_term(kbdev);
#endif /* CONFIG_MALI_MTK_DEBUG */

#if IS_ENABLED(CONFIG_MALI_MTK_DEVFREQ_GOVERNOR)
	mtk_devfreq_governor_term(kbdev);
#endif /* CONFIG_MALI_MTK_DEVFREQ_GOVERNOR */

#if IS_ENABLED(CONFIG_MALI_MTK_LOG_BUFFER)
	mtk_logbuffer_term(kbdev);
#endif /* CONFIG_MALI_MTK_LOG_BUFFER */

#if IS_ENABLED(CONFIG_MALI_MIDGARD_DVFS) && IS_ENABLED(CONFIG_MALI_MTK_DVFS_POLICY)
	mtk_dvfs_term(kbdev);
#endif /* CONFIG_MALI_MIDGARD_DVFS && CONFIG_MALI_MTK_DVFS_POLICY */

#if IS_ENABLED(CONFIG_MALI_MTK_PROC_FS)
	mtk_common_procfs_term(kbdev);
#endif /* CONFIG_MALI_MTK_PROC_FS */

	mtk_platform_pm_term(kbdev);
}

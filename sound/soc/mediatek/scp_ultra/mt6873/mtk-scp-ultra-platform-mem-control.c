// SPDX-License-Identifier: GPL-2.0
//
// Copyright (C) 2018 MediaTek Inc.

#include "mt6873-afe-common.h"
#include "mtk-scp-ultra-platform-mem-control.h"
#include "mtk-scp-ultra-common.h"
#include "mt6873-afe-clk.h"

const int get_scp_ultra_memif_id(int scp_ultra_task_id)
{
	switch (scp_ultra_task_id) {
	case SCP_ULTRA_DL_DAI_ID:
		return MT6873_MEMIF_DL7;
	case SCP_ULTRA_UL_DAI_ID:
		return MT6873_MEMIF_VUL3;
	default:
		pr_info("%s(), error return -1\n", __func__);
		return -1;
	}
}

const int set_afe_clock(bool enable, struct mtk_base_afe *afe)
{
	if (enable)
		mt6873_afe_enable_clock(afe);
	else
		mt6873_afe_disable_clock(afe);
	return 0;
}

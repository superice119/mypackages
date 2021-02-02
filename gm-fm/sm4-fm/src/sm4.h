/* SPDX-License-Identifier: GPL-2.0 */

/*
 *  * Common values for the SM4 algorithm
 *   * Copyright (C) 2018 ARM Limited or its affiliates.
 *    */

#ifndef _CRYPTO_SM4_H
#define _CRYPTO_SM4_H

#include <linux/types.h>
#include <linux/crypto.h>

#include "fm_def.h"
#include "fm_cpc_kpub.h"

#define SM4_KEY_SIZE	16
#define SM4_BLOCK_SIZE	16
#define SM4_RKEY_WORDS	32

struct crypto_sm4_ctx {
	//u32 rkey_enc[SM4_RKEY_WORDS];
	//u32 rkey_dec[SM4_RKEY_WORDS];

	u8 key_enc[SM4_KEY_SIZE];
	u8 key_dec[SM4_KEY_SIZE];
	FM_HANDLE hDev;
};


#endif

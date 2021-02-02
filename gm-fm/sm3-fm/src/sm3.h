/*
 * Common values for SM3 algorithm
 */

#ifndef _CRYPTO_SM3_H
#define _CRYPTO_SM3_H

#include <linux/types.h>

#include "fm_def.h"
#include "fm_cpc_kpub.h"

#define SM3_DIGEST_SIZE	32
#define SM3_BLOCK_SIZE	64

struct sm3_state {
	u32 state[SM3_DIGEST_SIZE / 4];
	u64 count;
	u8 buffer[SM3_BLOCK_SIZE];
};

struct shash_sm3_ctx {
	FM_HANDLE hDev;
	FM_CPC_SM3_CTX SM3Ctx;
};

//struct shash_desc sm3_ctx;
struct shash_sm3_ctx sm3_ctx;

#endif

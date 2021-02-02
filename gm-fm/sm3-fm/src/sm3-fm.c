/*
 * SM3 secure hash, as specified by OSCCA GM/T 0004-2012 SM3 and
 * described at https://tools.ietf.org/html/draft-shen-sm3-hash-01
 *
 * Copyright (C) 2017 ARM Limited or its affiliates.
 * Written by Gilad Ben-Yossef <gilad@benyossef.com>
      *
 * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
  *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License
   * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <crypto/internal/hash.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>

#include "sm3.h"

static int sm3_init(struct shash_desc *desc)
{
	struct shash_sm3_ctx *ctx = shash_desc_ctx(desc);

	FM_U32 u32Ret = 0;
#if 0
	unsigned char    pID[64];
	unsigned int     uiIDLen = 0;
	FM_ECC_PublicKey pubEcc;

	memset(pID, '0', 64);
	memset(&pubEcc, 0, sizeof(FM_ECC_PublicKey));

	//u32Ret = FMK_SM3Init(ctx->hDev, NULL, NULL, 0, &ctx->SM3Ctx);
	//u32Ret = FMK_SM3Init(ctx->hDev, &pubEcc, pID, uiIDLen, &ctx->SM3Ctx);
	//u32Ret = FMK_SM3Init(ctx->hDev, NULL, NULL, 0);
#endif
	u32Ret = FMK_SM3Init(sm3_ctx.hDev, NULL, NULL, 0, &sm3_ctx.SM3Ctx);
	if (FME_OK != u32Ret)
	{
		printk("FMK_SM3Init error, u32Ret=0x%08x\n", u32Ret);
		return u32Ret;
	}

	return 0;
}

int sm3_update(struct shash_desc *desc, const u8 *data, unsigned int len)
{
	struct shash_sm3_ctx *ctx = shash_desc_ctx(desc);

	FM_U32 u32Ret = 0;

	//FM_CPC_SM3_CTX SM3Ctx = {0};

	//u32Ret = FMK_SM3Update(ctx->hDev, data, len, &ctx->SM3Ctx);
	u32Ret = FMK_SM3Update(sm3_ctx.hDev, data, len, &sm3_ctx.SM3Ctx);
	if (FME_OK != u32Ret)
	{
		printk("FMK_SM3Update error, u32Ret=0x%08x\n", u32Ret);
		return u32Ret;
	}
	return u32Ret;

	//return sm3_base_do_update(desc, data, len, sm3_generic_block_fn);
}

static int sm3_final(struct shash_desc *desc, u8 *out)
{
	//sm3_base_do_finalize(desc, sm3_generic_block_fn);
	//return sm3_base_finish(desc, out);

	struct shash_sm3_ctx *ctx = shash_desc_ctx(desc);

	FM_U32 u32Ret = 0;

	//FM_U8  au8HashResult[32];
	FM_U32 u32ResultLen = 0;
	//FM_CPC_SM3_CTX SM3Ctx = {0};

	u32ResultLen = 32;
	u32Ret = FMK_SM3Final(sm3_ctx.hDev, out, &u32ResultLen, &sm3_ctx.SM3Ctx);
	if (FME_OK != u32Ret)
	{
		printk("FMK_SM3Final error, u32Ret=0x%08x\n", u32Ret);
		return u32Ret;
	}
	return u32Ret;
}

static int _vpn_sm3_init(struct crypto_tfm *tfm)
{
	struct shash_sm3_ctx *ctx =  crypto_tfm_ctx(tfm);
	int ret = 0;

	FM_U32 u32Ret = 0;
	FM_U32 DevID = 0;// default dev id
	FM_U32 u32DevType = FM_DEV_TYPE_MINPCIE_1_0X; //default type mini card

	//open device
	printk("fm 3 Open device !! \n");

	//u32Ret = FMK_OpenDevice(DevID, u32DevType, FM_OPEN_SCH | FM_OPEN_SHARE , &ctx->hDev);
	u32Ret = FMK_OpenDevice(DevID, u32DevType, FM_OPEN_SCH | FM_OPEN_SHARE , &sm3_ctx.hDev);
	if (FME_OK != u32Ret)
	{
		printk("FM_GenRandom error, u32Ret=0x%08x\n", u32Ret);
		return (int)u32Ret;
	}

	return ret;
}

static void _vpn_sm3_cleanup(struct crypto_tfm *tfm)
{
	struct shash_sm3_ctx *ctx =  crypto_tfm_ctx(tfm);
	FM_U32 u32Ret = 0;

	printk("fm 3 close device !! \n");
	//u32Ret = FMK_CloseDevice(ctx->hDev);
	u32Ret = FMK_CloseDevice(sm3_ctx.hDev);
	if (FME_OK != u32Ret)
	{
		printk("FMK_CloseDevice error, u32Ret=0x%08x\n", u32Ret);
	}

}

int sm3_finup(struct shash_desc *desc, const u8 *data,
		unsigned int len, u8 *out)
{
#if DEBUG
	printk(" function: %s ,  line= %d \n", __FUNCTION__, __LINE__);
#endif

	sm3_init(desc);
	sm3_update(desc, data, len);
	return sm3_final(desc, out);
}

static struct shash_alg sm3_alg = {

	.init		=	sm3_init,
	.update		=	sm3_update,
	.final		=	sm3_final,
	.finup		=	sm3_finup,
	.descsize	=	sizeof(struct shash_sm3_ctx),
	.digestsize	=	SM3_DIGEST_SIZE,
	.base		=	{
		.cra_name	 =	"sm3",
		.cra_driver_name =	"fm-sm3",
		.cra_flags	 =	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	 =	SM3_BLOCK_SIZE,
		.cra_module	 =	THIS_MODULE,

		//.cra_ctxsize		= sizeof(struct shash_sm3_ctx),
		//.cra_alignmask		= SHA_ALIGN_MSK,

		.cra_init		= _vpn_sm3_init,
		.cra_exit		= _vpn_sm3_cleanup,
	}
};

static int __init sm3_generic_mod_init(void)
{
	printk("wq3 %s \n", __FUNCTION__);
	return crypto_register_shash(&sm3_alg);
}

static void __exit sm3_generic_mod_fini(void)
{
	printk("wq3 %s \n", __FUNCTION__);
	crypto_unregister_shash(&sm3_alg);
}

module_init(sm3_generic_mod_init);
module_exit(sm3_generic_mod_fini);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("SM3 Secure Hash Algorithm");

MODULE_ALIAS_CRYPTO("sm3");
MODULE_ALIAS_CRYPTO("sm3-generic");


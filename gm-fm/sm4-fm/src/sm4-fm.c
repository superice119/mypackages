
/*
 *  * SM4 Cipher Algorithm.
 *   *
 *    * Copyright (C) 2018 ARM Limited or its affiliates.
 *     * All rights reserved.
 *      */

//#include <crypto/sm4.h>
#include "sm4.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/crypto.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>


#define DEBUG

/**
 *  * crypto_sm4_set_key - Set the AES key.
 *   * @tfm:	The %crypto_tfm that is used in the context.
 *    * @in_key:	The input key.
 *     * @key_len:	The size of the key.
 *      *
 *       * Returns 0 on success, on failure the %CRYPTO_TFM_RES_BAD_KEY_LEN flag in tfm
 *        * is set. The function uses crypto_sm4_expand_key() to expand the key.
 *         * &crypto_sm4_ctx _must_ be the private data embedded in @tfm which is
 *          * retrieved with crypto_tfm_ctx().
 *           */
int crypto_sm4_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		unsigned int key_len)
{
	struct crypto_sm4_ctx *ctx = crypto_tfm_ctx(tfm);

	u32 *flags = &tfm->crt_flags;

	//int ret;

	memcpy(ctx->key_enc, in_key, SM4_KEY_SIZE);
	memcpy(ctx->key_dec, in_key, SM4_KEY_SIZE);

	*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;

	//return -EINVAL;
	return 0;
}
EXPORT_SYMBOL_GPL(crypto_sm4_set_key);


/* encrypt a block of text */
static void sm4_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct crypto_sm4_ctx *ctx = crypto_tfm_ctx(tfm);

	FM_U32 u32Ret = 0;
	FM_U32 u32OutLen = 0;
	//FM_U8 au8IV[SM4_KEY_SIZE];

	u32Ret = FMK_Encrypt(ctx->hDev, FM_HKEY_FROM_HOST,  FM_ALG_SM4, FM_ALGMODE_ECB, in,
			SM4_BLOCK_SIZE, out, &u32OutLen, ctx->key_enc, SM4_KEY_SIZE,
			NULL, 0);
	if (u32Ret != FME_OK)
	{
		printk("FM_CPC_Encrypt error, u32Ret = 0x%08x\n", u32Ret);
	}

}

/* decrypt a block of text */

static void sm4_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct crypto_sm4_ctx *ctx = crypto_tfm_ctx(tfm);

	FM_U32 u32Ret = 0;
	FM_U32 u32OutLen = 0;
	//FM_U8 au8IV[SM4_KEY_SIZE];

	u32Ret = FMK_Decrypt(ctx->hDev, FM_HKEY_FROM_HOST,  FM_ALG_SM4, FM_ALGMODE_ECB, in,
			SM4_BLOCK_SIZE, out, &u32OutLen, ctx->key_enc, SM4_KEY_SIZE,
			NULL, 0);
	if (u32Ret != FME_OK)
	{
		printk("FM_CPC_Encrypt error, u32Ret = 0x%08x\n", u32Ret);
	}

}

static int _vpn_sm4_init(struct crypto_tfm *tfm)
{
	struct crypto_sm4_ctx *ctx = crypto_tfm_ctx(tfm);
	int ret = 0;

	FM_U32 u32Ret = 0;
	FM_U32 DevID = 0;// default dev id
	FM_U32 u32DevType = FM_DEV_TYPE_MINPCIE_1_0X; //default type mini card

	//open device
	printk("fm 4 Open device !! \n");
	u32Ret = FMK_OpenDevice(DevID, u32DevType, FM_OPEN_SCH | FM_OPEN_SHARE , &ctx->hDev);
	if (FME_OK != u32Ret)
	{
		printk("FM_GenRandom error, u32Ret=0x%08x\n", u32Ret);
		return (int)u32Ret;
	}

	return ret;
}

static void _vpn_sm4_cleanup(struct crypto_tfm *tfm)
{
	const struct crypto_sm4_ctx *ctx = crypto_tfm_ctx(tfm);
	FM_U32 u32Ret = 0;

	printk("fm 4 close device !! \n");
	u32Ret = FMK_CloseDevice(ctx->hDev);
	if (FME_OK != u32Ret)
	{
		printk("FM_CPC_CloseDevice error, u32Ret=0x%08x\n", u32Ret);
	}

}

static struct crypto_alg sm4_alg = {
	.cra_name		=	"sm4",
	.cra_driver_name	=	"fm-sm4",
	.cra_priority		=	100,
	.cra_flags		=	CRYPTO_ALG_TYPE_CIPHER,
	.cra_blocksize		=	SM4_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct crypto_sm4_ctx),
	.cra_module		=	THIS_MODULE,

	.cra_init               = 	_vpn_sm4_init,
	.cra_exit               = 	_vpn_sm4_cleanup,

	.cra_u			=	{
		.cipher = {
			.cia_min_keysize	=	SM4_KEY_SIZE,
			.cia_max_keysize	=	SM4_KEY_SIZE,
			.cia_setkey		=	crypto_sm4_set_key,
			.cia_encrypt		=	sm4_encrypt,
			.cia_decrypt		=	sm4_decrypt
		}
	}
};

static int __init sm4_init(void)
{
	printk("fm cpc sm4 %s \n", __FUNCTION__);
	return crypto_register_alg(&sm4_alg);
}

static void __exit sm4_fini(void)
{
	printk("fm cpc sm4 %s \n", __FUNCTION__);
	crypto_unregister_alg(&sm4_alg);
}

module_init(sm4_init);
module_exit(sm4_fini);

MODULE_DESCRIPTION("SM4 Cipher Algorithm");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS_CRYPTO("sm4");
MODULE_ALIAS_CRYPTO("sm4-generic");


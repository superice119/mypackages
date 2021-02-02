#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fips.h>
#include <linux/time.h>
#include <linux/random.h>
#include <linux/crypto.h>
#include <crypto/internal/rng.h>
#include <crypto/rng.h>

#include "fm_def.h"
#include "fm_cpc_kpub.h"

//#define DEBUG

struct vpn_rng_ctx {
	FM_HANDLE hDev;
	spinlock_t vpn_rng_lock;
};

static int vpn_rng_get_random(struct crypto_rng *tfm,
			     const u8 *src, unsigned int slen,
			     u8 *rdata, unsigned int dlen)
{
	struct vpn_rng_ctx *rng = crypto_rng_ctx(tfm);
	int ret = 0;
	FM_U32 u32Ret = 0;

	spin_lock(&rng->vpn_rng_lock);
#ifdef DEBUG
	get_random_bytes(rdata, dlen);
#else
	printk("fm generate random !!\n");
	u32Ret = FMK_GenRandom(rng->hDev, dlen/*1 ~ 16k*/, rdata);

	if (FME_OK != u32Ret)
	{
		printk("FM_GenRandom error, u32Ret=0x%08x\n", u32Ret);
		return (int)u32Ret;
	}
#endif
	spin_unlock(&rng->vpn_rng_lock);

	return ret;
}

static int _vpn_rng_init(struct crypto_tfm *tfm)
{
	struct vpn_rng_ctx *rng = crypto_tfm_ctx(tfm);
	int ret = 0;

#ifndef DEBUG
	FM_U32 u32Ret = 0;
	FM_U32 DevID = 0;// default dev id
	FM_U32 u32DevType = FM_DEV_TYPE_MINPCIE_1_0X; //default type mini card

	//open device
	printk("fm Open device !! \n");
	u32Ret = FMK_OpenDevice(DevID, u32DevType, FM_OPEN_SCH | FM_OPEN_SHARE , &rng->hDev);
	if (FME_OK != u32Ret)
	{
		printk("FM_GenRandom error, u32Ret=0x%08x\n", u32Ret);
		return (int)u32Ret;
	}

#endif
	spin_lock_init(&rng->vpn_rng_lock);

	return ret;
}

static void _vpn_rng_cleanup(struct crypto_tfm *tfm)
{
	struct vpn_rng_ctx *rng = crypto_tfm_ctx(tfm);
	FM_U32 u32Ret = 0;

	spin_lock(&rng->vpn_rng_lock);

#ifndef DEBUG
	printk("fm close device !! \n");
	u32Ret = FMK_CloseDevice(rng->hDev);
	if (FME_OK != u32Ret)
	{
		printk("FM_CPC_CloseDevice error, u32Ret=0x%08x\n", u32Ret);
	}

#endif
	spin_unlock(&rng->vpn_rng_lock);
}

static int _vpn_rng_reset(struct crypto_rng *tfm,
			    const u8 *seed, unsigned int slen)
{
	return 0;
}

static struct rng_alg vpn_rng_alg = {
	.generate		= vpn_rng_get_random,
	.seed			= _vpn_rng_reset,
	.seedsize		= 0,
	.base			= {
		.cra_name               = "vpnrng",
		.cra_driver_name        = "vpnrng",
		.cra_priority           = 100,
		.cra_ctxsize            = sizeof(struct vpn_rng_ctx),
		.cra_module             = THIS_MODULE,
		.cra_init               = _vpn_rng_init,
		.cra_exit               = _vpn_rng_cleanup,

	}
};

int vpn_rng_init(void)
{
	int ret;

	printk("rng fm init \n");

	ret = crypto_register_rng(&vpn_rng_alg);
	if (ret)
	{
		printk("rng fm init fail !! \n");
	}

	return ret;
}

void vpn_rng_exit(void)
{
	printk("rng fm exit \n");

	crypto_unregister_rng(&vpn_rng_alg);
}

module_init(vpn_rng_init);
module_exit(vpn_rng_exit);

MODULE_DESCRIPTION("random Algorithm");
MODULE_LICENSE("GPL v2");


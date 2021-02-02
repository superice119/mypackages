/******************************************************************************
* Copyright (c) 2014, Shandong Fisherman Information Technology Co., Ltd.
* All rights reserved.
* 文件名称: fm_cpc_kpub.h
* 文件描述: Fisherman 加密设备(Crypto PCI Card, CPC)内核态公共函数接口
* 当前版本: 1.0.90
* 作    者:
* 创建时间: 2014-1-2
* 修改记录:
* ----------------------------------------------
*    时间    作者     描述
******************************************************************************/
FM_U32 FMK_OpenDevice
(
    FM_I FM_U32     u32Index,
    FM_I FM_U32     u32Type,
    FM_I FM_U32     u32Flag,
    FM_O FM_HANDLE *phDev
);
FM_U32 FMK_CloseDevice
(
    FM_I FM_HANDLE  hDev
);
FM_U32 FMK_GenRandom
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Len,
    FM_O FM_U8     *pu8Random
);
FM_U32 FMK_ImportKey
(
    FM_I FM_HANDLE hDev,
    FM_I FM_U32    u32Alg,
    FM_I FM_U8    *pu8Key,
    FM_I FM_U32    u32KeyLen,
    FM_B FM_HKEY  *phKey
);
FM_U32 FMK_ExportKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_O FM_U8     *pu8Key,
    FM_B FM_U32    *pu32Len
);
FM_U32 FMK_GenKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_U32     u32InLen,
    FM_B FM_HKEY   *phKey,
    FM_O FM_U8     *pu8Key
);
FM_U32 FMK_DelKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey
);
FM_U32 FMK_Encrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U32     u32Alg,
    FM_I FM_U32     u32WorkMode,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen,
    FM_I FM_U8     *pu8Key,
    FM_I FM_U32     u32KeyLen,
    FM_I FM_U8     *pu8IV,
    FM_I FM_U32     u32IVLen
);
FM_U32 FMK_Decrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U32     u32Alg,
    FM_I FM_U32     u32WorkMode,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen,
    FM_I FM_U8     *pu8Key,
    FM_I FM_U32     u32KeyLen,
    FM_I FM_U8     *pu8IV,
    FM_I FM_U32     u32IVLen
);
FM_U32 FMK_GenECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_B FM_HKEY   *phKey,
    FM_O FM_ECC_PublicKey  *pPubkey,
    FM_O FM_ECC_PrivateKey *pPrikey
);
FM_U32 FMK_DelECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey
);
FM_U32 FMK_ImportECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_B FM_HKEY   *phKey,
    FM_I FM_ECC_PublicKey  *pPubkey,
    FM_I FM_ECC_PrivateKey *pPrikey
);
FM_U32 FMK_ExportECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_O FM_ECC_PublicKey  *pPubkey,
    FM_O FM_ECC_PrivateKey *pPrikey
);
FM_U32 FMK_ECCEncrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_HKEY    hKey,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_I FM_ECC_PublicKey *pPubkey,
    FM_O FM_ECC_Cipher    *pECCCipher
);
FM_U32 FMK_ECCDecrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_HKEY    hKey,
    FM_I FM_ECC_Cipher     *pECCCipher,
    FM_I FM_ECC_PrivateKey *pPrikey,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
FM_U32 FMK_ECCSign
(
    FM_I FM_HANDLE             hDev,
    FM_I FM_U32                u32Alg,
    FM_I FM_HKEY               hKey,
    FM_I FM_U8                *pu8InBuf,
    FM_I FM_U32                u32InLen,
    FM_I FM_ECC_PrivateKey    *pPrikey,
    FM_O FM_ECC_Signature     *pSignature
);
FM_U32 FMK_ECCVerify
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_U32            u32Alg,
    FM_I FM_HKEY           hKey,
    FM_I FM_ECC_PublicKey *pPubkey,
    FM_I FM_U8            *pu8InBuf,
    FM_I FM_U32            u32InLen,
    FM_I FM_ECC_Signature *pSignature
);
FM_U32 FMK_GenerateAgreementDataWithECC
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_U32            u32Alg,
    FM_I FM_HKEY           hKey,
    FM_I FM_U32            u32KeyBits,
    FM_I FM_U8            *pu8SponsorID,
    FM_I FM_U32            u32SponsorIDLen,
    FM_O FM_ECC_PublicKey *pSponsorPubKey,
    FM_O FM_ECC_PublicKey *pSponsorTmpPubKey,
    FM_O FM_HANDLE        *phAgreementHandle
);
FM_U32 FMK_GenerateAgreementDataAndKeyWithECC
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_U32            u32Alg,
    FM_I FM_HKEY           hKey,
    FM_I FM_U32            u32KeyBits,
    FM_I FM_U8            *pu8ResponseID,
    FM_I FM_U32            u32ResponseIDLen,
    FM_I FM_U8            *pu8SponsorID,
    FM_I FM_U32            u32SponsorIDLen,
    FM_I FM_ECC_PublicKey *pSponsorPubKey,
    FM_I FM_ECC_PublicKey *pSponsorTmpPubKey,
    FM_O FM_ECC_PublicKey *pResponsePubKey,
    FM_O FM_ECC_PublicKey *pResponseTmpPubKey,
    FM_O FM_HKEY          *phKeyHandle
);
FM_U32 FMK_GenerateKeyWithECC
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_U32            u32Alg,
    FM_I FM_U8            *pu8ResponseID,
    FM_I FM_U32            u32ResponseIDLen,
    FM_I FM_ECC_PublicKey *pResponsePubKey,
    FM_I FM_ECC_PublicKey *pResponseTmpPubKey,
    FM_I FM_HANDLE        *phAgreementHandle,
    FM_O FM_HKEY          *phKeyHandle
);
FM_U32 FMK_GenRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32KeyBits,
    FM_B FM_HKEY   *phKey,
    FM_O FM_RSA_PublicKey  *pPubkey,
    FM_O FM_RSA_PrivateKey *pPrikey
);
FM_U32 FMK_DelRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey
);
FM_U32 FMK_ImportRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_B FM_HKEY   *phKey,
    FM_I FM_RSA_PublicKey  *pPubkey,
    FM_I FM_RSA_PrivateKey *pPrikey
);
FM_U32 FMK_ExportRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_O FM_RSA_PublicKey  *pPubkey,
    FM_O FM_RSA_PrivateKey *pPrikey
);
FM_U32 FMK_RSAEncrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen,
    FM_I FM_RSA_PublicKey *pPubkey
);
FM_U32 FMK_RSADecrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen,
    FM_I FM_RSA_PrivateKey *pPrikey
);
FM_U32 FMK_SHA1Init
(
    FM_I FM_HANDLE          hDev,
    FM_O PFM_CPC_SHA1_CTX   pSHACtx
);
FM_U32 FMK_SHA1Update
(
    FM_I FM_HANDLE           hDev,
    FM_I FM_U8              *pu8InBuf,
    FM_I FM_U32              u32InLen,
    FM_B PFM_CPC_SHA1_CTX    pSHACtx
);
FM_U32 FMK_SHA1Final
(
    FM_I FM_HANDLE           hDev,
    FM_O FM_U8              *pu8HashBuf,
    FM_O FM_U32             *pu32HashLen,
    FM_I PFM_CPC_SHA1_CTX    pSHACtx
);
FM_U32 FMK_SM3Init
(
    FM_I FM_HANDLE          hDev,
    FM_I FM_ECC_PublicKey  *pPubkey,
    FM_I FM_U8             *pu8ID,
    FM_I FM_U32             u32IDLen,
    FM_O PFM_CPC_SM3_CTX    pSM3Ctx
);
FM_U32 FMK_SM3Update
(
    FM_I FM_HANDLE          hDev,
    FM_I FM_U8             *pu8InBuf,
    FM_I FM_U32             u32InLen,
    FM_B PFM_CPC_SM3_CTX    pSM3Ctx
);
FM_U32 FMK_SM3Final
(
    FM_I FM_HANDLE          hDev,
    FM_O FM_U8             *pu8HashBuf,
    FM_O FM_U32            *pu32HashLen,
    FM_I PFM_CPC_SM3_CTX    pSM3Ctx
);

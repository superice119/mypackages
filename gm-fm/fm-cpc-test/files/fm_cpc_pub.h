/******************************************************************************
* Copyright (c) 2008, Shandong Fisherman Information Technology Co., Ltd.
* All rights reserved.
* 文件名称: fm_cpc_pub.h
* 文件描述: Fisherman 加密设备(Crypto PCI Card, CPC)公共函数接口
* 当前版本: 1.0
* 作    者:
* 创建时间: 2008-08-21
* 修改记录:
* ----------------------------------------------
*    时间    作者     描述
******************************************************************************/
#ifndef FM_CPC_PUB_H
#define FM_CPC_PUB_H

#ifdef  __cplusplus
    extern "C" {
#endif

#define FM_SERVER_API_VERSION "1.0.3.0"

// 卡类型定义
#define FM_DEV_TYPE_PCI_1_2X      0x0120    //pci  V1.2
#define FM_DEV_TYPE_PCI_2_0X      0x0200    //pci  V2.0
#define FM_DEV_TYPE_PCIE_1_0X     0xE100    //pcie V1.0
#define FM_DEV_TYPE_PCIE_2_0X     0xE200    //pcie V2.0
#define FM_DEV_TYPE_PCIE_3_0X     0xE200    //pcie V3.0
#define FM_DEV_TYPE_PCIE_4_0X     0xE400    //pcie V4.0
#define FM_DEV_TYPE_PCIE_5_0X     0xE600    //pcie V5.0
#define FM_DEV_TYPE_YIBU          0xE601    //异步操作

#define FM_DEV_TYPE_PCIE_GDB_1_0X 0xE300    //pcie GDB V1.0
#define FM_DEV_TYPE_MINPCIE_1_0X  0xE500    //mini pcie V1.0
#define FM_DEV_TYPE_MINPCIE_3_0X  0xE700

/************************ 用户管理 ************************/
// 用户身份
#define CPC_USER_ADMIN          1   /* 管理员 */
#define CPC_USER_OPER           2   /* 操作员 */
// 用户数目
#define CPC_USER_MAXADMIN       5   /* 最多管理员数目 */
#define CPC_USER_MAXOPER        5   /* 最多操作员数目 */
#define CPC_USER_MAXUSER        (CPC_USER_MAXADMIN + CPC_USER_MAXOPER) /* 最多用户数 */
// 用户管理
#define CPC_USER_ADDADMIN       1   /* 添加管理员 */
#define CPC_USER_ADDOPER        2   /* 添加操作员 */
#define CPC_USER_DELADMIN       3   /* 删除管理员 */
#define CPC_USER_DELOPER        4   /* 删除操作员 */

// 用户备份管理
#define CPC_USER_BAK            1   /* 备份 */
#define CPC_USER_RES            2   /* 恢复 */
#define CPC_USER_BAKINIT        3   /* 备份初始化 */
#define CPC_USER_RESINIT        4   /* 恢复初始化 */
// 认证状态
#define CPC_USER_AUTHPASS       1   /* 认证通过 */
#define CPC_USER_AUTHNOPASS     0   /* 认证未通过 */

typedef struct tagUserInfoSt
{
    FM_U8 au8Serial[FM_DEVINFO_SERIAL_LEN];     // 序列号
    FM_U8 u8Flag;           // 本卡身份 CPC_USER_ADMIN / CPC_USER_OPER */
    FM_U8 u8IsAuthed;       // 是否登录     1已登录
    FM_U8 u8Reserve1;       // 保留
    FM_U8 u8Reserve2;       // 保留
}CPC_USER_INFO, *PCPC_USER_INFO;

/****************************备份需要的公开结构********************************/
// 备份类型
#define CPC_BK_TYPE_ALL     1   // 备份所有内容
#define CPC_BK_TYPE_FILEALL 2   // 备份文件系统 暂不支持
#define CPC_BK_TYPE_FILE    3   // 备份单个文件 暂不支持
#define CPC_BK_TYPE_WKALL   4   // 备份所有工作密钥
#define CPC_BK_TYPE_WK      5   // 备份单个工作密钥
#define CPC_BK_TYPE_RSAALL  6   // 备份所有RSA密钥
#define CPC_BK_TYPE_RSA     7   // 备份单个RSA密钥

// 备份信息存储的设备类型
#define CPC_BK_STORE_HOST   1    // 备份信息存储在主机文件
#define CPC_BK_STORE_DEVKEY 2    // 备份信息存储在设备所属的IC卡或者USBKey 暂不支持

// 备份管理初始化时的配置结构
typedef struct stBkConifg
{
	FM_U32	u32Type;             // 备份类型
 	FM_U32	u32StoreDev;          // 备份信息存储的设备类型
	FM_U8	u8StoreFile[128];    // 备份信息存储的文件名
    FM_U32  u32ThresholdKeyID;      // 设备所属的IC卡或者USBKey中门限备份的密钥ID [0-99]

	// u32Type为CPC_BK_TYPE_WK/CPC_BK_TYPE_RSA时有效
	// 备份初始化(CPC_BK_MNGOP_BAKINIT)时，此参数表示备份的密钥ID
	// 恢复初始化(CPC_BK_MNGOP_RESINIT)时，此参数表示恢复到此指定密钥ID。等于全F时
	// 表示使用备份时的ID作为恢复的密钥ID
	FM_U32	u32HKey;

	FM_U8	u8FullPath[128];    // u32Type为CPC_BK_TYPE_FILE时有效,备份的单个文件的全路径
    FM_U32  u32Resvered[16];    // 保留为0
} CPC_BK_CONFIG, *PCPC_BK_CONFIG;



/****************************** API 函数定义 *********************************/
FM_RET FM_CPC_OpenDevice
(
    FM_I FM_U8     *pu8Id,
    FM_I FM_U32     u32Type,
    FM_I FM_U32     u32Flag,
    FM_O FM_HANDLE *phDev
);
FM_RET FM_CPC_OpenDevice_InputIP
(
    FM_I FM_U8     *pu8Id,
    FM_I FM_U32     u32Type,
    FM_I FM_U32     u32Flag,
    FM_O FM_HANDLE *phDev,
    FM_I FM_U8     *pu8IP
);
FM_RET FM_CPC_CloseDevice
(
    FM_I FM_HANDLE  hDev
);
FM_RET FM_CPC_GetDeviceInfo
(
    FM_I FM_HANDLE  hDev,
    FM_O FM_DEV_INFO  *pDevInfo
);
FM_RET FM_CPC_GenRandom
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Len,
    FM_O FM_U8     *pu8Random
);

FM_RET FM_CPC_GetInfo
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Flag,
    FM_I FM_U32     u32InLen,
    FM_I FM_U8     *pu8InBuf,
    FM_O FM_U32    *pu32Len,
    FM_O FM_U8     *pu8OutBuf
);
FM_RET FM_CPC_GetErrInfo
(
    FM_I FM_U32     u32LanFlag,
    FM_I FM_U32     u32ErrCode,
    FM_B FM_U32    *pu32Len,
    FM_O FM_S8     *ps8Info
);
FM_RET FM_CPC_GenRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32KeyBits,
    FM_I FM_HKEY   *phKey,
    FM_O FM_RSA_PublicKey  *pPubkey,
    FM_O FM_RSA_PrivateKey *pPrikey
);
FM_RET FM_CPC_DelRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey
);
FM_RET FM_CPC_ImportRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY   *phKey,
    FM_I FM_RSA_PublicKey  *pPubkey,
    FM_I FM_RSA_PrivateKey *pPrikey
);
FM_RET FM_CPC_ExportRSAKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_O FM_RSA_PublicKey  *pPubkey,
    FM_O FM_RSA_PrivateKey *pPrikey
);
FM_RET FM_CPC_RSAEncrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen,
    FM_I FM_RSA_PublicKey *pPubkey
);
FM_RET FM_CPC_RSADecrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen,
    FM_I FM_RSA_PrivateKey *pPrikey
);
FM_RET FM_CPC_RSASign
(
	FM_I	FM_HANDLE	hDev,
	FM_I	FM_HKEY		hKey,
	FM_I	FM_U32		u32Alg,
	FM_I	FM_U8		*pu8DataBuf,
	FM_I	FM_U32		u32DataLen,
	FM_O	FM_U8		*pu8SignBuf,
	FM_O 	FM_U32		*pu32SignLen,
	FM_I 	FM_RSA_PrivateKey *pPrikey
);
FM_RET FM_CPC_RSAVerify
(
	FM_I	FM_HANDLE	hDev,
	FM_I	FM_HKEY		hKey,
	FM_I	FM_U32		u32Alg,
	FM_I	FM_U8		*pu8DataBuf,
	FM_I	FM_U32		u32DataLen,
	FM_I	FM_U8		*pu8SignBuf,
	FM_I	FM_U32		u32SignLen,
	FM_I	FM_RSA_PublicKey	*pPubkey
);

FM_RET FM_CPC_GenECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_HKEY   *phKey,
    FM_O FM_ECC_PublicKey  *pPubkey,
    FM_O FM_ECC_PrivateKey *pPrikey
);
FM_RET FM_CPC_DelECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey
);
FM_RET FM_CPC_ImportECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY   *phKey,
    FM_I FM_ECC_PublicKey  *pPubkey,
    FM_I FM_ECC_PrivateKey *pPrikey
);
FM_RET FM_CPC_ExportECCKeypair
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_O FM_ECC_PublicKey  *pPubkey,
    FM_O FM_ECC_PrivateKey *pPrikey
);
FM_RET FM_CPC_ECCEncrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_HKEY    hKey,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_I FM_ECC_PublicKey *pPubkey,
    FM_O FM_ECC_Cipher    *pECCCipher
);
FM_RET FM_CPC_ECCDecrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_HKEY    hKey,
    FM_I FM_ECC_Cipher     *pECCCipher,
    FM_I FM_ECC_PrivateKey *pPrikey,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
FM_RET FM_CPC_ECCSign
(
    FM_I FM_HANDLE             hDev,
    FM_I FM_U32                u32Alg,
    FM_I FM_HKEY               hKey,
    FM_I FM_U8                *pu8InBuf,
    FM_I FM_U32                u32InLen,
    FM_I FM_ECC_PrivateKey    *pPrikey,
    FM_O FM_ECC_Signature     *pSignature
);
FM_RET FM_CPC_ECCVerify
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_U32            u32Alg,
    FM_I FM_HKEY           hKey,
    FM_I FM_ECC_PublicKey *pPubkey,
    FM_I FM_U8            *pu8InBuf,
    FM_I FM_U32            u32InLen,
    FM_I FM_ECC_Signature *pSignature
);
FM_RET FM_CPC_GenerateAgreementDataWithECC
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
FM_RET FM_CPC_GenerateAgreementDataAndKeyWithECC
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
FM_RET FM_CPC_GenerateKeyWithECC
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

FM_RET FM_CPC_GenKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_U32     u32InLen,
    FM_I FM_HKEY   *phKey,
    FM_O FM_U8     *pu8Key
);
FM_RET FM_CPC_DelKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey
);
FM_RET FM_CPC_ImportKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_U8     *pu8Key,
    FM_I FM_U32     u32KeyLen,
    FM_I FM_HKEY   *phKey
);
FM_RET FM_CPC_ExportKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_O FM_U8     *pu8Key,
    FM_I FM_U32    *pu32Len
);
FM_RET FM_CPC_EncryptInit
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U32     u32Alg,
    FM_I FM_U32     u32WorkMode,
    FM_I FM_U8     *pu8Key,
    FM_I FM_U32     u32KeyLen,
    FM_I FM_U8     *pu8IV,
    FM_I FM_U32     u32IVLen
);
FM_RET FM_CPC_EncryptUpdate
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
FM_RET FM_CPC_EncryptFinal
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg
);
FM_RET FM_CPC_Encrypt
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
FM_RET FM_CPC_DecryptInit
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HKEY    hKey,
    FM_I FM_U32     u32Alg,
    FM_I FM_U32     u32WorkMode,
    FM_I FM_U8     *pu8Key,
    FM_I FM_U32     u32KeyLen,
    FM_I FM_U8     *pu8IV,
    FM_I FM_U32     u32IVLen
);
FM_RET FM_CPC_DecryptUpdate
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
FM_RET FM_CPC_DecryptFinal
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg
);
FM_RET FM_CPC_Decrypt
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

FM_RET FM_CPC_HashInit
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_U32            u32Alg
);
FM_RET FM_CPC_HashUpdate
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen
);
FM_RET FM_CPC_HashFinal
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Alg,
    FM_O FM_U8     *pu8OutMACBuf,
    FM_O FM_U32    *pu32OutMACLen
);

FM_RET FM_CPC_HashInit_ex
(
	 FM_I FM_HANDLE         hDev,
	 FM_I FM_U32            u32Alg,
	 FM_O FM_U8		        *pu8Ctx,
	 FM_O FM_U32            *pu32CtxLen
 );
FM_RET FM_CPC_HashUpdate_ex
(
	 FM_I FM_HANDLE  hDev,
	 FM_I FM_U32     u32Alg,
	 FM_I FM_U8     *pu8InBuf,
	 FM_I FM_U32     u32InLen,
	 FM_B FM_U8		*pu8Ctx,
	 FM_B FM_U32    *pu32CtxLen
 );
FM_RET FM_CPC_HashFinal_ex
(
	 FM_I FM_HANDLE  hDev,
	 FM_I FM_U32     u32Alg,
	 FM_O FM_U8     *pu8OutMACBuf,
	 FM_O FM_U32    *pu32OutMACLen,
	 FM_I FM_U8		*pu8Ctx,
	 FM_I FM_U32    u32CtxLen
);

FM_U32 FM_CPC_SM3Init
(
    FM_I FM_HANDLE         hDev,
    FM_I FM_ECC_PublicKey *pPubkey,
    FM_I FM_U8            *pu8ID,
    FM_I FM_U32            u32IDLen
);
FM_U32 FM_CPC_SM3Update
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen
);
FM_U32 FM_CPC_SM3Final
(
    FM_I FM_HANDLE  hDev,
    FM_O FM_U8     *pu8HashBuf,
    FM_O FM_U32    *pu32HashLen
);

FM_U32 FM_CPC_SM3Init_ex
(
	 FM_I FM_HANDLE         hDev,
	 FM_I FM_ECC_PublicKey *pPubkey,
	 FM_I FM_U8            *pu8ID,
	 FM_I FM_U32            u32IDLen,
	 FM_O FM_U8		        *pu8Ctx,
	 FM_O FM_U32            *pu32CtxLen
);
FM_U32 FM_CPC_SM3Update_ex
(
	 FM_I FM_HANDLE  hDev,
	 FM_I FM_U8     *pu8InBuf,
	 FM_I FM_U32     u32InLen,
	 FM_B FM_U8		*pu8Ctx,
	 FM_B FM_U32    *pu32CtxLen
 );
FM_U32 FM_CPC_SM3Final_ex
(
	 FM_I FM_HANDLE  hDev,
	 FM_O FM_U8     *pu8HashBuf,
	 FM_O FM_U32    *pu32HashLen,
	 FM_I FM_U8		*pu8Ctx,
	 FM_I FM_U32    u32CtxLen
);

FM_RET FM_CPC_USER_Login
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Type,
    FM_I FM_U8     *pu8PinBuf,
    FM_I FM_U32     u32PinLen,
    FM_O FM_HUSER  *phUser,
    FM_O FM_U32    *pu32RetryNum
);
FM_RET FM_CPC_USER_Logout
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_HUSER   hUser
);
FM_RET FM_CPC_USER_ChangePin
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Flag,
    FM_I FM_U8     *pu8OldPinBuf,
    FM_I FM_U32     u32OldPinLen,
    FM_I FM_U8     *pu8NewPinBuf,
    FM_I FM_U32     u32NewPinLen,
    FM_O FM_U32    *pu32RetryNum
);
FM_RET FM_CPC_USER_GetInfo
(
    FM_I FM_HANDLE  hDev,
    FM_O CPC_USER_INFO aUserInfo[CPC_USER_MAXUSER]
);
FM_RET FM_CPC_USER_UserMng
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Flag,
    FM_I FM_U8     *pu8Buf,
    FM_I FM_U32     u32Len
);
FM_RET FM_CPC_USER_BackupMng
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Flag,
    FM_I CPC_BK_CONFIG  *pstBkConfig,
    FM_O FM_U32    *pu32UserAllNum,
    FM_O FM_U32    *pu32UserDealNum
);

FM_RET FM_CPC_FILE_Init
(
    FM_I FM_HANDLE  hDev
);
FM_RET FM_CPC_FILE_CreateDir
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8FullDir,
    FM_I FM_U32     u32AccCond
);
FM_RET FM_CPC_FILE_DeleteDir
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8FullDir
);
FM_RET FM_CPC_FILE_CreateFile
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8DirName,
    FM_I FM_S8     *ps8FileName,
    FM_I FM_U32     u32FileSize,
    FM_I FM_U32     u32AccCond
);
FM_RET FM_CPC_FILE_ReadFile
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8DirName,
    FM_I FM_S8     *ps8FileName,
    FM_I FM_U32     u32Offset,
    FM_I FM_U32     u32Size,
    FM_O FM_U8     *pu8Buf
);
FM_RET FM_CPC_FILE_ReadFile2
(
 FM_I FM_HANDLE  hDev,
 FM_I FM_S8     *ps8DirName,
 FM_I FM_S8     *ps8FileName,
 FM_I FM_U32     u32Offset,
 FM_I FM_U32     u32Size,
 FM_O FM_U8     *pu8Buf,
 FM_O FM_U32    *pu32BytesRead
);
FM_RET FM_CPC_FILE_WriteFile
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8DirName,
    FM_I FM_S8     *ps8FileName,
    FM_I FM_U32     u32Offset,
    FM_I FM_U32     u32Size,
    FM_I FM_U8     *pu8Buf
);
FM_RET FM_CPC_FILE_WriteFile2
(
 FM_I FM_HANDLE  hDev,
 FM_I FM_S8     *ps8DirName,
 FM_I FM_S8     *ps8FileName,
 FM_I FM_U32     u32Offset,
 FM_I FM_U32     u32Size,
 FM_I FM_U8     *pu8Buf,
 FM_O FM_U32    *pu32BytesWrote
);
FM_RET FM_CPC_FILE_DeleteFile
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8DirName,
    FM_I FM_S8     *ps8FileName
);
FM_RET FM_CPC_FILE_EnmuDir
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8DirName,
    FM_B FM_U32    *pu32BufLen,
    FM_O FM_U8     *pu8Buf,
    FM_O FM_U32    *pu32DirNum
);
FM_RET FM_CPC_FILE_EnmuFile
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_S8     *ps8DirName,
    FM_B FM_U32    *pu32BufLen,
    FM_O FM_U8     *pu8Buf,
    FM_O FM_U32    *pu32FileNum
);
FM_RET FM_CPC_Cmd
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U32     u32Cmd,
    FM_I FM_U32     aPara[],
    FM_I FM_U8     *pu8InData,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutData,
    FM_O FM_U32    *pu32OutLen
);


//生成设备主密钥
FM_RET CPC_Master_GenKey
(
    FM_I FM_HANDLE  hDev
);
//导入设备主密钥
FM_RET CPC_Master_ImportKey
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8     *pu8Key,
    FM_I FM_U32     u32KeyLen
);
//导出设备主密钥
FM_RET CPC_Master_ExportKey
(
    FM_I FM_HANDLE  hDev,
    FM_O FM_U8     *pu8Key,
    FM_O FM_U32    *pu32Len
);
//主密钥进行加密
FM_RET CPC_Master_Encrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
//主密钥进行解密
FM_RET CPC_Master_Decrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
//key中密钥进行加密
FM_RET CPC_Usbkey_Encrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);
//key中密钥进行解密
FM_RET CPC_Usbkey_Decrypt
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8     *pu8InBuf,
    FM_I FM_U32     u32InLen,
    FM_O FM_U8     *pu8OutBuf,
    FM_O FM_U32    *pu32OutLen
);

FM_RET FM_CPC_USER_Right
(
    FM_I FM_HANDLE  hDev,
    FM_I FM_U8  *pu8serial,
    FM_I FM_U32  u32Len
);

FM_RET FM_CPC_ContainerWrite
(
	FM_I FM_HANDLE  hDev,
	FM_I FM_U32  	u32Flag,
	FM_I FM_U8  	*pContainerName,
	FM_I FM_U8  	*pCert,
	FM_I FM_U32  	u32CertLen
);

FM_RET FM_CPC_ContainerRead
(
	FM_I FM_HANDLE  hDev,
	FM_I FM_U32  	u32Flag,
	FM_I FM_U8  	*pContainerName,
	FM_O FM_U8  	*pValue,
	FM_O FM_U32  	*pu32ValueLen
);

FM_RET FM_CPC_ContainerDelete
(
	FM_I FM_HANDLE  hDev,
	FM_I FM_U8  	*pContainerName
);

FM_RET FM_CPC_ContainerEnum
(
	FM_I FM_HANDLE  hDev,
	FM_O FM_U8  	*pContainerNames,
	FM_O FM_U32  	*pu32ContainerCount
);
FM_RET FM_CPC_GetSofwareVersion
(
 FM_I FM_HANDLE  hDev,
 FM_O FM_U8     *SofwareVersion
);
/*
FM_RET FM_CPC_GetSofwareConfig
(
 FM_I FM_HANDLE  hDev,
 FM_O FM_SOF_CONFIG *SofwareConfig
);
*/
int FM_CPC_InitData
(
 FM_I FM_HANDLE  hDev
);

int FM_CPC_ReadData
(
 FM_I FM_HANDLE  hDev,
 FM_I FM_U32 u32Offset,
 FM_I FM_U32 u32DataLen,
 FM_O FM_U8* pu8Data
);

int FM_CPC_WriteData
(
 FM_I FM_HANDLE  hDev,
 FM_I FM_U32 u32Offset,
 FM_I FM_U8* pu8Data,
 FM_I FM_U32 u32DataLen
);

int FM_CPC_InitDataForGDB
(
 FM_I FM_HANDLE  hDev
);

int FM_CPC_ReadDataForSGD
(
 FM_I FM_HANDLE  hDev,
 FM_I FM_U32 u32Offset,
 FM_I FM_U32 u32DataLen,
 FM_O FM_U8* pu8Data
);

int FM_CPC_WriteDataForSGD
(
 FM_I FM_HANDLE  hDev,
 FM_I FM_U32 u32Offset,
 FM_I FM_U8* pu8Data,
 FM_I FM_U32 u32DataLen
);

int FM_CPC_GetPriKeyPwdForSGD
(
	FM_I FM_HANDLE  hDev,
	FM_I FM_U32 keyNum,
	FM_I FM_U32 u32DataLen,
	FM_O FM_U8* pu8Data
);

FM_RET FM_CPC_Get_CPin
(
 FM_I FM_HANDLE  hDev,
 FM_O FM_U8      *pu8Pinbuffer,
 FM_I FM_U32     u32PinIndex,
 FM_I FM_U32     u32PinFlag
);

#ifdef  __cplusplus
    }
#endif  /* #ifdef  __cplusplus */

#endif  /* #ifndef FM_CPC_PUB_H */



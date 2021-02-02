/******************************************************************************
* Copyright (c) 2014, Shandong Fisherman Information Technology Co., Ltd.
* All rights reserved.
* 文件名称: fm_def.h
* 文件描述: 渔翁公共定义头文件。
* 当前版本: 1.1.2
* 作    者:
* 创建时间: 2015-4-9
* 修改记录:
* ----------------------------------------------
*    时间    作者     描述
******************************************************************************/
#ifndef FM_DEF_H
#define FM_DEF_H

#ifdef  __cplusplus
    extern "C" {
#endif

#define FM_LITTLE_ENDIAN
//#define FM_BIG_ENDIAN
//#define HIDKEY  //编译HIDKEY API需打开此宏
#define GUOXIN_KEY	//Linux下 国芯KEY 库编译时需要使用此宏
//#define  TF //用于编译支持TF卡(3202、3302)的api、demo、FMKeyAndroidTest
//#define TF3302_DEV_DRIVE_H  //编译3302的api除需打开#define TF外，还需打开此宏
							  //该宏不用了，3302使用3202的ccdrive即可
//#define TF3302 //编译CCM3302芯片的TF卡cos时需要打开此宏,其他时候需关闭此宏
//#define USE_OPENSSL //导入证书时使用OPENSSL解析证书
//#define GUOXIN_3310S_T_ANDROID  //在android下编译3310S_T api时需要打开该宏

/******* 客户定制开关 ********/
//#define NANRUIXINTONG
//#define ZHUHAIHONGRUI
//#define JMJ_RSA4096
//#define WUHANSANJIANG
//#define TONGZHIWEIYE
//#define JIANGSUGUOYIN
//#define BEIYINGSI //贝英斯定制开关  导出密钥需满足管理员权限
/*****************************/

/*****国芯key cos定制开关****/
//#define GUOXINFORCARD //只能用于密码卡权限管理和备份恢复

/*************版本*********/
#define FM_KEYAPI_VERSION "V2.5.0.0"
#define FM_TFAPI_VERSION  "V2.5.0.0"

/**********key 类型***********/
#define FM_KEYTYPE_GUOXIN  5
#define FM_KEYTYPE_HSC32EU 7

/*********************** basic data type ************************/
#define FM_S8               char
#define FM_U8               unsigned char
#define FM_S16              short int
#define FM_U16              unsigned short int
#define FM_S32              int
#define FM_U32              unsigned int
#define FM_S64              long       /* only used in 64b os */
#define FM_U64              unsigned long  /* only used in 64b os */
#define FM_UP               unsigned long  /* sizeof(FM_UP)==sizeof(pointer) */
#define FM_F32              float
#define FM_F64              double
#define FM_VOID             void
#define FM_HANDLE           void *
#define FM_HKEY             void *
#define FM_HUSER            void *
#define FM_BOOL             unsigned int

#define FM_NULL             ((void *) 0)
#define FM_TRUE             1
#define FM_FALSE            0
#define FM_U8_INVALID       0xff
#define FM_U16_INVALID      0xffff
#define FM_U32_INVALID      0xffffffff
#define FM_HANDLE_INVALID   (FM_HANDLE)(-1)
#define FM_C                const

#define HDISK void*

#ifdef FM_WINDLL
    #define    FM_RET  FM_U32 _stdcall
    #define    FM_DLLCALL     _stdcall
#else
    #define    FM_RET  FM_U32
    #define    FM_DLLCALL
#endif

/*************** function parameter description  ****************/
#define FM_I    /* input  parameter */
#define FM_O    /* output parameter */
#define FM_B    /* both input and output parameter */

/********************* language identifier **********************/
#define FM_LAN_CN               0      /* chinese */
#define FM_LAN_EN               1      /* english */

/************************ algorithm mode ************************/
#define FM_ALGMODE_ECB          0x00000000
#define FM_ALGMODE_CBC          0x00000001
#define FM_ALGMODE_CFB          0x00000002
#define FM_ALGMODE_OFB          0x00000003
#define FM_ALGMODE_MAC          0x00000004
#define FM_ALGMODE_CTR          0x00000005
/********************* algorithm identifier *********************/
/* get alg mask */
#define FM_ALG_GETMASK(alg) (1 << ((alg) % 32))
/* symmetry */
#define FM_ALG_SSF33            0x00000000
#define FM_ALG_SCB2_S           0x00000001   /* SCB2 special */
#define FM_ALG_SCB2_G           0x00000002   /* SCB2 general */
#define FM_ALG_SM1              FM_ALG_SCB2_G
#define FM_ALG_SM6              FM_ALG_SCB2_S
#define FM_ALG_3DES             0x00000003
#define FM_ALG_AES              0x00000004
#define FM_ALG_DES              0x00000005
#define FM_ALG_RC2              0x00000006
#define FM_ALG_RC4              0x00000007
#define FM_ALG_SM4              0x00000008
#define FM_ALG_AES192           0x00000009
#define FM_ALG_AES256           0x0000000a


/* asymmetry */
#define FM_ALG_RSA1024          0x00000000
#define FM_ALG_RSA2048          0x00000001
#define FM_ALG_RSA4096          0x00000002
#define FM_ALG_SM2_1            0x00000003
#define FM_ALG_SM2_2            0x00000004
#define FM_ALG_SM2_3            0x00000005
/* hash */
#define FM_ALG_MD2              0x00000000
#define FM_ALG_MD4              0x00000001
#define FM_ALG_MD5              0x00000002
#define FM_ALG_SHA1             0x00000003
#define FM_ALG_SHA256           0x00000004
#define FM_ALG_SHA384           0x00000005
#define FM_ALG_SHA512           0x00000006
#define FM_ALG_SM3              0x00000007

/******************** change byte order macro *******************/
#ifdef FM_LITTLE_ENDIAN
#define FM_SWAP16(s) (((s)&0xff)<<8|((s)&0xff00)>>8)
#define FM_SWAP32(u) (((u)&0xff)<<24|((u)&0xff00)<<8|((u)&0xff0000)>>8|((u)&0xff000000)>>24)
#else
#define FM_SWAP16(s) (s)
#define FM_SWAP32(u) (u)
#endif

#ifdef FM_LITTLE_ENDIAN
    #define FM_H2NS(s) FM_SWAP16(s)
    #define FM_N2HS(s) FM_SWAP16(s)
    #define FM_H2NL(u) FM_SWAP32(u)
    #define FM_N2HL(u) FM_SWAP32(u)
#else
    #define FM_H2NS(s)  (s)
    #define FM_N2HS(s)  (s)
    #define FM_H2NL(u)  (u)
    #define FM_N2HL(u)  (u)
#endif

#define FM_SWAP16_2(s) (((s)&0xff)<<8|((s)&0xff00)>>8)
#define FM_SWAP32_2(u) (((u)&0xff)<<24|((u)&0xff00)<<8|((u)&0xff0000)>>8|((u)&0xff000000)>>24)

/**** error code: low 11 bit of api return value 0x000-0x7ff ****/

/* get error code macro */
#define FME_GETERR(e)       ((e) & 0x7ff)

/* general error        0x000 - 0x07f */
#define FME_OK                  0x000      /* ok */
#define FME_ERR                 0x001      /* fail */
#define FME_POK                 0x002      /* part ok */
#define FME_UNKNOWN             0x003      /* unknown error */
#define FME_NOTSUPPORT          0x004      /* not support */
#define FME_PARA                0x005      /* error parameter */
#define FME_NORIGHT             0x006      /* error right */
#define FME_BUSY                0x007      /* device busy */
#define FME_TIMEOUT             0x008      /* operation timeout */
#define FME_NOMEM               0x009      /* no mem */
#define FME_NORES               0x00a      /* no resouce */
#define FME_COMMERR             0x00b      /* communication error */
#define FME_ACCESSREG           0x00c      /* register access error */
#define FME_STACKOVER           0x00d      /* stack is overflow */
#define FME_DEVINUSE            0x00e      /* device is used now */
#define FME_SESEXCEED           0x00f      /* open session is out of range */
#define FME_DMAREADERR          0x010      /* dma read error */
#define FME_DMAWRITEERR         0x011      /* dma write error */
#define FME_CREATESYNCERR       0x012      /* create sync objcect error */
#define FME_GETSYNCERR          0x013      /* get sync objcect error */
#define FME_RELEASESYNCERR      0x014      /* release sync objcect error */
#define FME_DATALENERR          0x015      /* data length error */
#define FME_KEYLENERR           0x016      /* key length error or not support */
#define FME_DEVNOTCLOSE         0x017

/* atuo test error      0x080 - 0x09f */
#define FME_AT_OK               0x080      /* auto test success */
#define FME_AT_USB              0X081      /* usb test error */
#define FME_AT_RANDOM           0X082      /* random test error */
#define FME_AT_SM1              0X083      /* sm1/scb2 alg test error */
#define FME_AT_PROGCHECK        0X084      /* program integrality test error */
#define FME_AT_STARTING         0X085      /* device is starting  */
#define FME_AT_SM2              0X086      /* sm2 alg init error */
#define FME_AT_SM2E             0X087      /* sm2e alg init error */

/* common alg error     0x0a0 - 0x0ff */
#define FME_KEYNOTEXIST         0x0a0      /* key is not exist */
#define FME_KEYNOFREE           0x0a1      /* no free key handle */
#define FME_KEYEXCEED           0x0a2      /* key handle is out of range */
#define FME_STEPERR             0x0a3      /* alg step err(init/update/final) */

/* sym err              0x100 - 0x17f */
#define FME_IVLENERR            0x100      /* iv length error*/

/* asym err             0x180 - 0x1ff */
#define FME_ECC_NOTINIT         0x180      /* ECC not init */
#define FME_ECC_PUBKEYERR       0x181      /* ECC pubkey error */
#define FME_ECC_PRIKEYERR       0x182      /* ECC prikey error */
#define FME_ECC_SIGNERR         0x183      /* ECC sign error */
#define FME_ECC_VERIFYERR       0x184      /* ECC verify error */
#define FME_ECC_ENCRYPTERR      0x185      /* ECC encrypt error */
#define FME_ECC_DECRYPTERR      0x186      /* ECC decrypt error */
#define FME_ECC_DECVERIFYERR    0x187      /* ECC decrypt verify error */
#define FME_ECC_AGREEMENTERR    0x188

/* hash err             0x200 - 0x23f */
#define FME_SM3_IDLENERR        0x200      /* SM3 user'ID length is out of range */

/* file system err      0x240 - 0x27f */
#define FME_FILE_NOTINIT        0x240      /* file system is not init */
#define FME_FILE_DIRDEPTH       0x241      /* dir nested too deep */
#define FME_FILE_DIRNOTEXIST    0x242      /* dir is not exist */
#define FME_FILE_FILENOTEXIST   0x243      /* file is not exist */
#define FME_FILE_DIREXIST       0x244      /* dir has already exist */
#define FME_FILE_FILEEXIST      0x245      /* file has already exist */
#define FME_FILE_DIRNUMEXCEED   0x246      /* dir number is out of range */
#define FME_FILE_FILENUMEXCEED  0x247      /* file number is out of range */
#define FME_FILE_NOSPACE        0x248      /* file space is not enough */
#define FME_FILE_OPRANGE        0x249      /* file operation is out of range */
#define FME_FLASH_TIMEOUT       0x260      /* flash operation is timeout */
#define FME_FLASH_WRITEERR      0x261      /* flash write error */
#define FME_FLASH_READERR       0x262      /* flash read error */
#define FME_FLASH_OPRANGE       0x263      /* flash operation is out of range */
#define FME_EE_TIMEOUT          0x264      /* eeprom operation is timeout */
#define FME_EE_WRITEERR         0x265      /* eeprom write error  */
#define FME_EE_READERR          0x266      /* eeprom read error */
#define FME_EE_OPRANGE          0x267      /* eeprom operation is out of range */

/* user err             0x280 - 0x2bf  */
#define FME_USER_NOTEXIST       0x280      /* user is not exist */
#define FME_USER_EXIST          0x281      /* user has already exist */
#define FME_USER_EXCEED         0x282      /* user number is out of range */
#define FME_USER_PINERR         0x283      /* user pin err */
#define FME_USER_STEPERR        0x284      /* backup step err */
#define FME_USER_OPENDEVERR     0x285      /* open user dev err */
#define FME_USER_GETDEVINFOERR  0x286      /* get user dev info err */
#define FME_USER_WRITEDEVERR    0x287      /* write user dev mem err */
#define FME_USER_READDEVERR     0x288      /* read user dev mem err */
#define FME_USER_NOLOG          0x289      /* user is not login */

/* network err             0x2c0 - 0x2ff  */
#define FME_NET_NOCONNECT                   0x2c0               /*can not get connect */
#define FME_NET_SENDERR                     0x2c1           /*send data error */
#define FME_NET_RECVERR                     0x2c2           /*receive data error */
#define FME_NET_SOCKETPOOLFREEERR   0x2c3           /*free socket pool error */

/* debug err            0x2c0 - 0x2ff  */

/********************* config releated macro ********************/
/* cpu and os platform type */
//#define FMCFG_OS_WINDOWS
#define FMCFG_OS_LINUX
//#define FMCFG_OS_ANDROID
//#define FMCFG_OS_DSP
//#define FMCFG_OS_COS

/* product type */
//#define FMCFG_TYPE_SJK0809  /* PCI Express Crypto PCI Card */
#define FMCFG_TYPE_SJK0810  /* PCI Crypto PCI Card */
//#define FMCFG_TYPE_SJK0918  /* USB Key */
//#define FMCFG_TYPE_SJJ1002  /* SCSI Key */

/* compile version */
//#define FMCFG_VER_DEBUG     /* debug version */
#define FMCFG_VER_RELEASE   /* release version */

/*************************** dev type ***************************/
#define FM_DEV_TYPE_SJK0809   1
#define FM_DEV_TYPE_SJK0810   2
#define FM_DEV_TYPE_SSX1104   3
#define FM_DEV_TYPE_SJK1150   4
#define FM_DEV_TYPE_SJJ1002   5
#define FM_DEV_TYPE_SSX1208   6

/********************** open device flag ************************/
#define FM_OPEN_EXCLUSIVE     0x00000001  /* exclusive open device */
#define FM_OPEN_MULTITHREAD   0x00000002  /* device support multi thread. only used by user api */
#define FM_OPEN_MULTIPROCESS  0x00000004  /* device support multi process only used by user api */
#define FM_OPEN_INT           0x00000008  /* host and device communicate by interrupt. only used by kernel api */
#define FM_OPEN_SCH           0x00000010  /* host and device communicate by poll. only used by kernel api  */
#define FM_OPEN_SHARE         0x00000020  /* share open device */
#define FM_OPEN_ASYNC         0x00000040  /* async call mode. only used by kernel api  */
#define FM_OPEN_FMCS          0x00000080  /* only for fisherman crypto server */

/************************* device info **************************/
#define FM_DEVINFO_ISSUER_LEN   16
#define FM_DEVINFO_DEVICE_LEN   16
#ifdef JIANGSUGUOYIN
#define FM_DEVINFO_SERIAL_LEN   15
#else
#define FM_DEVINFO_SERIAL_LEN   16
#endif


#define FM_DEV_TYPE_PCI_1_2X      0x0120    //pci  V1.2
#define FM_DEV_TYPE_PCI_2_0X      0x0200    //pci  V2.0
#define FM_DEV_TYPE_PCIE_1_0X     0xE100    //pcie V1.0
#define FM_DEV_TYPE_PCIE_2_0X     0xE200    //pcie V2.0
#define FM_DEV_TYPE_PCIE_3_0X     0xE200    //pcie V3.0
#define FM_DEV_TYPE_PCIE_4_0X     0xE400    //pcie V4.0
#define FM_DEV_TYPE_PCIE_5_0X     0xE600    //pcie V5.0
#define FM_DEV_TYPE_YIBU          0xE601    //靠靠

#define FM_DEV_TYPE_PCIE_GDB_1_0X 0xE300    //pcie GDB V1.0
#define FM_DEV_TYPE_MINPCIE_1_0X  0xE500    //mini pcie V1.0
#define FM_DEV_TYPE_MINPCIE_3_0X  0xE700


typedef struct tagDeviceInfoSt
{
    FM_U32 u32State;                                /* 设备状态 */
                                                    // 其他，未初始化
    FM_U8  au8IssuerName[FM_DEVINFO_ISSUER_LEN];    /* 生产厂商名称 */
    FM_U8  au8DeviceName[FM_DEVINFO_DEVICE_LEN];    /* 型号 */
    FM_U8  au8DeviceSerial[FM_DEVINFO_SERIAL_LEN];  /* 编号 */
    FM_U32 u32DeviceVersion;                        /* 内部软件的版本号 */
    FM_U32 au32SymAlgMask[4];                       /* 所支持的对称算法 */
    FM_U32 au32AsymAlgMask[4];                      /* 所支持的非对称的算法 */
    FM_U32 au32HashAlgMask[4];                      /* 所支持的杂凑算法 */
    FM_U32 au32SymKeyNum[4];                        /* 对称密钥数目 低16位是永久密钥数目，高16位是临时密钥数目 */
    FM_U32 au32AsymKeyNum[4];                       /* 非对称密钥数目 低16位是永久密钥数目，高16位是临时密钥数目  */
    FM_U32 u32FileSize;                             /* 支持文件存储空间 */
    FM_U16 u16DirNum;                               /* 支持目录的数目 */
    FM_U16 u16FileNum;                              /* 支持文件的数目 */
}FM_DEV_INFO, *PFM_DEV_INFO; //144字节


/************************* key handle ***************************/
#define FM_HKEY_TO_HOST     (FM_HANDLE)0x01ffffff  /* key is not storage in dev */
#define FM_HKEY_FROM_HOST   (FM_HANDLE)0x02ffffff  /* use host import temp key */
#define FM_HKEY_BYDEV_PERM  (FM_HANDLE)0x03ffffff  /* key handle created by dev, permanent storage in device */
#define FM_HKEY_BYDEV_TEMP  (FM_HANDLE)0x83ffffff  /* key handle created by dev, temporary storage in device */
#define FM_HKEY_MEM_MASK               0x80000000  /* the high bit equal to 1, temporary storage in device */

/***************************** RSA ******************************/
/* len */
#ifdef JMJ_RSA4096
#define FM_RSA_MAX_BITS     4096
#define FM_RSA_MAX_LEN      512
#define FM_RSA_MAX_PLEN     256
#define FM_RSA_MAX_KEYNUM   384
#else
#define FM_RSA_MAX_BITS     2048
#define FM_RSA_MAX_LEN      256
#define FM_RSA_MAX_PLEN     128
#define FM_RSA_MAX_KEYNUM   768
#endif
/* public Key size=516byte */
typedef struct tagRSAPublicKeySt
{
     FM_U32 bits;                      /* modulus bit length */
     FM_U8 m[FM_RSA_MAX_LEN];          /* modulus N, N=P*Q */
     FM_U8 e[FM_RSA_MAX_LEN];          /* public exponent E */
}FM_RSA_PublicKey, *PFM_RSA_PublicKey;

/* private Key size=1412byte */
typedef struct tagRSAPrivateKeySt
{
     FM_U32 bits;                      /* modulus bit length */
     FM_U8 m[FM_RSA_MAX_LEN];          /* modulus N, N=P*Q */
     FM_U8 e[FM_RSA_MAX_LEN];          /* public exponent E */
     FM_U8 d[FM_RSA_MAX_LEN];          /* private exponent D, D=(E^-1)mod(P-1)(Q-1), now not use */
     FM_U8 prime[2][FM_RSA_MAX_PLEN];  /* prime P and Q, P>Q */
     FM_U8 pexp[2][FM_RSA_MAX_PLEN];   /* Dp,Dq, Dp=D mod(P-1),Dq=D mod(Q-1) */
     FM_U8 coef[FM_RSA_MAX_PLEN];      /* Q^-1 mod P  */
}FM_RSA_PrivateKey, *PFM_RSA_PrivateKey;

/***************************** ECC ******************************/
/* len */
#define FM_ECC_MAX_BITS      256
#define FM_ECC_MAX_LEN       ((FM_ECC_MAX_BITS+7)/8)
#define FM_ECC_MAX_INBUF_LEN 160
/* public Key */
typedef struct tag_ECC_PublicKey_St
{
     FM_U32 bits;                      /* modulus bit length */
     FM_U8 x[FM_ECC_MAX_LEN];          /* x */
     FM_U8 y[FM_ECC_MAX_LEN];          /* y */
}FM_ECC_PublicKey, *PFM_ECC_PublicKey;
/* private Key */
typedef struct tag_ECC_PrivateKey_St
{
     FM_U32 bits;                      /* modulus bit length */
     FM_U8  D[FM_ECC_MAX_LEN];         /* PrivateKey */
}FM_ECC_PrivateKey, *PFM_ECC_PrivateKey;

/* ECC Signature */
typedef struct tag_ECC_Signature_St
{
     FM_U8 r[FM_ECC_MAX_LEN];          /* r */
     FM_U8 s[FM_ECC_MAX_LEN];          /* s */
}FM_ECC_Signature, *PFM_ECC_Signature;

#ifdef TF3302
typedef struct tag_ECC_Cipher_st
{
    FM_U32 Clength;            /*加密数据长度*/
    FM_U8  x[FM_ECC_MAX_LEN];  /*与y组成椭圆曲线上的点(x,y)*/
    FM_U8  y[FM_ECC_MAX_LEN];  /*与x组成椭圆曲线上的点(x,y)*/
    FM_U8  c[FM_ECC_MAX_INBUF_LEN];             /*加密数据*/
    FM_U8  M[FM_ECC_MAX_LEN];  /*预留,用于支持带MAC输出的ECC算法*/
}FM_ECC_Cipher, *PFM_ECC_Cipher;
#else
typedef struct tag_ECC_Cipher_st
{
    FM_U32 Clength;            /*加密数据长度*/
    FM_U8  x[FM_ECC_MAX_LEN];  /*与y组成椭圆曲线上的点(x,y)*/
    FM_U8  y[FM_ECC_MAX_LEN];  /*与x组成椭圆曲线上的点(x,y)*/
    FM_U8  C[FM_ECC_MAX_INBUF_LEN];             /*加密数据*/
    FM_U8  M[FM_ECC_MAX_LEN];  /*预留,用于支持带MAC输出的ECC算法*/
}FM_ECC_Cipher, *PFM_ECC_Cipher;
#endif

/* ECC curve data */
typedef struct tag_ECC_curve_data_st
{
    FM_U8  p[FM_ECC_MAX_LEN];   /*素数P*/
    FM_U8  a[FM_ECC_MAX_LEN];   /*参数a*/
    FM_U8  b[FM_ECC_MAX_LEN];   /*参数b*/
    FM_U8  x[FM_ECC_MAX_LEN];   /**/
    FM_U8  y[FM_ECC_MAX_LEN];   /**/
    FM_U8  N[FM_ECC_MAX_LEN];   /**/
    FM_U32 bits;
}FM_ECC_CURVE_DATA, *PFM_ECC_CURVE_DATA;

typedef struct
{
    FM_U32 h0,h1,h2,h3,h4;
    FM_U32 Nl,Nh;
    FM_U32 data[16];
    FM_U32 num;
    FM_U32 len;
    FM_U8  hash[32];
}FM_CPC_SHA1_CTX, *PFM_CPC_SHA1_CTX;

typedef struct
{
    FM_U32 h[8];
    FM_U32 Nl,Nh;
    FM_U32 data[16];
    FM_U32 num;
    FM_U32 md_len;
}FM_CPC_SHA256_CTX, *PFM_CPC_SHA256_CTX;

typedef struct
{
    FM_U8  u8TempData[64];
    FM_U32 u32TempDataLen;
    FM_U32 u32IV[8];
    FM_U32 Nl,Nh;
    FM_U32 u32DateLen;
    FM_U8  u8ID[128];
    FM_U32 u32IDLen;
    FM_ECC_PublicKey EccPubkey;
}FM_CPC_SM3_CTX, *PFM_CPC_SM3_CTX;

/***************************** pin ******************************/
/* len */
#define FM_PIN_MAXLEN         16  /* pin max length */
#define FM_PIN_MINLEN         8   /* pin min length *///之前为6
#define FM_PIN_MAXRETRY       6   /* max retry times */
/* pin type */
#define FM_PIN_ADMIN          1
#define FM_PIN_OPER           2
/* change type */
#define FM_PIN_CHANGEOPER     1   /* change operator pin */
#define FM_PIN_CHANGEADMIN    2   /* change administrator pin */
#define FM_PIN_UNBLOCKOPER    3   /* unblock operator pin */

/************************* authenticate *************************/
#define FM_INTAUTH            1   /* internal authenticate */
#define FM_EXTAHTH            2   /* external authenticate */

/*********************** get/set info flag **********************/
/*  */
#define FM_INFO_STATE            0x0001
/* dev info */
#define FM_INFO_VENDOR           0x0101
#define FM_INFO_DEVTYPE          0x0102
#define FM_INFO_DEVNAME          0x0103
#define FM_INFO_SERIAL           0x0104
/* dev version */
#define FM_INFO_HWVER            0x0201
#define FM_INFO_FWVER            0x0202
#define FM_INFO_DRVVER           0x0203
#define FM_INFO_APIVER           0x0204
/* dev alg */
#define FM_INFO_SYMALG           0x0301
#define FM_INFO_ASYMALG          0x0302
#define FM_INFO_HASHALG          0x0303
/* sym alg key */
#define FM_INFO_SYMKEY_TEMP      0x0401
#define FM_INFO_SYMKEY_PERM      0x0402
#define FM_INFO_SYMKEYSM1_TEMP   0x0403
#define FM_INFO_SYMKEYSM1_PERM   0x0404
#define FM_INFO_SYMKEYSM6_TEMP   0x0405
#define FM_INFO_SYMKEYSM6_PERM   0x0406
#define FM_INFO_SYMKEYSSF33_TEMP 0x0407
#define FM_INFO_SYMKEYSSF33_PERM 0x0408
/* asym alg key */
#define FM_INFO_ASYMKEY_TEMP     0x0501
#define FM_INFO_ASYMKEY_PERM     0x0502
#define FM_INFO_ASYMKEYRSA_TEMP  0x0503
#define FM_INFO_ASYMKEYRSA_PERM  0x0504
#define FM_INFO_ASYMKEYECC_TEMP  0x0505
#define FM_INFO_ASYMKEYECC_PERM  0x0506

/* file */
#define FM_INFO_FILESYSSIZE      0x0601   /* file systme all space */
#define FM_INFO_FILESIZE         0x0602   /* one file size */
#define FM_INFO_DIRNUM           0x0603
#define FM_INFO_FILENUM          0x0604
/* user */

/* cert */
#define FM_CERT_ENC       1
#define FM_CERT_SIGN      2
#define FM_KEYNUM_ENC     3
#define FM_KEYNUM_SIGN    4
#define FM_CONT_TYPE      5

#define FM_RSA_CERT_ENC           1
#define FM_RSA_CERT_SIGN          2
#define FM_RSA_KEYNUM_ENC         3
#define FM_RSA_KEYNUM_SIGN        4
#define FM_ECC_CERT_ENC           5
#define FM_ECC_CERT_SIGN          6
#define FM_ECC_KEYNUM_ENC         7
#define FM_ECC_KEYNUM_SIGN        8
#define FM_EMPTY_RSA_CONTAINER    9
#define FM_EMPTY_ECC_CONTAINER    10

#define FM_CONTAINER_TYPE_RSA 0
#define FM_CONTAINER_TYPE_ECC 1

#define FM_MIN(a,b) ((a) > (b) ?  (b) : (a))
#define FM_MAX(a,b) ((a) > (b) ?  (a) : (b))

/* waitfor event */
#define FM_WAITFORARRIVAL 0
#define FM_WAITFORREMOVE 3
#define FM_SETEVENT 1
#define FM_RESETEVENT 2

/****basic data type for linux*****/
//#ifdef FMCFG_OS_LINUX
#ifndef FMCFG_OS_WINDOWS
    #define HANDLE            void *
    #define DWORD             unsigned int
    #define PDWORD            unsigned int *
    #define PVOID             void *
    #define BYTE              unsigned char
    #define PBYTE             unsigned char *
    #define WORD              unsigned short
    #define BOOL              int
    #define LONG              int
    #define ULONG             unsigned int
#endif

/****KEY的日志区与隐藏区相关常量****/
#define FM_DISK_LOG_SIZE  10    //单位：MB
#define FM_DISK_HID_ALL   40    //单位：MB
#define FM_DISK_HID_STARTS   FM_DISK_LOG_SIZE * 1024 * 1024 / 512    //隐藏区的起始扇区号
#define FM_DISK_LOG_SECTORS  FM_DISK_HID_STARTS    //日志区的起始扇区号

/********tfcard兼容linux宏定义********/
#ifdef FMCFG_OS_WINDOWS
#define FM_HDEV
#define FM_PHDEV
#else
#define FM_HDEV (FM_S32)(FM_S64)
#define FM_PHDEV (FM_S32 *)(FM_S64 *)
#endif



FM_U16 FM_SWAP16_Little(FM_U16 u16Data);
FM_U32 FM_SWAP32_Little(FM_U32 u32Data);
FM_U16 FM_SWAP16_Large(FM_U16 u16Data);
FM_U32 FM_SWAP32_Large(FM_U32 u32Data);


extern FM_U32 u32TestEndian;

#ifdef  __cplusplus
    }
#endif  /* #ifdef  __cplusplus */

#endif  /* #ifndef FM_DEF_H */


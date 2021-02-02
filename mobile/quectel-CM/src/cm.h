#ifndef __CM_H__
#define __CM_H__

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <netinet/in.h>
#include <netinet/ether.h>

#include <libubox/avl.h>
#include <libubox/uloop.h>
#include <unistd.h>
#include <signal.h>
#include "libubus.h"

//#include "timing.h"

#include "QMIThread.h"

#define DEVID_LEN 32
#define ICCID_LEN 32
#define IMEI_LEN 16
#define IMSI_LEN 32
#define CAP_LEN  16
#define SIM_STAT_LEN 32 

#define CLASS_CDMA (1)
#define CLASS_HDR (1<<1)
#define CLASS_GSM (1<<2)
#define CLASS_WCDMA (1<<3)
#define CLASS_LTE (1<<4)
#define CLASS_TDSCDMA (1<<5)

//typedef enum _class{
enum {
	CM_NONE = 0x0,
	CM_CDMA = 0x10,
	CM_HDR,
	CM_GSM,
	CM_WCDMA,
	CM_LTE,
	CM_TDSCDMA
};

struct class2str {
    unsigned char class;
	const char* str;
};


struct bb {
	char devid[DEVID_LEN]; 
	char imei[IMEI_LEN]; 

	//function
	int (*read)(struct bb* bb, char * devid, char * imei);
	int (*write)(struct bb* bb, char * devid, char * imei); 
};

struct sim {
    char stat[SIM_STAT_LEN];
	char iccid[ICCID_LEN]; 
	char imsi[IMSI_LEN]; 

	//function
	int (*read)(struct sim* sim, char *stat, char *iccid, char *imsi);
	int (*write)(struct sim* sim, char *stat, char *iccid, char *imsi);
};

struct sig
{
    UCHAR cla;

	QMINAS_SIG_INFO_CDMA_TLV_MSG sig_cdma;
	QMINAS_SIG_INFO_HDR_TLV_MSG sig_hdr; 
	QMINAS_SIG_INFO_GSM_TLV_MSG sig_gsm; 
	QMINAS_SIG_INFO_WCDMA_TLV_MSG sig_wcdma; 
	QMINAS_SIG_INFO_LTE_TLV_MSG sig_lte; 
	QMINAS_SIG_INFO_TDSCDMA_TLV_MSG sig_tdscdma;
	//NR5G_SYSTEM_INFO sig_5g,
};

union signal
{
	QMINAS_SIG_INFO_CDMA_TLV_MSG sig_cdma;
	QMINAS_SIG_INFO_HDR_TLV_MSG sig_hdr; 
	QMINAS_SIG_INFO_GSM_TLV_MSG sig_gsm; 
	QMINAS_SIG_INFO_WCDMA_TLV_MSG sig_wcdma; 
	QMINAS_SIG_INFO_LTE_TLV_MSG sig_lte; 
	QMINAS_SIG_INFO_TDSCDMA_TLV_MSG sig_tdscdma;
	//NR5G_SYSTEM_INFO sig_5g,
};

struct reg_state {
    USHORT country;
    USHORT network;

	UCHAR PSAttachedState;

    //UCHAR DeviceClass; // LTE,CDMA,GSM
    //ULONG DataCapList; // LTE|EDGE|UMTS
    char DataCapStr[CAP_LEN];

	//int rssi; //except TDSCDMA
	//int clas; //LTE 0x14,CDMA 0x10,GSM

	struct sig sig;

	//function
	int (*read)(struct reg_state* reg, 
		USHORT *country, USHORT *network,
		UCHAR *attach, char *cap, int *, void * sig);

	int (*write)(struct reg_state* reg, 
		USHORT *country, USHORT *network,
		UCHAR *attach, char *cap, int , void * sig);
};

struct cm {
	//time_t refresh_interval;
	pthread_rwlock_t rwlock;
	//pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

	struct bb bb_status;
	struct sim sim_status;
	struct reg_state reg_status;

    UCHAR conn_status;

	//function
	int (*read)(struct cm* cm, UCHAR *conn_status);
	int (*write)(struct cm* cm, UCHAR conn_status);

	int (*dump)(struct cm* cm);
};

struct cm cm_status;
int cm_init(void);
int cm_destroy(void);
int cm_show(void);

#endif /* __CM_H__ */


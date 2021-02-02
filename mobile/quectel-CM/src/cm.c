#include "cm.h"

//#define UBUS_DEBUG

struct class2str clas2str[] = {
    {CM_NONE, "UNKNOWN"},
    {CM_GSM, "GPRS"},
    {CM_GSM, "EDGE"},
    {CM_WCDMA, "UMTS"},
    {CM_GSM, "HSDPA"},
    {CM_GSM, "HSUPA"},
    {CM_LTE, "LTE"},
    {CM_GSM, "5G_NSA"},
    {CM_GSM, "5G_SA"},
    {CM_CDMA, "1XRTT"},
    {CM_CDMA, "1XEVDO"},
    {CM_CDMA, "1XEVDO_REVA"},
    {CM_CDMA, "1XEVDV"},
    {CM_CDMA, "3XRTT"},
    {CM_CDMA, "1XEVDO_REVB"},
    {0, NULL},
};

UCHAR get_class(char * str)
{
    int i = 0;

    for (i = 0; clas2str[i].str != NULL; i++) {
		if (strcmp(clas2str[i].str, (const char*) str) == 0) {
            return clas2str[i].class;
		}
    }
    return CM_NONE;
}

//bb functions
int bb_read(struct bb* bb, char * devid, char * imei)
{
	if (bb == NULL)
		return -1;

	struct cm *cm = container_of(bb, struct cm, bb_status);

	pthread_rwlock_rdlock(&(cm->rwlock));
	if (devid != NULL)
		memcpy( devid, bb->devid, strlen(bb->devid));

	if (imei != NULL)
		memcpy(imei, bb->imei, strlen(bb->imei));
	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

int bb_write(struct bb* bb, char * devid, char *imei)
{
	if (bb == NULL)
		return -1;

	struct cm *cm = container_of(bb, struct cm, bb_status);

	pthread_rwlock_wrlock(&(cm->rwlock));
	if (devid != NULL)
		memcpy(bb->devid, devid, strlen(devid));

	if (imei != NULL)
		memcpy(bb->imei, imei, strlen(imei));
	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

// sim functions
int sim_read(struct sim* sim, char *stat, char *iccid, char *imsi)
{
	if (sim == NULL)
		return -1;

	struct cm *cm = container_of(sim, struct cm, sim_status);

	pthread_rwlock_rdlock(&(cm->rwlock));

	if (iccid != NULL)
		memcpy(iccid, sim->iccid, strlen(sim->iccid));

	if (imsi != NULL)
		memcpy(imsi, sim->imsi, strlen(sim->imsi));

	if (stat != NULL)
		memcpy(stat, sim->stat, strlen(sim->stat));

	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

int sim_write(struct sim* sim, char *stat, char *iccid, char *imsi)
{
	if (sim == NULL)
		return -1;

	struct cm *cm = container_of(sim, struct cm, sim_status);

	pthread_rwlock_wrlock(&(cm->rwlock));
	if (iccid != NULL)
		memcpy(sim->iccid, iccid, strlen(iccid));

	if (imsi != NULL)
		memcpy(sim->imsi, imsi, strlen(imsi));

	if (stat != NULL)
		memcpy(sim->stat, stat, strlen(stat));

	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

//reg functions
int reg_state_read(struct reg_state* reg, 
		USHORT *country, USHORT *network,
		UCHAR *attach, char *cap, 
		int *clas, void * sig)
{
	if (reg == NULL)
		return -1;

	struct cm *cm = container_of(reg, struct cm, reg_status);

	pthread_rwlock_rdlock(&(cm->rwlock));

	if (country != NULL)
		*country = reg->country ;

	if (network != NULL)
		*network = reg->network;

	if (attach != NULL)
		*attach = reg->PSAttachedState;

	if (cap != NULL)
		memcpy(cap, reg->DataCapStr, strlen(reg->DataCapStr));

    *clas = get_class(reg->DataCapStr);

	if (sig != NULL)
	{
		switch (*clas)
		{
			case CM_CDMA:// 0x10: 
				memcpy(sig, &(reg->sig.sig_cdma), sizeof(QMINAS_SIG_INFO_CDMA_TLV_MSG));
				break;
			case CM_HDR: //0x11: 
				memcpy(sig, &(reg->sig.sig_hdr), sizeof(QMINAS_SIG_INFO_HDR_TLV_MSG));
				break;
			case CM_GSM: //0x12: 
				memcpy(sig, &(reg->sig.sig_gsm), sizeof(QMINAS_SIG_INFO_GSM_TLV_MSG));
				break;
			case CM_WCDMA: //0x13: 
				memcpy(sig, &(reg->sig.sig_wcdma), sizeof(QMINAS_SIG_INFO_WCDMA_TLV_MSG));
				break;
			case CM_LTE://0x14: 
				memcpy(sig, &(reg->sig.sig_lte), sizeof(QMINAS_SIG_INFO_LTE_TLV_MSG));
				break;
			case CM_TDSCDMA: //0x15: 
				memcpy(sig, &(reg->sig.sig_tdscdma), sizeof(QMINAS_SIG_INFO_TDSCDMA_TLV_MSG));
				break;
			default:
				break;
		}
		
	}

	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

int reg_state_write(struct reg_state* reg, 
		USHORT *country, USHORT *network,
		UCHAR *attach, char *cap,
		int clas, void * sig)
{
	if (reg == NULL)
		return -1;

	struct cm *cm = container_of(reg, struct cm, reg_status);

	pthread_rwlock_wrlock(&(cm->rwlock));

	if (country != NULL)
		reg->country = *country;

	if (network != NULL)
		reg->network = *network;

	if (attach != NULL)
		reg->PSAttachedState = *attach;

	if (cap != NULL)
		memcpy(reg->DataCapStr, cap, strlen(cap));

	if (sig != NULL)
	{
		switch (clas)
		{
			case CM_CDMA:// 0x10: 
				reg->sig.cla |= CLASS_CDMA;
				memcpy(&(reg->sig.sig_cdma), sig, sizeof(QMINAS_SIG_INFO_CDMA_TLV_MSG));
				break;
			case CM_HDR: //0x11: 
				reg->sig.cla |= CLASS_HDR;
				memcpy(&(reg->sig.sig_hdr), sig, sizeof(QMINAS_SIG_INFO_HDR_TLV_MSG));
				break;
			case CM_GSM: //0x12: 
				reg->sig.cla |= CLASS_GSM;
				memcpy(&(reg->sig.sig_gsm), sig, sizeof(QMINAS_SIG_INFO_GSM_TLV_MSG));
				break;
			case CM_WCDMA: //0x13: 
				reg->sig.cla |= CLASS_WCDMA;
				memcpy(&(reg->sig.sig_wcdma), sig, sizeof(QMINAS_SIG_INFO_WCDMA_TLV_MSG));
				break;
			case CM_LTE://0x14: 
				reg->sig.cla |= CLASS_LTE;
				memcpy(&(reg->sig.sig_lte), sig, sizeof(QMINAS_SIG_INFO_LTE_TLV_MSG));
				break;
			case CM_TDSCDMA: //0x15: 
				reg->sig.cla |= CLASS_TDSCDMA;
				memcpy(&(reg->sig.sig_tdscdma), sig, sizeof(QMINAS_SIG_INFO_TDSCDMA_TLV_MSG));
				break;
			default:
				break;
		}
		
	}
	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

// cm function
int cm_read(struct cm* cm, UCHAR *conn_status)
{
	if (cm == NULL)
		return -1;

	pthread_rwlock_rdlock(&(cm->rwlock));
	*conn_status = cm->conn_status;
	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

int cm_write(struct cm* cm, UCHAR conn_status)
{
	if (cm == NULL)
		return -1;

	pthread_rwlock_wrlock(&(cm->rwlock));
    cm->conn_status = conn_status; 
	pthread_rwlock_unlock(&(cm->rwlock));

	return 0;
}

int cm_init(void)
{
	int ret = 0; 

	ret = pthread_rwlock_init(&(cm_status.rwlock), NULL);
	if (ret != 0)
		return -1;

	cm_status.read = cm_read;
	cm_status.write = cm_write;

	cm_status.bb_status.read = bb_read;
	cm_status.bb_status.write = bb_write;

	cm_status.sim_status.read = sim_read;
	cm_status.sim_status.write = sim_write;

	cm_status.reg_status.read = reg_state_read;
	cm_status.reg_status.write = reg_state_write;

	return 0;
}

int cm_destroy(void)
{
	int ret = 0; 

	ret = pthread_rwlock_destroy(&(cm_status.rwlock));
	if (ret != 0)
		return -1;

	return 0;
}

#ifdef UBUS_DEBUG
int cm_show(void)
{
	dbg_time("%s ConnectionStatus: %s", __func__, 
			(cm_status.conn_status == QWDS_PKT_DATA_CONNECTED) ? "CONNECTED" : "DISCONNECTED");

	dbg_time("%s DEVID : %s", __func__, cm_status.bb_status.devid);
	
	dbg_time("%s ICCID : %s", __func__, cm_status.sim_status.iccid);

	dbg_time("%s IMSI : %s", __func__, cm_status.sim_status.imsi);

	dbg_time("%s sim stat : %s", __func__, cm_status.sim_status.stat);

    dbg_time("%s MCC: %d, MNC: %d, PS: %s, DataCap: %s", 
			__func__,
			cm_status.reg_status.country, 
			cm_status.reg_status.network,
			(cm_status.reg_status.PSAttachedState == 1) ? "Attached" : "Detached" ,
			cm_status.reg_status.DataCapStr);

	if (cm_status.reg_status.sig.cla & CLASS_CDMA)
		dbg_time("%s CDMA: RSSI %d dBm, ECIO %.1lf dBm", __func__,
				cm_status.reg_status.sig.sig_cdma.rssi, (-0.5) * (double)cm_status.reg_status.sig.sig_cdma.ecio);

	if (cm_status.reg_status.sig.cla & CLASS_GSM)
		dbg_time("%s GSM: RSSI %d dBm", __func__, cm_status.reg_status.sig.sig_gsm.rssi);

	if (cm_status.reg_status.sig.cla & CLASS_WCDMA)
		dbg_time("%s WCDMA: RSSI %d dBm, ECIO %.1lf dBm", __func__,
				cm_status.reg_status.sig.sig_cdma.rssi, (-0.5) * (double)cm_status.reg_status.sig.sig_cdma.ecio);

	if (cm_status.reg_status.sig.cla & CLASS_LTE)
		dbg_time("%s LTE: RSSI %d dBm, RSRQ %d dB, RSRP %d dBm, SNR %.1lf dB", __func__,
				cm_status.reg_status.sig.sig_lte.rssi, cm_status.reg_status.sig.sig_lte.rsrq, cm_status.reg_status.sig.sig_lte.rsrp, (0.1) * (double)cm_status.reg_status.sig.sig_lte.snr);

	return 0;
}
#else
int cm_show(void)
{
	return 0;
}
#endif

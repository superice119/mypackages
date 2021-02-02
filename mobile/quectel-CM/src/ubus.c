/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <unistd.h>
#include <signal.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include <libubox/blobmsg_json.h>
#include "libubus.h"
#include "cm.h"

static struct ubus_context *ctx;
static struct blob_buf b;

const char *ubus_socket = "/var/run/ubus.sock";

static int 
get_cm_status(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req,
				const char *method,
                struct blob_attr *msg)
{
	time_t now;
	struct tm *tm;

	unsigned char conn;
	char imei[16];
	char devid[32];
	char iccid[32];
	char imsi[32];
	char stat[32];
	
	USHORT mcc = 0;
	USHORT mnc = 0;
	unsigned char ps;
	char cap[32];
	int clas;

	char tmp[32] = {'0'};

	union signal sig;

	struct sysinfo info;
	void *c;

	if (sysinfo(&info))
		return UBUS_STATUS_UNKNOWN_ERROR;

	now = time(NULL);

	if (!(tm = localtime(&now)))
		return UBUS_STATUS_UNKNOWN_ERROR;

	blob_buf_init(&b, 0);

	cm_status.read(&cm_status, &conn); 

	cm_status.bb_status.read(&(cm_status.bb_status), devid, imei); 

	cm_status.sim_status.read(&(cm_status.sim_status), 
			stat, iccid, imsi); 

	cm_status.reg_status.read(&(cm_status.reg_status), 
		&mcc, &mnc,
		&ps, cap, 
		&clas, (void * )&sig);

	blobmsg_add_string(&b, "connection", (conn == QWDS_PKT_DATA_CONNECTED) ? "CONNECTED" : "DISCONNECTED");

	c = blobmsg_open_table(&b, "baseband");
	blobmsg_add_string(&b, "devid", devid);
	blobmsg_add_string(&b, "imei", imei);
	blobmsg_close_table(&b, c);

	c = blobmsg_open_table(&b, "SIM");
	blobmsg_add_string(&b, "iccid", iccid);
	blobmsg_add_string(&b, "imsi", imsi);
	blobmsg_add_string(&b, "status", stat);
	blobmsg_close_table(&b, c);

	if (!strcmp(stat, "SIM_READY"))
	{
		c = blobmsg_open_table(&b, "register");
		blobmsg_add_u16(&b, "MCC", mcc);
		blobmsg_add_u16(&b, "MNC", mnc);
		blobmsg_add_string(&b, "PS", (ps == 1) ? "Attached" : "Detached" );
		blobmsg_add_string(&b, "cap", cap);
		blobmsg_close_table(&b, c);

		c = blobmsg_open_table(&b, "signal");

		if (clas != 0x0)
		{
			switch (clas)
			{
				case CM_CDMA:// 0x10: 
				case CM_WCDMA: //0x13: 
					sprintf(tmp, "%d dBm", sig.sig_cdma.rssi);

					blobmsg_add_string(&b, "rssi", tmp);

					memset(tmp, 0, 32);

					sprintf(tmp, "%.1lf dBm", (-0.5) * (double)sig.sig_cdma.ecio);
					blobmsg_add_string(&b, "ecio", tmp);
					break;
				case CM_HDR: //0x11: 
				case CM_GSM: //0x12: 
					sprintf(tmp, "%d dBm", sig.sig_gsm.rssi);
					blobmsg_add_string(&b, "rssi", tmp);
					break;
					break;
				case CM_LTE://0x14: 

					sprintf(tmp, "%d dBm", sig.sig_lte.rssi);
					blobmsg_add_string(&b, "rssi", tmp);

					sprintf(tmp, "%d dBm", sig.sig_lte.rsrq);
					blobmsg_add_string(&b, "rsrq", tmp);

					sprintf(tmp, "%d dBm", sig.sig_lte.rsrp);
					blobmsg_add_string(&b, "rsrp", tmp);

					memset(tmp, 0, 32);
					sprintf(tmp, "%.1lf dB", (0.1) * (double)sig.sig_lte.snr);
					blobmsg_add_string(&b, "snr", tmp);
					break;
				case CM_TDSCDMA: //0x15: 
					break;
				default:
					break;
			}
		}

		blobmsg_close_table(&b, c);
	}

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

static const struct ubus_method cm_methods[] = {
	UBUS_METHOD_NOARG("status",  get_cm_status),
};

static struct ubus_object_type cm_object_type =
	UBUS_OBJECT_TYPE("quectel", cm_methods);

static struct ubus_object cm_object = {
	.name = "quectel",
	.type = &cm_object_type,
	.methods = cm_methods,
	.n_methods = ARRAY_SIZE(cm_methods),
};

//static void init_ubus(void)
static void server_main(void)
{
	int ret;

	ret = ubus_add_object(ctx, &cm_object);
	if (ret)
		fprintf(stderr, "Failed to add object: %s\n", ubus_strerror(ret));

	uloop_run();
}

static void ubus_sigaction(int signo) {
	uloop_end();
}

#if 1
void * ubus_main(void * arg)
{
	const char *ubus_socket = NULL;

	uloop_init();
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, ubus_sigaction);

	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return NULL;
	}

	ubus_add_uloop(ctx);

	server_main();

	ubus_free(ctx);
	uloop_done();

	return NULL;
}
#endif

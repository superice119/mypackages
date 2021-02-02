/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/utsname.h>
#ifdef linux
#include <sys/sysinfo.h>
#endif
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <libubox/uloop.h>

static struct blob_buf b;
static int notify;
static struct ubus_context *_ctx;


static int cm_status(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
	time_t now;
	struct tm *tm;

	struct sysinfo info;
	void *c;

	if (sysinfo(&info))
		return UBUS_STATUS_UNKNOWN_ERROR;

	now = time(NULL);

	if (!(tm = localtime(&now)))
		return UBUS_STATUS_UNKNOWN_ERROR;

	blob_buf_init(&b, 0);

	blobmsg_add_u32(&b, "localtime", now + tm->tm_gmtoff);

	blobmsg_add_u32(&b, "uptime",    info.uptime);

	c = blobmsg_open_array(&b, "load");
	blobmsg_add_u32(&b, NULL, info.loads[0]);
	blobmsg_add_u32(&b, NULL, info.loads[1]);
	blobmsg_add_u32(&b, NULL, info.loads[2]);
	blobmsg_close_array(&b, c);

	c = blobmsg_open_table(&b, "memory");
	blobmsg_add_u64(&b, "total",    info.mem_unit * info.totalram);
	blobmsg_add_u64(&b, "free",     info.mem_unit * info.freeram);
	blobmsg_add_u64(&b, "shared",   info.mem_unit * info.sharedram);
	blobmsg_add_u64(&b, "buffered", info.mem_unit * info.bufferram);
	blobmsg_close_table(&b, c);

	c = blobmsg_open_table(&b, "swap");
	blobmsg_add_u64(&b, "total",    info.mem_unit * info.totalswap);
	blobmsg_add_u64(&b, "free",     info.mem_unit * info.freeswap);
	blobmsg_close_table(&b, c);

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

static const struct ubus_method cm_methods[] = {
	UBUS_METHOD_NOARG("status",  cm_status),
};

static struct ubus_object_type cm_object_type =
	UBUS_OBJECT_TYPE("quetcel", cm_methods);

static struct ubus_object system_object = {
	.name = "quetcel",
	.type = &cm_object_type,
	.methods = cm_methods,
	.n_methods = ARRAY_SIZE(cm_methods),
};

void ubus_init_system(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;
	ret = ubus_add_object(ctx, &system_object);
	if (ret)
		ERROR("Failed to add object: %s\n", ubus_strerror(ret));
}

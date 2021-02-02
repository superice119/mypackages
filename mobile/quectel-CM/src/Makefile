ifneq ($(CROSS_COMPILE),)
CROSS-COMPILE:=$(CROSS_COMPILE)
endif
#CROSS-COMPILE:=/workspace/buildroot/buildroot-qemu_mips_malta_defconfig/output/host/usr/bin/mips-buildroot-linux-uclibc-
#CROSS-COMPILE:=/workspace/buildroot/buildroot-qemu_arm_vexpress_defconfig/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-
#CROSS-COMPILE:=/workspace/buildroot-git/qemu_mips64_malta/output/host/usr/bin/mips-gnu-linux-
ifeq ($(CC),cc)
CC:=$(CROSS-COMPILE)gcc
endif
LD:=$(CROSS-COMPILE)ld

QL_CM_SRC=QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c qmap_bridge_mode.c mbim-cm.c device.c

ifeq (1,1)
QL_CM_DHCP=udhcpc.c
else
LIBMNL=libmnl/ifutils.c libmnl/attr.c libmnl/callback.c libmnl/nlmsg.c libmnl/socket.c
DHCP=libmnl/dhcp/dhcpclient.c libmnl/dhcp/dhcpmsg.c libmnl/dhcp/packet.c
QL_CM_DHCP=udhcpc_netlink.c
QL_CM_DHCP+=${LIBMNL}
endif

ifeq ($(CM_STATUS),1)
CM=cm.c 
UBUS=ubus.c
UBUS_LD=-lblobmsg_json -ljson-c -lubox -lubus -D DISABLE_LIST_INTERNAL
endif

ifeq ($(UDHCPC),1)
EN_UDHCPC= -D ENABLE_UDHCPC
else
EN_UDHCPC:=
endif

# -D SIGNAL_DEBUG

release: clean qmi-proxy mbim-proxy
	$(CC) $(CFLAGS) -Wall -s  ${QL_CM_SRC} ${QL_CM_DHCP} ${UBUS} ${CM} -o quectel-CM $(LDFLAGS) $(UBUS_LD) $(EN_UDHCPC) -lpthread -ldl

debug: clean
	$(CC) $(CFLAGS) -Wall -g -DCM_DEBUG ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-CM $(LDFLAGS) -lpthread -ldl

qmi-proxy:
	$(CC) $(CFLAGS) -Wall -s quectel-qmi-proxy.c  -o quectel-qmi-proxy $(LDFLAGS) -lpthread -ldl

mbim-proxy:
	$(CC) $(CFLAGS) -Wall -s quectel-mbim-proxy.c  -o quectel-mbim-proxy $(LDFLAGS) -lpthread -ldl

clean:
	rm -rf quectel-CM *~
	rm -rf quectel-qmi-proxy

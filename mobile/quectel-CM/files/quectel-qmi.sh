#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_quectel_init_config() {
	available=1
	no_device=1
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_string pincode
	proto_config_add_int delay
	proto_config_add_string modes
	proto_config_add_string pdptype
	proto_config_add_int profile
	proto_config_add_boolean dhcpv6
	proto_config_add_boolean autoconnect
	proto_config_add_int plmn
	proto_config_add_defaults
}

proto_quectel_setup() {
	local interface="$1"
	local auth_int=0
	local pincfg
	local exe_cmd="/usr/bin/quectel-CM"
	local device apn auth username password pincode delay modes pdptype profile dhcpv6 autoconnect plmn $PROTO_DEFAULT_OPTIONS
	local ip_6 ip_prefix_length gateway_6 dns1_6 dns2_6
	json_get_vars device apn auth username password pincode delay modes pdptype profile dhcpv6 autoconnect plmn $PROTO_DEFAULT_OPTIONS

	[ "$metric" = "" ] && metric="0"

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$device" ] || {
		echo "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	device="$(readlink -f $device)"
	[ -c "$device" ] || {
		echo "The specified control device does not exist"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	devname="$(basename "$device")"
	devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
	ifname="$(ls "$devpath"/net)"
	[ -n "$ifname" ] || {
		echo "The interface could not be found."
		proto_notify_error "$interface" NO_IFACE
		proto_set_available "$interface" 0
		return 1
	}

	[ -f /sys/class/net/$ifname/qmi/raw_ip ] || {
		echo "Device only supports raw-ip mode but is missing this required driver attribute: /sys/class/net/$ifname/qmi/raw_ip"
		return 1
	}

	echo "Device does not support 802.3 mode. Informing driver of raw-ip only for $ifname .."
	#echo "Y" > /sys/class/net/$ifname/qmi/raw_ip
	# with ec20 raw-ip patch
	echo "N" >/sys/class/net/$ifname/qmi/raw_ip

	#_get_info_status_by_at "/dev/ttyUSB2"
	#[ $? != 0 ] && {
	#       echo "Check AT failed !"
	#       proto_notify_error "$interface" CALL_FAILED
	#       proto_set_available "$interface" 0
	#       return 1
	#}

	echo "Check sleep !!!!!!!!!!!!!!!!!!!!!!!! !"
	[ -n "$delay" ] && sleep "$delay"

	#proto_notify_error "$interface" CALL_FAILED

	[ "$auth" = "none" ] && auth_int=0
	[ -n "$auth" -a "$auth" != "none" ] && auth_int=1
	

	logger -t quectel-qmi "Setting up $ifname with ${apn:+APN-$apn} [${username:+$username}-${password:+$password} ${pincode:+pin-$pincode} auth${auth:+-$auth}-$auth_int]"

	#[ -n "$apn" ] && {
	#	exe_cmd="$exe_cmd -s $apn"
	#	[ -n "$username" -a -n "$password" -a -n ${auth_int} ] && exe_cmd="$exe_cmd $username $password $auth_int"
	#	[ -n "$pincode" ] && exe_cmd="$exe_cmd -p $pincode"
	#}

	exe_cmd="$exe_cmd &"

	/bin/pidof quectel-CM && killall -INT quectel-CM && sleep 1

	proto_export "INTERFACE=${interface}"
	proto_run_command "${interface}" quectel-CM \
		${apn:+-s $apn} \
		${username:+ $username} \
		${password:+ $password} \
		${pincode:+-p $pincode} \
		$auth_int 


	#eval ${exe_cmd}
	logger -t quectel-qmi "run: $exe_cmd"
	sleep 1

	/bin/pidof quectel-CM && {
		json_init
		json_add_string name "${interface}_4"
		json_add_string ifname "@$interface"
		json_add_string proto "dhcp"
		proto_add_dynamic_defaults
		json_close_object
		ubus call network add_dynamic "$(json_dump)"
	}

	echo "Setting up $ifname"

	proto_init_update "$ifname" 1
	proto_send_update "$interface"

	return 0
}

proto_quectel_teardown() {
	local interface="$1"

	#[ -n "$ctl_device" ] && device=$ctl_device

	echo "Stopping network $interface"

	proto_kill_command "$interface"

	#json_load "$(ubus call network.interface.$interface status)"
	#json_select data
	#json_get_vars cid_4 pdh_4 cid_6 pdh_6

	#/bin/pidof quectel-CM && killall -INT quectel-CM && sleep 1

	#proto_init_update "*" 0
	#proto_send_update "$interface"
}


[ -n "$INCLUDE_ONLY" ] || {
	add_protocol quectel
}

#!/usr/bin/lua

--require "uci"
require "ubus"
require "uloop"
require "luci.sys"

log = require "nixio"

--open sys-logging
log.openlog("Led-4G", "ndelay", "cons", "nowait");

--[[
root@Vhalo:~# ubus call quetcel status
{
"connection": "CONNECTED",
"baseband": {
"devid": "EC20CEFHKGR06A04M2G",
"imei": "none"
},
"SIM": {
"iccid": "89860319750107164920",
"imsi": "460110012940140",
"status": "SIM_READY"
},
"register": {
"MCC": 460,
"MNC": 11,
"PS": "Attached",
"cap": "LTE"
},
"signal": {
"rssi": "-51 dBm",
"rsrq": "-4 dBm",
"rsrp": "-74 dBm",
"snr": "28.2 dB"
}
}

--]]

local timer
local method = "qmi"

uloop.init()

function get_status_by_at()

	local status = {}
	local block = io.popen("gcom", "r")
	local ln, dev, devices = nil, nil, {}
	local SIM = {}

	repeat
		ln = block:read("*l")
		sim = ln and ln:match("^SIM ready")

		if sim then
			status = {
				SIM = {
					status = "SIM_READY"
				}
			}
		end

		local reg = {}                         
		local sig = {}                                                       
		local rssi

		s = ln and ln:match("^Signal Quality:")
		if s then                                                        
			local signal = ln:match("%d+")                               
			--print (signal .. "  ")                                                           

			SIM["status"] = "SIM_READY"                                                      
			stat["SIM"] = SIM                                                                

			--TODO: valid ? 
			reg["PS"] = "Attached"                                                           
			reg["cap"] = "LTE/GSM"                                           

			stat["register"] = reg                                   

			--TODO: valid ? 
			rssi = "%d" % signal                                                             

			sig["rssi"] = rssi                                                               

			stat["signal"] = sig                                                             

			--print (stat.SIM.status)
		end

	until not ln

	block:close()

	return status

end


function get_status()
	local status

	ubus = ubus.connect()
	if not ubus then
		error("Failed to connect to ubusd")
	end

	status = ubus:call("quetcel", "status", {})

	if status == nil then
		method = "at"
		status = get_status_by_at()
	end

	ubus:close()

	return status
end

-- check sim status
function check_sim(status)
	log.syslog("info","check sim status")

	if ( "SIM_READY" == status.SIM.status ) then
		return true
	else
		return false
	end
end

--
-- check register status
-- 
function check_register(status)
	log.syslog("info","check register status")

	if ( "Attached" == status.register.PS and "UNKNOW" !=  status.register.cap ) then
		return true
	else
		return false
	end

end

-- 
-- get rssi
--
--[[
2G / 3G

>= -70 dBm	Excellent
-70 dBm to -85 dBm	Good
-86 dBm to -100 dBm	Fair
< -100 dBm	Poor
-110 dBm	No signal

4G

> -65 dBm	Excellent
-65 dBm to -75 dBm	Good
-75 dBm to -85 dBm	Fair
-85 dBm to -95 dBm	Poor
<= -95 dBm	No signal

-- try at command 
-- use gcom -d /dev/ttyUSB2 sig

--]]
function get_signal(status)
	log.syslog("info","check get signal")


	if ("qmi" == method) then
		print(status.signal.rssi)
		local rssi = tonumber(string.match(status.signal.rssi, "%-?%d+"))

		if ( rssi >= -70 ) then
			return "Excellent"
		elseif ( rssi >= -85 and rssi <= -70 ) then
			return "Good"
		elseif ( rssi >= -100 and rssi <= -86 ) then
			return "Fair"
		else
			return "Poor"
		end
	else
		print(status.signal.rssi)
		local rssi = tonumber(string.match(status.signal.rssi, "%d+"))
		if ( rssi >= -70 ) then
			return "Excellent"
		elseif ( rssi >= -85 and rssi <= -70 ) then
			return "Good"
		elseif ( rssi >= -100 and rssi <= -86 ) then
			return "Fair"
		else
			return "Poor"
		end
	end
end

--[[
echo $1 > /sys/class/leds/industry-M105WL32M256M-v1\:4G-high\:status/brightness
echo $2 > /sys/class/leds/industry-M105WL32M256M-v1\:4G-mid\:status/brightness
echo $3 > /sys/class/leds/industry-M105WL32M256M-v1\:4G-low\:status/brightness
--]]
function led_set(high, mid, low)
	luci.sys.call("echo '%d' > /sys/class/leds/industry-M105WL32M256M-v1\:4G-high\:status/brightness" % high)
	luci.sys.call("echo '%d' > /sys/class/leds/industry-M105WL32M256M-v1\:4G-mid\:status/brightness" % mid)
	luci.sys.call("echo '%d' > /sys/class/leds/industry-M105WL32M256M-v1\:4G-low\:status/brightness" % low)
end

--
-- control led
--
function led_ctrl(strength)
	log.syslog("info","led control")

	-- initial leds 
	--led_set(0, 0, 0)

	if ( "Excellent" == strength ) then
		led_set(1, 1, 1)
	elseif  ( "Fair" == strength ) then
		led_set(0, 1, 1)
	else 
		led_set(0, 0, 1)
	end

end

--
-- control led
--
function led_signal(status)
	local status = get_status()

	if (check_sim(status)) then
		if (check_register(status) and check_register(status)) then
			led_ctrl(get_signal(status))
		end
	else
		-- sim error
		led_set(0, 0, 0)
	end

	timer:set(5 * 1000)
end


timer = uloop.timer(led_signal)
timer:set(5 * 1000)

uloop.run()


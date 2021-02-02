#!/bin/sh

module="FM_CPC_DRV"
device="FM_CPC_DRV"
mode="666"

#remove the lock file
rm -f /tmp/FM_FILE.LCK
rm -f /tmp/FM_PROCESS.LCK

#rmmod before insmod
echo "Check old driver and unload it."
check=`lsmod | grep FM_CPC_DRV`
if [ "$check" != "" ]; then
	echo "rmmod FM_CPC_DRV"
	/sbin/rmmod FM_CPC_DRV
fi

#invoke insmod with all arguments we got
echo "Install driver."
insmod ./$module.ko DEBUGLEVEL=3 || exit 1

major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
if [ "$major" = "" ]; then
  echo "Card not detected!"
  exit
fi

#remove stale nodes
rm -f /dev/${device}[0-5]

#mknod 0-5
mknod /dev/${device}0  c $major 0
mknod /dev/${device}1  c $major 1
mknod /dev/${device}2  c $major 2
mknod /dev/${device}3  c $major 3
mknod /dev/${device}4  c $major 4
mknod /dev/${device}5  c $major 5

#chmod
chmod $mode /dev/$device[0-5]

exit 0

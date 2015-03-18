#!/bin/sh

do_brcm2708_boot() {
	modprobe nls_cp437
	modprobe nls_iso8859-1
	modprobe vfat
	mkdir -p /boot
	mount /dev/mmcblk0p1 /boot
	if [ -f /boot/sysupgrade.tgz ]
	then
		mv /boot/sysupgrade.tgz /
	fi
}

boot_hook_add preinit_mount_root do_brcm2708_boot

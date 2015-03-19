mmcdev=mmc-bcm2835.0
mmcdir=/sys/bus/platform/drivers/mmc-bcm2835
mmcbootpart=mmcblk0p1


platform_get_magic() {
	(get_image "$@" | dd bs=1 skip=$((0x1be+4)) count=$((0x200-0x1be-4)) | hexdump -v -e '1/1 "%02x"') 2>/dev/null
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	case "$(platform_get_magic "$1")" in
	0c??????????????????????????????83??????????????????????????????????????????????????????????????????????????????????????55aa)
		return 0
		;;
	esac
	return 1
}

platform_do_upgrade() {
	sync
	cat $1 >/dev/mmcblk0
	sync
}

platform_copy_config() {
	mount /dev/$mmcbootpart /mnt || return
	cp -af "$CONF_TAR" /mnt/
	umount /mnt || return
	sync
}
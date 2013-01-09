platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1
	if [ "$(get_magic_long "$1")" != "68737173" ]
	then
		echo "Invalid image type."
		return 1
	fi
	return 0
}

platform_do_upgrade() {
	mtd write "$1" rootfs
}

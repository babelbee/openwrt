#! /bin/sh
append DRIVERS "owl"

find_owl_phy() {
	local device="$1"
	
	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	config_get phy "$device" phy
	[ -z "$phy" -a -n "$macaddr" ] && {
		for phy in $(ls /sys/devices/virtual/net 2>/dev/null); do
			[ "$macaddr" = "$(cat /sys/devices/virtual/net/${phy}/address)" ] || continue
			config_set "$device" phy "$phy"
			break
		done
		config_get phy "$device" phy
	}
	[ -n "$phy" -a -d "/sys/devices/virtual/net/$phy" ] || {
		echo "PHY for wifi device $1 not found"
		return 1
	}
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/devices/virtual/net/${phy}/address)"
	}
	return 0
}

disable_owl() {
	echo disable_owl $1
	local device="$1"
	find_owl_phy "$device" || return 0
	iface=$phy
	[ -f "/var/run/$iface.pid" ] && kill $(cat /var/run/$wdev.pid) >&/dev/null 2>&1                                                     
	for pid in `pidof wpa_supplicant`; do                                                                                              
		grep "$iface" /proc/$pid/cmdline >/dev/null && \                                                                            
			kill $pid                                                                                                          
	done                                                                                                                               
	ifconfig $iface down
}

enable_owl() {
	echo enable_owl $1
	local device="$1"
	config_get vifs "$device" vifs
	find_owl_phy "$device" || return 0
	echo "vifs=$vifs phy=$phy"
	local first=1
	for vif in $vifs
	do
		ifname=$phy
		config_set "$vif" ifname "$ifname"
		config_get mode "$vif" mode
		config_get ssid "$vif" ssid
		case "$mode" in
			sta)
				iwconfig "$ifname" mode managed
				config_get addr "$device" bssid
				[ -z "$addr" ] || {
					iwconfig "$ifnme" ap "$addr"
				}
			;;
			*) iwconfig "$phy" mode "$mode";;
		esac
		[ "$first" = 1 ] && {
			config_get rate "$vif" rate
			[ -n "$rate" ] && iwconfig "$phy" rate "${rate%%.*}"
			
			config_get frag "$vif" frag
			[ -n "$frag" ] && iwconfig "$phy" frag "${frag%%.*}"
			
			config_get rts "$vif" rts
			[ -n "$rts" ] && iwconfig "$phy" rts "${rts%%.*}"
			
		}
		
		config_get enc "$vif" encryption
		case "$enc" in
			WEP|wep)
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$ifname" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				key="${key:-1}"
				case "$key" in
					[1234]) iwconfig "$ifname" enc "[$key]";;
					*) iwconfig "$ifname" enc "$key";;
				esac
			;;
		esac
		
		local net_cfg
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			start_net "$ifname" "$net_cfg"
		}
		set_wifi_up "$vif" "$ifname"
		
		case "$mode" in
			sta)
				if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
					wpa_supplicant_setup_vif "$vif" wext || {
						echo "enable_owl($device): Failed to set up wpa_supplicant for interface $ifname" >&2
						ifconfig "$ifname" down
						continue
					}
				fi
			;;
		esac
		first=0
	done
}

scan_owl() {
	echo scan_owl $1
	local device="$1"
	local adhoc sta ap monitor mesh disabled
	
	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled = 0 ] || continue
		config_get mode "$vif" mode
		case "$mode" in
			sta)
				append $mode "$vif"
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done
	
	config_set "$device" vifs "${ap:+$ap }${adhoc:+$adhoc }${sta:+$sta }${monitor:+$monitor }${mesh:+$mesh}"
}

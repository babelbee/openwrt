#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# This Makefile is a skeleton
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=owl-linux
PKG_VERSION:=1.0.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://linux.hd-wireless.se/pub/Linux/DownloadDrivers/
PKG_MD5SUM:=3dad6e132568192421660d83a80e8d2b

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/owl
  SUBMENU:=Network Devices
  TITLE:=owl linux driver
  URL:=http://linux.hd-wireless.se
  FILES:= \
	$(PKG_BUILD_DIR)/owl.ko
  AUTOLOAD:=$(call AutoLoad,91,owl)
endef

define Package/owl/description
  OWL WLAN SDIO driver
endef

define Build/Compile
	# pass CFLAGS again to override -O2 set by configure
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE="$(KERNEL_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		KERNELDIR=$(LINUX_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" 
endef

define KernelPackage/owl/install
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/owl.sh $(1)/lib/wifi/owl.sh
endef

$(eval $(call KernelPackage,owl))

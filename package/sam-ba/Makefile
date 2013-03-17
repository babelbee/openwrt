#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=sam-ba
PKG_VERSION:=2.12
PKG_RELEASE:=1

PKG_BUILD_DIR:= $(BUILD_DIR)/sam-ba_cdc_cdc_linux
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).zip
PKG_SOURCE_URL:=http://www.atmel.com/Images
PKG_MD5SUM:=18d74822d979375a52ae4e345b8d1506

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
  $(MAKE) -C $(PKG_BUILD_DIR)/applets/legacy/sam-ba_applets/lowlevelinit CROSS_COMPILE=arm-openwrt-linux- BOARD=at91sam9g20-ek CHIP=at91sam9g20 sdram
endef

define Package/sam-ba
  SECTION:=base
  CATEGORY:=Boot Loaders
  TITLE:=sam-ba
  DEFAULT:=n
endef

define Package/sam-ba/description
  Atlmel SAM-BA In System Programmer
endef


define Package/sam-ba/install
  false
endef

$(eval $(call BuildPackage,sam-ba))

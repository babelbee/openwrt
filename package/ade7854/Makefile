# 
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ade7854
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ade7854
  SECTION:=utils
  CATEGORY:=Base system
  DEPENDS:=+kmod-ade7854
  TITLE:=ade7854 tool
  PKGARCH:=all
endef

define Package/ade7854/description
  A tool to interact with the ade7854 power measurement chip
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/ade7854/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ade7854 $(1)/usr/bin/ade7854
	$(INSTALL_BIN) ./files/ade7854.conf $(1)/etc/ade7854.conf
	$(INSTALL_BIN) ./files/ade7854.init $(1)/etc/init.d/ade7854
endef

$(eval $(call BuildPackage,ade7854))

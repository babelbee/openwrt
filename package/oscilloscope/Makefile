# 
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oscilloscope
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/oscilloscope
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=oscilloscope tool
  PKGARCH:=all
endef

define Package/oscilloscope/description
  A openwrt extension to display an oscilloscope image
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/oscilloscope/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller/admin
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/admin_status
	$(INSTALL_DIR) $(1)/www/luci-static/resources
	$(INSTALL_BIN) ./files/oscilloscope.lua $(1)/usr/lib/lua/luci/controller/admin/oscilloscope.lua
	$(INSTALL_BIN) ./files/oscilloscope.htm $(1)/usr/lib/lua/luci/view/admin_status/oscilloscope.htm
	$(INSTALL_BIN) ./files/oscilloscope.svg $(1)/www/luci-static/resources/oscilloscope.svg
endef

$(eval $(call BuildPackage,oscilloscope))

#
# Copyright (C) 2016 Tobias Ilte <tobias.ilte@campus.tu-berlin.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=led_4g
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/${PKG_NAME}
        CATEGORY:=V-secure
        SUBMENU:=ToB
        DEPENDS:=+lua +libubus-lua +libuci-lua +libubox-lua +luci-lib-nixio
        TITLE:= 4G Led manager
endef

define Package/${PKG_NAME}/description
endef

#define Package/${PKG_NAME}/conffiles
#/etc/config/led_4g
#endef

define Build/Compile
endef

define Package/${PKG_NAME}/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./src/led_4g.lua $(1)/usr/sbin/led_4g
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./src/led_4g.init $(1)/etc/init.d/led_4g
endef

$(eval $(call BuildPackage,${PKG_NAME}))

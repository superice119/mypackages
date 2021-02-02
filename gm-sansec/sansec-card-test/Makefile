#
# Copyright (C) 2009-2010 Jo-Philipp Wich <xm@subsignal.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sansec-card-test
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
        CATEGORY:=V-secure
        SUBMENU:=ToB
        TITLE:=Userspace port of the sansec crypto card test tool
        DEPENDS:=+libusb-1.0
endef

define Package/$(PKG_NAME)/description
 Userspace port of the fisherman crypto card test tool.
endef

#define Build/Prepare
#endef
#
#define Build/Configure
#endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) ./files/TestSDS_v1.3_i486_openwrt $(1)/usr/bin/TestSDS_sansec
	$(INSTALL_BIN) ./files/swcsmmgmt_csm42_v4.8.1_i486_openwrt $(1)/usr/bin/swcsmmgmt_sansec
	$(INSTALL_BIN) ./files/libswsds.so_csm42_v2.9.2.0_i486_openwrt $(1)/usr/lib/libswsds.so
	$(INSTALL_BIN) ./files/libswhkey.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

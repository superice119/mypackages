#
# Copyright (C) 2009-2010 Jo-Philipp Wich <xm@subsignal.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fm-cpc-test-glibc
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
        CATEGORY:=V-secure
        SUBMENU:=ToB
        TITLE:=Userspace port of the fisherman crypto card test tool
        DEPENDS:=+libpthread +libc
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
	$(INSTALL_BIN) ./files/TestCard $(1)/usr/bin/
	$(INSTALL_BIN) ./files/pintool $(1)/usr/bin/
	$(INSTALL_BIN) ./files/TestSGD $(1)/usr/bin/
	$(INSTALL_DATA) ./files/SDFDevice.conf $(1)/etc/
	$(INSTALL_BIN) ./files/libfmapiv100.so $(1)/usr/lib/
	$(INSTALL_BIN) ./files/libsgd.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

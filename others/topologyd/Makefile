#
# Copyright (C) 2018 iopsys
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=topologyd
PKG_VERSION:=0.1

PKG_SOURCE_VERSION:=23b812bb512189487484182ab736e957d54da9f3
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://dev.iopsys.eu/iopsys/topologyd.git

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/topologyd
	CATEGORY:=Utilities
	DEPENDS:=+libubox +ubus +libpthread +libopenssl +libnl-genl +libnl +libuci +libwifi
	TITLE:=topology daemon
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include

define Package/topologyd/description
	gathers topology information and presents it as json structure
endef

define Package/topologyd/install
	$(CP) ./files/* $(1)/
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/topologyd $(1)/sbin/
endef

$(eval $(call BuildPackage,topologyd))

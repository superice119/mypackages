#
# Copyright (C) 2015-2018 neil.zhang@siflower.com.cn
#
# sifiower Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=quectel-CM
PKG_VERSION:=1.6.0.16
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/quectel-CM
        CATEGORY:=V-secure
        SUBMENU:=ToB
        TITLE:=quectel-CM
        DEPENDS:= +libpthread +libubus +libubox +libblobmsg-json 
endef

define Package/quectel-CM/description
EC20 quectel-CM for GobiNetUSB & QMI WWAN driver.
endef

define Package/quectel-CM/config
  if PACKAGE_quectel-CM
    config ENABLE_UDHCPC
      bool "Enable udhcpc internal"
      default n

    config ENABLE_CM_STATUS
      bool "Enable ubus status interface"
      default n
  endif
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

ifeq ($(CONFIG_ENABLE_UDHCPC),y)
export UDHCPC=1
endif

ifeq ($(CONFIG_ENABLE_CM_STATUS),y)
export CM_STATUS=1
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -Werror -mips32r2 -s" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/quectel-CM/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/quectel-CM $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/lib/netifd/proto/
	$(INSTALL_BIN) ./files/quectel-qmi.sh $(1)/lib/netifd/proto/
endef


$(eval $(call BuildPackage,quectel-CM))

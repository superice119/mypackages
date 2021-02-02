#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sm4-fm
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/$(PKG_NAME)
  CATEGORY:=V-secure
  SUBMENU:=ToB
  TITLE:= sm4 kernel driver
  FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).ko
  $(call AddDepends/crypto)
endef

define KernelPackage/$(PKG_NAME)/description
 This is a addition for the following in-kernel drivers:
 1) sm4 - sync crypto 

endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	EXTRA_CFLAGS+="-I$(PKG_BUILD_DIR)/include" \
	SUBDIRS="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,$(PKG_NAME)))

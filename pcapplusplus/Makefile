#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpcapplusplus
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/seladb/PcapPlusPlus/archive/
PKG_SOURCE_VERSION:=19.12
PKG_SOURCE:=v$(PKG_SOURCE_VERSION).tar.gz
PKG_HASH:=9bebe2972a6678b8fb80f93b92a3caf9babae346137f2171e6941f35b56f88bb

PKG_LICENSE:=UNLICENSE

PKG_BUILD_DIR:=$(BUILD_DIR)/PcapPlusPlus-$(PKG_SOURCE_VERSION)
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libpcapplusplus
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=C++ wrapper for libpcap
  DEPENDS:=+libpcap +libstdcpp
  URL:=https://pcapplusplus.github.io/
endef

define Package/libpcapplusplus/description
PcapPlusPlus enables capturing and sending network packets 
through easy-to-use C++ wrappers for the most popular packet 
processing engines such as libpcap, WinPcap, Npcap, DPDK and PF_RING
endef

CONFIGURE_CMD  = ./configure-linux.sh
CONFIGURE_ARGS = --default --install-dir=$(PKG_INSTALL_DIR)

# The configure script throws an error if the install-dir does not exist

define Build/Configure
	mkdir -p $(PKG_INSTALL_DIR)
	$(call Build/Configure/Default)
	$(SED) 's|-I/usr/include/netinet|-I$(TOOLCHAIN_DIR)/include/netinet|g' `grep -lr "\-I/usr/include/netinet" $(PKG_BUILD_DIR)`
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/include/pcapplusplus $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/lib{Common++,Packet++,Pcap++}.a $(1)/usr/lib/
endef

define Package/libpcapplusplus/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/Dist/examples/TcpReassembly $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libpcapplusplus))



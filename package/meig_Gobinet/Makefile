#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gobinet
PKG_VERSION:=1.4.2
PKG_RELEASE:=1

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/gobinet
  SUBMENU:=WWAN Support
  CATEGORY:=LINK4ALL
  TITLE:=Meig Linux USB Gobinet Driver
  DEPENDS:=+kmod-usb-net
  FILES:=$(PKG_BUILD_DIR)/GobiNet.ko
  AUTOLOAD:=$(call AutoLoad,81,GobiNet)
endef

define KernelPackage/gobinet/description
  Meig Linux USB gobinet Driver
endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	CXXFLAGS="$(TARGET_CXXFLAGS)" \
	M="$(PKG_BUILD_DIR)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,gobinet))

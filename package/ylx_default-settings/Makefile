#
# Copyright (C) 2016-2023 GitHub 
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=ylx-default-settings
PKG_VERSION:=1
PKG_RELEASE:=0
PKG_LICENSE:=GPLv3
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/ylx-default-settings
  CATEGORY:=LINK4ALL
  TITLE:=ylx default settings
endef


define Build/Compile
endef

define Package/ylx-default-settings/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/zzz-ylx-default-settings $(1)/etc/uci-defaults/99-ylx-default-settings
endef

$(eval $(call BuildPackage,ylx-default-settings))

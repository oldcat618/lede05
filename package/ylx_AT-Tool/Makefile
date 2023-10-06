include $(TOPDIR)/rules.mk

PKG_NAME:= ylx_AT-Tool
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ylx_AT-Tool
  CATEGORY:=LINK4ALL
  TITLE:=ylx_AT-Tool
#   DEPENDS:=+kmod-qmi_wwan_q 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		M="$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CC)" \
		LIBS="$(TARGET_LDFLAGS)"
endef

define Package/ylx_AT-Tool/install
	$(INSTALL_DIR) $(1)/usr/bin 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ylx_AT-Tool $(1)/usr/bin
endef

$(eval $(call BuildPackage,ylx_AT-Tool))
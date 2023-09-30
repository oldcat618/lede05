include $(TOPDIR)/rules.mk

PKG_NAME:= YLX_CallManager
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/YLX_CallManager
  CATEGORY:=LINK4ALL
  TITLE:=YLX_CallManager
  DEPENDS:=+kmod-qmi_wwan_q +kmod-qmi_wwan_f +kmod-qmi_wwan_m \
			+quectel-CM-5G +Meig-CM +fibocom-dial \
			+jq #+libjson-c
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
		LIBS="$(TARGET_LDFLAGS) #-lfros_api -ljson-c"
endef

define Package/YLX_CallManager/install
	$(INSTALL_DIR) $(1)/etc/init.d 
	$(INSTALL_DIR) $(1)/usr/bin 
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/etc/init.d/YLX_CallManager.sh		$(1)/etc/init.d/YLX_CallManager
	$(INSTALL_BIN) ./files/sbin/YLX_CallManager_ColdPlug.sh		$(1)/sbin/YLX_CallManager_ColdPlug.sh
	$(INSTALL_CONF)	./files/etc/config/modem_match.json			$(1)/etc/config/modem_match.json
	$(INSTALL_BIN) ./files/etc/hotplug.d/usb/YLX_CallManager_HotPlug.sh		$(1)/etc/hotplug.d/usb/YLX_CallManager_HotPlug.sh	
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/YLX_CallManager $(1)/usr/bin
endef

$(eval $(call BuildPackage,YLX_CallManager))
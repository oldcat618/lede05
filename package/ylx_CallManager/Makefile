include $(TOPDIR)/rules.mk

PKG_NAME:= ylx_CallManager
PKG_VERSION:=1.2.0
PKG_RELEASE:=3

include $(INCLUDE_DIR)/package.mk

define Package/ylx_CallManager
  CATEGORY:=LINK4ALL
  TITLE:=ylx_CallManager
  DEPENDS:=+kmod-qmi_wwan_q +kmod-qmi_wwan_f +kmod-qmi_wwan_m \
			+quectel-CM-5G +Meig-CM +fibocom-dial \
			+jq +bash +libuci +cJSON +libfros_common 
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
		LIBS="$(TARGET_LDFLAGS) -luci  -lcjson -lfros_common"
endef

define Package/ylx_CallManager/install
	$(INSTALL_DIR) $(1)/etc/init.d 
	$(INSTALL_DIR) $(1)/usr/bin 
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/etc/uci-defaults/ylx_CallManagerDefaults $(1)/etc/uci-defaults/z-ylx_CallManagerDefaults
	$(INSTALL_BIN) ./files/etc/init.d/ylx_CallManager.sh		$(1)/etc/init.d/ylx_CallManager
	$(INSTALL_BIN) ./files/sbin/ylx_CallManager_ColdPlug.sh		$(1)/sbin/ylx_CallManager_ColdPlug.sh
	$(INSTALL_BIN) ./files/sbin/ylx_ModemState_Detect.sh		$(1)/sbin/ylx_ModemState_Detect.sh
	$(INSTALL_BIN) ./files/sbin/ylx_ModemNetwork_Update.sh		$(1)/sbin/ylx_ModemNetwork_Update.sh
	$(INSTALL_CONF)	./files/etc/config/modem_match.json			$(1)/etc/config/modem_match.json
	$(INSTALL_CONF)	./files/etc/config/modem_dial				$(1)/etc/config/modem_dial
	$(INSTALL_BIN) ./files/etc/hotplug.d/usb/ylx_CallManager_HotPlug.sh		$(1)/etc/hotplug.d/usb/ylx_CallManager_HotPlug.sh	
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ylx_CallManager $(1)/usr/bin
endef

$(eval $(call BuildPackage,ylx_CallManager))
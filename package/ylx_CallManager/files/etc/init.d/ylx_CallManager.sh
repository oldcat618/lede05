#!/bin/sh /etc/rc.common
# Copyright (C) 2006-2011 OpenWrt.org

START=99

USE_PROCD=1
PROG=/usr/bin/ylx_CallManager

start_service() {
        procd_open_instance
        procd_set_param command "$PROG"
        procd_set_param respawn
        procd_close_instance

        # /sbin/ylx_CallManager_ColdPlug.sh 2>/dev/null
}

stop_service(){
    service_stop "$PROG"
}


#!/bin/sh /etc/rc.common
# Copyright (C) 2006-2011 OpenWrt.org

START=99

USE_PROCD=1
PROG=/usr/bin/ylx_CallManager

restart_modem()
{
    switch_list=$(ls /sys/class/gpio/power_mod* -d)
    for i in $switch_list; do 
        echo 0 > $i/value 
    done
    sleep 3
    for i in $switch_list; do 
        echo 1 > $i/value 
        sleep 5
    done
}

start_service() {
        # procd_open_instance
        # procd_set_param command "$PROG"
        # procd_set_param respawn
        # procd_close_instance
        # restart_modem &
        # /sbin/ylx_CallManager_ColdPlug.sh 2>/dev/null
        $PROG &
}

stop_service(){
    echo "service_stop "$PROG""
}


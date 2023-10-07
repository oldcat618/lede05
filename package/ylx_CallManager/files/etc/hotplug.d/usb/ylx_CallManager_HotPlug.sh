#!/bin/sh


# ipc_doc=/dev/console
ipc_doc=/tmp/modem-ipc
# lock_doc=/tmp/modem-lock
modemjson="/tmp/modem.json"

if [ ! -p "$ipc_doc" ]; then
    mkfifo $ipc_doc
fi

update_dial_conf() 
{
    if [ -z "$1" ]; then
        return 1
    fi
    
    target_path=$1

    if uci show modem_dial | grep -E "^modem_dial.@dial\[[0-9]+\].path='$target_path'" >/dev/null; then
        echo "The 'dial' node with path '$target_path' exists."
    else
        echo "The 'dial' node with path '$target_path' does not exist, create the new 'dial' node"
        
        uci add modem_dial dial
        uci set modem_dial.@dial[-1].path=$target_path
        uci set modem_dial.@dial[-1].custom="$(uci get modem_dial.global.custom)"
        uci set modem_dial.@dial[-1].apn="$(uci get modem_dial.global.apn)"
        uci set modem_dial.@dial[-1].user="$(uci get modem_dial.global.user)"
        uci set modem_dial.@dial[-1].passwd="$(uci get modem_dial.global.passwd)"
        uci set modem_dial.@dial[-1].auth="$(uci get modem_dial.global.auth)"
        uci set modem_dial.@dial[-1].ipv4="$(uci get modem_dial.global.ipv4)"
        uci set modem_dial.@dial[-1].ipv6="$(uci get modem_dial.global.ipv6)"
        uci set modem_dial.@dial[-1].pincode="$(uci get modem_dial.global.pincode)"

        uci commit modem_dial
    fi
}

update_modem_leds()
{
    if [ -z "$1" ]; then
        return 1
    fi
    
    netdev=$1
    uci set system.led_modem.dev="$netdev"
    uci set system.led_wan.dev=$(uci get network.wan.ifname)

    uci  commit system
    /etc/init.d/leds reload
}

if [[ "$DEVICENAME" != *":"* ]]; then

        vid=$(echo "$PRODUCT" | awk -F'/' '{print $1}')
        pid=$(echo "$PRODUCT" | awk -F'/' '{print $2}')


        if [ ${#pid} -eq 3 ]; then
                pid="0$pid"
        fi

        case "$ACTION" in
                "bind")
                        tty_list=$(ls -d /sys/bus/usb/devices/$DEVICENAME/${DEVICENAME}*/tty?* /sys/bus/usb/devices/$DEVICENAME/${DEVICENAME}*/tty/tty?* | sed "s/.*\///g" | tr "\n" " ")
                        ifc=$(find /sys$DEVPATH  -maxdepth 3 | grep net/ | awk -F/ '{print $(NF)}')
                        # netpath=$(readlink /sys/class/net/$ifc)
                        driver=$(grep DRIVER /sys/class/net/$ifc/device/uevent | cut -d= -f2)
                        # ctl_device=/dev/$(cd /sys/class/net/$ifc/; find ../../../ -name ttyUSB* |xargs -n1 basename | head -n1)
                        
                        atidx=$(jq -r --arg vid "$vid" --arg pid "$pid" '.["\($vid)\($pid)"].atIdx' /etc/config/modem_match.json)
                        if [ -z "$atidx" ]; then
                                exit 1
                        fi
			case "$driver" in
                                *cdc*)
                                        atidx=$((atidx + 1))
                                        ;;
                                # *) 
                                #         continue
                                # ;;
                        esac

                        atDev=$(echo "$tty_list" | awk '{print $'$atidx'}')

                        if [[ -n "$ifc" && -n "$driver" ]]; then
                                base=/sys$DEVPATH
                                msg="insert $vid $pid $ifc $atDev $driver $base"
                                # exec 200<>$lock_doc
                                # flock -n 200
                                update_dial_conf $base
                                update_modem_leds $ifc
                                echo "$msg" > $ipc_doc
                                # flock -u 200
                                # exec 200>&-
                        fi
                ;;
                "remove")
                        msg="remove $vid $pid /sys$DEVPATH"
                        # exec 200<>$lock_doc
                        # flock -n 200
                        echo "$msg" > $ipc_doc
                        # flock -u 200
                        # exec 200>&-
                ;;
        esac
fi
        

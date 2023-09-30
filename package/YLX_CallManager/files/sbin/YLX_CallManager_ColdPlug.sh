#!/bin/sh

ipc_doc=/tmp/modem-ipc
lock_doc=/tmp/modem-lock

if [ ! -p "$ipc_doc" ]; then
    mkfifo $ipc_doc
fi

for ifc in $(ls /sys/class/net/ | grep -e wwan -e usb -e eth); do
    driver=$(grep DRIVER /sys/class/net/$ifc/device/uevent | cut -d= -f2)

    case "$driver" in
        GobiNet|qmi_wwan*|cdc_mbim|sierra_net|cdc_ether|*cdc_ncm)
            path=$(readlink /sys/class/net/$ifc)
            if [ -n "$path" ]; then
                node=$(echo "$path" | awk -F'/' '{print $(NF-3)}')
                tty_list=$(ls -d /sys/bus/usb/devices/$node/${node}*/tty?* /sys/bus/usb/devices/$node/${node}*/tty/tty?* | sed "s/.*\///g" | tr "\n" " ")
                base=$(echo "$path" | awk -F'/' 'NF-=3' OFS='/'| sed 's|../../|/sys/|')
                pid=$(cat $base/idProduct)
                vid=$(cat $base/idVendor)

                atidx=$(jq -r --arg vid "$vid" --arg pid "$pid" '.["\($vid)\($pid)"].atIdx' /etc/config/modem_match.json)
                if [ -z "$atidx" ]; then
                    exit 1
                fi
                atDev=$(echo "$tty_list" | awk '{print $'$atidx'}')

                msg="insert $vid $pid $ifc $atDev $driver $base" 
                
                # exec 200<>$lock_doc
                # flock -n 200
                echo "$msg" #> $ipc_doc
                # flock -u 200
                # exec 200>&-
            fi  
            ;;
        *) continue;;
    esac
done

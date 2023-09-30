#!/bin/sh


# ipc_doc=/dev/console
ipc_doc=/tmp/modem-ipc
# lock_doc=/tmp/modem-lock

if [ ! -p "$ipc_doc" ]; then
    mkfifo $ipc_doc
fi

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
                        atDev=$(echo "$tty_list" | awk '{print $'$atidx'}')

                        if [[ -n "$ifc" && -n "$driver" ]]; then
                                msg="insert $vid $pid $ifc $atDev $driver /sys$DEVPATH"
                                # exec 200<>$lock_doc
                                # flock -n 200
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
        

#!/bin/sh

ipc_doc=/tmp/modem-ipc
lock_doc=/tmp/modem-lock
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


update_network_firewall() 
{
    local num=0

    uci show network | grep 'modem' | cut -d'.' -f2 | sort -u | grep -v '=' | while read -r entry; do uci delete network."$entry"; done

    jq -r '.modem[] | select(.InterFace and .Driver) | .InterFace + " " + .Driver' "$modemjson" | while read -r entry; do

        interface=$(echo "$entry" | awk '{print $1}')
        driver=$(echo "$entry" | awk '{print $2}')
        num=$((num + 1))

        case "$driver" in
            qmi_wwan*)
                interface=${interface}_1
            ;;
        esac

        uci_command="uci set network.modem_${num}=interface"
        uci_command+="\nuci set network.modem_${num}.proto='dhcp'"
        uci_command+="\nuci set network.modem_${num}.ifname='${interface}'"
        uci_command+="\nuci commit network"

        echo -e "$uci_command" | sh

    done

    fw_list=$(uci get firewall.@zone[1].network)
    fw_list=$(echo $fw_list | sed 's/modem_[0-9]*//g' | tr -s ' ')
    fw_list+=$(uci show network | grep 'modem' | cut -d'.' -f2 | sort -u | grep -v '=' | tr  '\n' ' ')
    uci set firewall.@zone[1].network="$fw_list"
    uci commit firewall

    /etc/init.d/network reload
    /etc/init.d/firewall reload
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
                case "$driver" in
                        *cdc*)
                                atidx=$((atidx + 1))
                        ;;
                        # *) 
                        #         continue
                        # ;;
                esac

                atDev=$(echo "$tty_list" | awk '{print $'$atidx'}')
                
                msg="insert $vid $pid $ifc $atDev $driver $base" 
                update_dial_conf $base
                update_modem_leds $ifc
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

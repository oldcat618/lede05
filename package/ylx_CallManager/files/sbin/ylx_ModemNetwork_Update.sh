#!/bin/bash

modemjson="/tmp/modem.json"
num=0

if [ ! -f "$modemjson" ]; then
  echo "JSON file not found: $modemjson"
  exit 1
fi

uci show network | grep 'modem' | cut -d'.' -f2 | sort -u | grep -v '=' | while read -r entry; do uci delete network."$entry"; done

jq -r '.modem[] | select(.InterFace and .Driver) | .InterFace + " " + .Driver' "$modemjson" | while read -r entry; do

    interface=$(echo "$entry" | awk '{print $1}')
    driver=$(echo "$entry" | awk '{print $2}')
    num=$((num + 1))

    case "$driver" in
        qmi_wwan*)
            if ip link show ${interface}_1 &> /dev/null; then
                interface=${interface}_1
            fi
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
fw_list+=" $(uci show network | grep 'modem' | cut -d'.' -f2 | sort -u | grep -v '=' | tr '\n' ' ')"

uci set firewall.@zone[1].network="$fw_list"
uci commit firewall

/etc/init.d/firewall reload
# /etc/init.d/network reload

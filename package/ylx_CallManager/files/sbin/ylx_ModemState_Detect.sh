#!/bin/bash

ATtool="ylx_AT-Tool"
json_file="/tmp/modem.json"
updated_file="/tmp/updated_config.json"
tmp_file="/tmp/tmp_updated_config.json"


dev_set_atecho()
{   
    local CMD_SET_ATECHO="ATE"
    local dev=$1

    local reply=$($ATtool -p $dev -c $CMD_SET_ATECHO)
    if ! echo "$reply" | grep -q "OK"; then
        echo "Failed to set AT Commond ECHO on device $dev"
        exit 1
    fi
}

dev_get_version()
{
    local CMD_GET_VERSION="ATI"
    local dev=$1

    local reply=$($ATtool -p $dev -c $CMD_GET_VERSION)
    if echo "$reply" | grep -q "OK"; then
        ver=$(echo "$reply" | grep "Revision:" | awk '{print $2}')
        echo $ver
    fi
}

dev_get_imei()
{
    local CMD_GET_IMIE="AT+CGSN"
    local dev=$1

    local reply=$($ATtool -p $dev -c $CMD_GET_IMIE)
    if echo "$reply" | grep -q "OK"; then
        imei=$(echo "$reply" | grep -oE "[0-9]+" | awk 'NR==1{print}')
        echo $imei
    fi
}

dev_get_netinfo()
{
    
    local CMD_GET_NETINFO=("AT+QNWINFO" "AT+SGCELLINFOEX")
    local netType=''
    local dev=$1
    local band=''

    for cmd in "${CMD_GET_NETINFO[@]}"; do
        local reply=$($ATtool -p $dev -c $cmd)
        if echo "$reply" | grep -q "OK"; then
            case $cmd in 
                AT+QNWINFO)
                    netType=$(echo "$reply" | awk -F'[:,]' '/\+QNWINFO/{print $2}'|sed 's/"//g')
                    band=$(echo "$reply" | awk -F'[:,]' '/\+QNWINFO/{print $4}'|sed 's/"//g')
                ;;
                AT+SGCELLINFOEX)
                    netType=$(echo "$reply" | awk -F'[:,]' '/\+SGCELLINFOEX/{print $3}')
                    band=$(echo "$reply" | awk -F'[:,]' '/\+SGCELLINFOEX/{print $5}')
                ;;
            esac
            break
        else
            continue
        fi
    done
    
    echo $netType/$band
}

dev_get_rssi()
{
    local CMD_GET_RSSI="AT+CSQ"
    local dev=$1

    local reply=$($ATtool -p $dev -c $CMD_GET_RSSI)
    if echo "$reply" | grep -q "OK"; then
        rssi=$(echo "$reply" | awk -F'CSQ:' '{print $2}' | tr -d '\r\n')
        echo $rssi
    fi
}

dev_get_operatorname()
{
    
    local CMD_GET_OPERATORNAME=("AT^EONS=1" "AT+QSPN")
    local operatorname=''
    local dev=$1

    for cmd in "${CMD_GET_OPERATORNAME[@]}"; do
        local reply=$($ATtool -p $dev -c $cmd)
        if echo "$reply" | grep -q "OK"; then
            case $cmd in 
                AT^EONS*)
                    operatorname=$(echo "$reply" | grep "ONS:" | awk -F'["]' '{print $2}')
                ;;
                AT+QSPN)
                    operatorname=$(echo "$reply" | grep "+QSPN" | awk -F'["]' '{print $2}')
                ;;
            esac
            break
        else
            continue
        fi
        
    done

    echo $operatorname
}

dev_get_imsi()
{
    local CMD_GET_IMSI="AT+CIMI"
    local dev=$1

    local reply=$($ATtool -p $dev -c $CMD_GET_IMSI)
    if echo "$reply" | grep -q "OK"; then
        imsi=$(echo "$reply" | grep -oE "[0-9]+" | awk 'NR==1{print}')
        echo $imsi
    fi
}

dev_get_ccid()
{
    local CMD_GET_CCID=("AT+ICCID" "AT+CCID")
    local dev=$1

    for cmd in "${CMD_GET_CCID[@]}"; do
        local reply=$($ATtool -p $dev -c $cmd)
        if echo "$reply" | grep -q "OK"; then
            case $cmd in 
                AT+ICCID)
                    ccid=$(echo "$reply" | grep "ICCID:" | awk '{print $2}')
                ;;
                AT+CCID)
                    ccid=$(echo "$reply" | grep "CCID:" | awk '{print $2}')
                ;;
            esac
            break
        else
            continue
        fi
        
    done

    echo $ccid
}

dev_get_cpin()
{
    local CMD_GET_CPIN="AT+CPIN?"
    local dev=$1

    case $($ATtool -p $dev -c $CMD_GET_CPIN) in
        *"READY"*)
            echo "SIM READY"
            ;;
        *"SIM PIN"*)
            echo "SIM PIN required"
            ;;
        *"SIM PUK"*)
            echo "SIM PUK required"
            ;;
        *"SIM not inserted"*)
            echo "SIM No Insert"
            ;;
        *)
            echo "SIM state unknown"
            ;;
    esac
}

cp $json_file $updated_file
atDev_list=$(jq -r '.modem[].atDev' $json_file)
for dev in $atDev_list; do
    # if [ ! -c "/dev/$dev" ]; then
    #     # continue 2
    # fi

    fix=/dev/$dev
    dev_set_atecho $fix
    rev=$(dev_get_version $fix)
    imei=$(dev_get_imei $fix)
    netinfo=$(dev_get_netinfo $fix)
    rssi=$(dev_get_rssi $fix)
    operatorname=$(dev_get_operatorname $fix)
    imsi=$(dev_get_imsi $fix)
    ccid=$(dev_get_ccid $fix)
    cpin=$(dev_get_cpin $fix)


    # if [ -n "$rev" ] && [ -n "$imei" ] && [ -n "$netinfo" ] && [ -n "$rssi" ] && [ -n "$operatorname" ] && [ -n "$imsi" ] && [ -n "$ccid" ] && [ -n "$cpin" ]; then

    # fi
    
    jq --arg dev "$dev" \
    --arg rev "$rev" \
    --arg imei "$imei" \
    --arg cpin "$cpin" \
    --arg rssi "$rssi" \
    --arg operator "$operatorname" \
    --arg netType "$(echo "$netinfo" | cut -d'/' -f1)" \
    --arg band "$(echo "$netinfo" | cut -d'/' -f2)" \
    --arg imsi "$imsi" \
    --arg ccid "$ccid" \
    '.modem[] |= if .atDev == $dev then . + {"state": {"Revision": $rev, "IMEI": $imei, "SIM": $cpin, "RSSI": $rssi, "Operator": $operator, "NetType": $netType, "Band": $band, "IMSI": $imsi, "CCID": $ccid}} else . end' "$updated_file" > "$tmp_file"

    mv $tmp_file $updated_file
done
mv $updated_file $json_file

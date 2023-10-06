#!/bin/bash

ATtool="ylx_AT-Tool"
json_file="/tmp/modem.json"
updated_file="/tmp/updated_config.json"
tmp_file="/tmp/tmp_updated_config.json"


dev_set_atecho()
{   
    local CMD_SET_ATECHO="ATE"

    dev=$1
    reply=$($ATtool -p $dev -c $CMD_SET_ATECHO)
    if ! echo "$reply" | grep -q "OK"; then
        echo "Failed to set AT Commond ECHO on device $dev"
        exit 1
    fi
}

dev_get_version()
{
    local CMD_GET_VERSION="ATI"
    dev=$1

    reply=$($ATtool -p $dev -c $CMD_GET_VERSION)
    if echo "$reply" | grep -q "OK"; then
        ver=$(echo "$reply" | grep "Revision:" | awk '{print $2}')
        echo $ver
    fi
}

dev_get_imei()
{
    local CMD_GET_IMIE="AT+CGSN"
    dev=$1

    reply=$($ATtool -p $dev -c $CMD_GET_IMIE)
    if echo "$reply" | grep -q "OK"; then
        imei=$(echo "$reply" | grep -oE "[0-9]+" | awk 'NR==1{print}')
        echo $imei
    fi
}

dev_get_netinfo()
{
    
    local CMD_GET_NETINFO=("AT+QNWINFO" "AT+SGCELLINFOEX")
    local netType=''
    dev=$1

    for cmd in "${CMD_GET_NETINFO[@]}"; do
        reply=$($ATtool -p $dev -c $cmd)
        if echo "$reply" | grep -q "OK"; then
            if echo "$reply" | grep -q "+SGCELLINFOEX"; then
                netType=$(echo "$reply" | awk -F'[:,]' '/\+SGCELLINFOEX/{print $3}')
                echo $netType
                break
            fi
        else
            continue
        fi
    done

}

dev_get_rssi()
{
    local CMD_GET_RSSI="AT+CSQ"
    dev=$1

    reply=$($ATtool -p $dev -c $CMD_GET_RSSI)
    if echo "$reply" | grep -q "OK"; then
        rssi=$(echo "$reply" | grep "+CSQ:" | awk -F'[:,]' '{print $2 "," $3}')
        echo $rssi
    fi
}

dev_get_operatorname()
{
    
    local CMD_GET_OPERATORNAME=("AT^EONS=1" "AT+QSPN")
    local operatorname=''
    dev=$1

    for cmd in "${CMD_GET_OPERATORNAME[@]}"; do
        reply=$($ATtool -p $dev -c $cmd)
        if echo "$reply" | grep -q "OK"; then
            if echo "$reply" | grep -q "ONS:"; then
                operatorname=$(echo "$reply" | grep "ONS:" | awk -F'["]' '{print $2}')
                echo $operatorname
                break 
            fi
        else
            continue
        fi
        
    done

}

dev_get_imsi()
{
    local CMD_GET_IMSI="AT+CIMI"
    dev=$1

    reply=$($ATtool -p $dev -c $CMD_GET_IMSI)
    if echo "$reply" | grep -q "OK"; then
        imsi=$(echo "$reply" | grep -oE "[0-9]+" | awk 'NR==1{print}')
        echo $imsi
    fi
}

dev_get_ccid()
{
    local CMD_GET_CCID="AT+ICCID"
    dev=$1

    reply=$($ATtool -p $dev -c $CMD_GET_CCID)
    if echo "$reply" | grep -q "OK"; then
        ccid=$(echo "$reply" | grep "ICCID:" | awk '{print $2}')
        echo $ccid
    fi
}

dev_get_cpin()
{
    local CMD_GET_CPIN="AT+CPIN?"
    dev=$1

    case $($ATtool -p $dev -c $CMD_GET_CPIN) in
        *"READY"*)
            echo "SIM Insert"
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
    dev=/dev/$dev
    # if [ ! -c "/dev/$dev" ]; then
    #     # continue 2
    # fi
    dev_get_version $dev
    dev_set_atecho $dev
    dev_get_imei $dev
    dev_get_netinfo $dev
    dev_get_rssi $dev
    dev_get_operatorname $dev
    dev_get_imsi $dev
    dev_get_ccid $dev
    dev_get_cpin $dev

    # jq '.modem[] |= if .atDev == "'$dev'" then . + {"state": {"IPV4": 3, "IPV6": 7}} else . end' $updated_file > $tmp_file
    # mv $tmp_file $updated_file
done
# mv $updated_file $json_file

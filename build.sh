#!/bin/bash

source "ylx_files/environment.ini"

buildtime=`date +%Y-%m-%d`
reset_color="\e[0m"
red="\e[31m"
green="\e[32m"
yellow="\e[33m"
blue="\e[34m" 
log_e(){
    echo -e "${red}$1${reset_color}"
}

log_w(){
    echo -e "${yellow}$1${reset_color}"
}

log(){
    echo -e "${green}$1${reset_color}"
}

patch_copy_to_kernel_directory(){

    echo "Copy the patch file from $1 to $2."

    if [ ! -d "$1" ] || [ -z "$(find "$1" -type f)" ]; then
        log_e "The source file does not exist or is empty."
        return
    fi

    for org in "$1"/*; do
        fn=$(basename "$org")
        if [ -e "$2/$fn" ]; then
            if ! cmp -s "$org" "$2/$fn"; then
                log "Copy $fn to the kernel patch folder."
                cp -f "$org" "$2/$fn"
            fi
        else
            log "Copy $fn to the kernel patch folder."
            cp -f "$org" "$2/$fn"
        fi
    done
}

update_custom_package(){
    package_name="$1"
    package_repo_url="$2"

    custom_feed="src-git $package_name $package_repo_url"
    if ! grep -qF "$custom_feed" "./feeds.conf.default"; then
        log "Add repo url: $custom_feed >> ./feeds.conf.default, Update feed/$package_name"
        echo "$custom_feed" >> "./feeds.conf.default"
        ./scripts/feeds update $package_name
        ./scripts/feeds install $package_name
    fi
}


env_init() {
    # Copy patch file
    # patch_copy_to_kernel_directory $SOURCE_PATCHES_PATH $TARGET_PATCHES_PATH
    # # cp ylx_files/x86/x86.kernel.config  target/linux/x86/config-5.15

    # # Update custom application
    # update_custom_package $CUSTOM_PCSKGES_NAME $YLX_PCSKGES_REPO_URL


    #Lan IPaddr
    sed -i "s/192\.168\.[0-9]*\.[0-9]*/$CUSTOM_LAN_IPADDRESS/g" ./package/base-files/files/bin/config_generate
    #HostName
    sed -i "s/hostname='.*'/hostname='$CUSTOM_HOSTNAME'/g" ./package/base-files/files/bin/config_generate
    #TimeZone
    sed -i "s/timezone='.*'/timezone='CST-8'/g" ./package/base-files/files/bin/config_generate
    sed -i "s/zonename=.*/zonename='Asia\/Shanghai'/" ./package/base-files/files/bin/config_generate

    #WiFi Settings
    if ! grep -q 'macaddr="$(cat /sys/class/ieee80211/${dev}/macaddress)"' ./package/kernel/mac80211/files/lib/wifi/mac80211.sh; then
        sed -i '/path="$(iwinfo nl80211 path "$dev")"/a \ \ \ \ \ \ \ \ macaddr="$(cat /sys/class/ieee80211/${dev}/macaddress)"' ./package/kernel/mac80211/files/lib/wifi/mac80211.sh
    fi
    sed -i 's/set wireless.default_radio${devidx}.ssid=.*/set wireless.default_radio${devidx}.ssid="YLX-$(echo $mode_band| tr "a-z" "A-Z" )-"$(echo $macaddr | tr -d ':' | tr "a-z" "A-Z" | cut -c 9-)/' ./package/kernel/mac80211/files/lib/wifi/mac80211.sh
    sed -i 's/set wireless\.\${devidx}\.disabled=1/set wireless\.\${devidx}\.disabled=0/g' ./package/kernel/mac80211/files/lib/wifi/mac80211.sh
    #ETH
    sed -i '/brounion-r86s|gowin-solution-r86s)/,/;;/d' ./target/linux/x86/base-files/etc/board.d/02_network
    sed -i '/esac/i \
brounion-r86s|gowin-solution-r86s)\
    ucidef_set_interfaces_lan_wan "eth0 eth1" "eth2"\
    ;;' ./target/linux/x86/base-files/etc/board.d/02_network

    #M21AX|M21AXS WiFi Drived
    if  ! grep -q "\$(PKG_BUILD_DIR)\/firmware\/mt7915_eeprom_dbdc\.bin \\\\" ./package/kernel/mt76/Makefile; then
    sed -i '/define KernelPackage\/mt7915-firmware\/install/ {
:a
N
$!ba
s/\(mt7915_wa.bin \\\n\)/&\t\t$(PKG_BUILD_DIR)\/firmware\/mt7915_eeprom_dbdc.bin \\\n/
}' ./package/kernel/mt76/Makefile
    fi
 
    #ETH
    sed -i '/iyunlink,m21axs)/,/;;/d' ./target/linux/ramips/mt7621/base-files/etc/board.d/02_network
    sed -i '/*)/i \
    iyunlink,m21axs)\
       # ucidef_set_interfaces_lan_wan "lan1 lan2 lan3" "wan-mod wan" \
       ucidef_add_switch "switch0" \\\
			"0:lan" "1:lan" "2:lan" "3:wan" "4:wan" "6u@eth0" "5u@eth1"\
        ;;' ./target/linux/ramips/mt7621/base-files/etc/board.d/02_network
    #LED
    sed -i '/iyunlink,m21axs)/,/;;/d' ./target/linux/ramips/mt7621/base-files/etc/board.d/01_leds
    sed -i '/esac/ i\
iyunlink,m21axs)\
    ucidef_set_led_netdev "wan" "Wan act" "led_net" "eth1" "tx rx" \
    ucidef_set_led_netdev "modem" "Modem act" "led_mod" "usb0" "tx rx" \
    ;;' target/linux/ramips/mt7621/base-files/etc/board.d/01_leds


}

repo(){
    # git clone 
    log_w "no"
}

build_option_target() {
    if [ $# -eq 0 ]; then
        log_e "Usage: $0 <option>"
        exit 1
    fi

    selected_option=$1

    if [[ -n ${option_to_build_target["$selected_option"]} ]]; then

        selected_info="${option_to_build_target["$selected_option"]}"
        selected_function="${selected_info%%:*}" 
        selected_comment="${selected_info#*:}" 
        if [[ -n $selected_function && -n $(type -t "$selected_function") && $(type -t "$selected_function") = "function" ]]; then
            log "Selected option: $selected_option ($selected_comment)"
            $selected_function $selected_option
        else
            log_e "No corresponding function found for $selected_option."
        fi
    else
        log_e "Invalid option: $selected_option"
        log_e "Please enter valid options:"
        for opt in "${!option_to_build_target[@]}"; do
            info="${option_to_build_target["$opt"]}"
            comment="${info#*:}"
            log_w "$opt: $comment"
        done
    fi
}


rebuild_base_files() {
    rm -rf ./files
    cp ylx_files/$1/$1.opwrt.config .config

    sed -i 's/CONFIG_VERSION_CODE="[^"]*"/CONFIG_VERSION_CODE="'$buildtime'"/' .config
    sed -i 's/CONFIG_VERSION_NUMBER="[^"]*"/CONFIG_VERSION_NUMBER="F16-X86"/' .config

    cp ylx_files/$1/files/ . -rf
    make package/base-files/clean V=s

}


build_target_x86() {
    mkdir -p firm/x86
    rebuild_base_files $1
    make V=s $2
    cp bin/targets/x86/64/iyunlink-x86-64-generic-squashfs-combined-efi.img.gz firm/x86/X86-$buildtime.img.gz
    ##create U-disk img
    # gunzip -k bin/targets/x86/64/iyunlink-x86-64-generic-squashfs-combined-efi.img.gz -c > firm/$1-efi-$buildtime.img 

}

build_target_m21axs() {
    mkdir -p firm/mt7621/m21axs
    
    rm -rf ./files
    cp ylx_files/mt7621/m21axs/m21axs_op.config .config

    sed -i 's/CONFIG_VERSION_CODE="[^"]*"/CONFIG_VERSION_CODE="'$buildtime'"/' .config
    sed -i 's/CONFIG_VERSION_NUMBER="[^"]*"/CONFIG_VERSION_NUMBER="M21AXS"/' .config

    cp ylx_files/mt7621/m21axs/files . -rf
    make package/base-files/clean V=s

    make V=s $2
    cp bin/targets/ramips/mt7621/iyunlink-ramips-mt7621-iyunlink_m21axs-squashfs-sysupgrade.bin firm/mt7621/m21axs/M21AXS-$buildtime.bin

}

build_target_m21l2s() {
    mkdir -p firm/mt7621/m21l2s
    
    rm -rf ./files
    cp ylx_files/mt7621/m21l2s/m21l2s_op.config .config

    sed -i 's/CONFIG_VERSION_CODE="[^"]*"/CONFIG_VERSION_CODE="'$buildtime'"/' .config
    sed -i 's/CONFIG_VERSION_NUMBER="[^"]*"/CONFIG_VERSION_NUMBER="M21L2S"/' .config

    cp ylx_files/mt7621/m21l2s/files . -rf
    make package/base-files/clean V=s

    make V=s $2
    cp bin/targets/ramips/mt7621/iyunlink-ramips-mt7621-iyunlink_m21l2s-squashfs-sysupgrade.bin firm/mt7621/m21l2s/M21L2S-$buildtime.bin

}

build_target_m28s() {
    mkdir -p firm/mt7628/m28s
    
    rm -rf ./files
    cp ylx_files/mt7628/m28s/m28s_op.config .config

    sed -i 's/CONFIG_VERSION_CODE="[^"]*"/CONFIG_VERSION_CODE="'$buildtime'"/' .config
    sed -i 's/CONFIG_VERSION_NUMBER="[^"]*"/CONFIG_VERSION_NUMBER="M28S"/' .config

    cp ylx_files/mt7628/m28s/files . -rf
    make package/base-files/clean V=s

    make V=s $2
    cp bin/targets/ramips/mt76x8/iyunlink-ramips-mt76x8-iyunlink_m28s-squashfs-sysupgrade.bin firm/mt7628/m28s/M28S-$buildtime.bin

}

build_target_q31() {
    mkdir -p firm/qca9531/q31
    
    rm -rf ./files
    cp ylx_files/qca9531/q31/q31_op.config .config

    sed -i 's/CONFIG_VERSION_CODE="[^"]*"/CONFIG_VERSION_CODE="'$buildtime'"/' .config
    sed -i 's/CONFIG_VERSION_NUMBER="[^"]*"/CONFIG_VERSION_NUMBER="Q31"/' .config

    cp ylx_files/qca9531/q31/files . -rf
    make package/base-files/clean V=s

    make V=s $2
    cp bin/targets/ath79/generic/iyunlink-ath79-generic-iyunlink_q31-squashfs-sysupgrade.bin firm/qca9531/q31/Q31-$buildtime.bin

}

declare -A option_to_build_target
# [option]="function:information"
option_to_build_target=(
    ["repo"]="repo:Obtain the device adaptation file."
    ["env"]="env_init:Initialize the compilation environment."
    ["x86"]="build_target_x86:Building target architecture for x86."
    ["m21axs"]="build_target_m21axs:Building target m21axs."
    ["m21l2s"]="build_target_m21l2s:Building target m21l2s."
    ["m28s"]="build_target_m28s:Building target m28s."
    ["q31"]="build_target_q31:Building target q31."

)

target=$1
thread=$2
echo "lsat building $LAST_BUILD_TARGET"
sed -i "s/LAST_BUILD_TARGET=.*/LAST_BUILD_TARGET='$target'/g" ylx_files/environment.ini
build_option_target $target $thread
log "$target Compile Done."
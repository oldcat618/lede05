#!/bin/bash

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
    # source "ylx_files/x86/environment.ini"
    # mkdir -p firm
    # # Copy patch file
    # patch_copy_to_kernel_directory $SOURCE_PATCHES_PATH $TARGET_PATCHES_PATH
    # cp ylx_files/x86/x86.kernel.config  target/linux/x86/config-5.15

    # # Update custom application
    # update_custom_package $CUSTOM_PCSKGES_NAME $YLX_PCSKGES_REPO_URL


    #Lan IPaddr
    sed -i "s/192\.168\.[0-9]*\.[0-9]*/$CUSTOM_LAN_IPADDRESS/g" ./package/base-files/files/bin/config_generate
    #HostName
    sed -i "s/hostname='.*'/hostname='$CUSTOM_HOSTNAME'/g" ./package/base-files/files/bin/config_generate
    #TimeZone
    sed -i "s/timezone='.*'/timezone='CST-8'/g" ./package/base-files/files/bin/config_generate
    sed -i "/timezone='.*'/a\\\t\t\set system.@system[-1].zonename='Asia/Shanghai'" ./package/base-files/files/bin/config_generate

    #WiFi Settings
    sed -i 's/set wireless.default_${name}.ssid=.*/set wireless.default_${name}.ssid="IYUNLINK-$(echo $mode_band| tr "a-z" "A-Z" )-"$(echo $macaddr | tr -d ':' | tr "a-z" "A-Z" | cut -c 7-)/' ./package/kernel/mac80211/files/lib/wifi/mac80211.sh
    sed -i 's/set wireless\.\${name}\.disabled=1/set wireless\.\${name}\.disabled=0/g' ./package/kernel/mac80211/files/lib/wifi/mac80211.sh
    #ETH
    sed -i '/brounion-r86s)/,/;;/d' ./target/linux/x86/base-files/etc/board.d/02_network
    sed -i '/esac/i \
brounion-r86s|gowin-solution-r86s)\
    ucidef_set_interfaces_lan_wan "eth0 eth1" "eth2"\
    ;;' ./target/linux/x86/base-files/etc/board.d/02_network

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
    sed -i 's/CONFIG_VERSION_NUMBER="[^"]*"/CONFIG_VERSION_NUMBER="'$1'"/' .config

    cp ylx_files/$1/files/ . -rf
    make package/base-files/clean V=s

}


build_target_x86() {
    rebuild_base_files $1
    make V=s $2
    cp bin/targets/x86/64/iyunlink-x86-64-generic-squashfs-combined-efi.img.gz firm/$1-efi-$buildtime.img.gz
    ##create U-disk img
    # gunzip -k bin/targets/x86/64/iyunlink-x86-64-generic-squashfs-combined-efi.img.gz -c > firm/$1-efi-$buildtime.img 

}


declare -A option_to_build_target
# [option]="function:information"
option_to_build_target=(
    ["repo"]="repo:Obtain the device adaptation file."
    ["env"]="env_init:Initialize the compilation environment."
    ["x86"]="build_target_x86:Building target architecture for x86."

)

target=$1
thread=$2
build_option_target $target $thread
log "$target Compile Done."
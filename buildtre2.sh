#!/bin/bash

#
# Copyright (C) 2018 Yuvraj Saxena (imyuvraj)
#

# Script to compile kernel

# Colors
ORNG=$'\033[0;33m'
CYN=$'\033[0;36m'
PURP=$'\033[0;35m'
BLINK_RED=$'\033[05;31m'
BLUE=$'\033[01;34m'
BLD=$'\033[1m'
GRN=$'\033[01;32m'
RED=$'\033[01;31m'
RST=$'\033[0m'
YLW=$'\033[01;33m'

function START_SCRIPT() {
if [[ ${BUILDING_ON_CI} = 'y' ]]; then
        args="${*}"
        while [[ ${#} -ge 1 ]]; do
	case "${1}" in
                "-b"|"--build")
                        BUILD_KERNEL=y
clear
                        ;;
		"-c"|"--clean")
			CLEAN_KERNEL=y
clear
			;;
                "-h"|"--help")
                        HELP=y
clear
                        ;;
		*)
clear
                        printf '\n'
			echo "${RED}Opps:Invalid Option${RST}"
                        exit;
			;;
		esac
		shift
        done
else
        echo "${GRN} Welcome ${RST}"
        read -rn1 -p "${PURP}Type h for help (b|c|h): ${RST}" O
	case "${O}" in
                "b"|"build")
                        BUILD_KERNEL=y
clear
                        ;;
		"c"|"clean")
			CLEAN_KERNEL=y
clear
			;;
                "h"|"help")
                        HELP=y
clear
                        ;;
		*)
clear
			printf '\n'
			echo "${RED}Opps:Invalid Option${RST}"
                        exit;
			;;
		esac
		shift
fi
}

function BANNER() {
clear
        INFO
        echo "${BLD}${BLINK_RED}

▓██   ██▓ █    ██  ██▒   █▓ ██▀███   ▄▄▄      ▄▄▄██▀▀▀
 ▒██  ██▒ ██  ▓██▒▓██░   █▒▓██ ▒ ██▒▒████▄      ▒██   
  ▒██ ██░▓██  ▒██░ ▓██  █▒░▓██ ░▄█ ▒▒██  ▀█▄    ░██   
  ░ ▐██▓░▓▓█  ░██░  ▒██ █░░▒██▀▀█▄  ░██▄▄▄▄██▓██▄██▓  
  ░ ██▒▓░▒▒█████▓    ▒▀█░  ░██▓ ▒██▒ ▓█   ▓██▒▓███▒   
   ██▒▒▒ ░▒▓▒ ▒ ▒    ░ ▐░  ░ ▒▓ ░▒▓░ ▒▒   ▓▒█░▒▓▒▒░   
 ▓██ ░▒░ ░░▒░ ░ ░    ░ ░░    ░▒ ░ ▒░  ▒   ▒▒ ░▒ ░▒░   
 ▒ ▒ ░░   ░░░ ░ ░      ░░    ░░   ░   ░   ▒   ░ ░ ░   
 ░ ░        ░           ░     ░           ░  ░░   ░   
 ░ ░                   ░                              
${RST}
"
sleep 2
clear
}

function INFO() {
        BNAME="dev"
        BNICK="melek"
        echo "${BLD}${CYN} MADE BY ${BNAME} (AKA: ${BNICK}) ${RST}"
}

function TOOLCHAIN() {
if [[ ! -d linaro ]];
then
        echo "${RED}####################################"
        echo "${CYN}#       TOOLCHAIN NOT FOUND!       #"
        echo "${YLW}####################################"
	sleep 0.7
clear
        echo "${YLW}####################################"
        echo "${GRN}#       CLONING TOOLCHAIN          #"
        echo "####################################"
        
  git clone -q https://github.com/svoboda18/android_gcc_6.4.1_arm-eabi gcc
        export ARCH=arm CROSS_COMPILE=${PWD}/gcc/bin/arm-eabi-
else
        export ARCH=arm CROSS_COMPILE=${PWD}/gcc/bin/arm-eabi-
fi
}

function BUILD() {
clear
        COMPILED_KERNEL="out/arch/arm/boot/zImage-dtb"
	export KBUILD_BUILD_USER=dev
        export KBUILD_BUILD_HOST=melek
        export KNM=melek
        mkdir -p out
        echo "${PURP} READING DEFCONFIG..."
        make k200_1g_debug_defconfig O=out/ &>> defconfig.log
clear
        echo "${YLW} BUILDING KERNEL..."
        make -j4 O=out/ &>> Kernel.log
	COMPILED_DATE=$(date +"%Y-%m-%d-%H:%M:%S")
}

function CHECK() {
clear
if [[ ! -e ${COMPILED_KERNEL} ]];
then
        echo "${RED}############################"
        echo "${RED}#        BUILD ERROR!      #"
        echo "${RED}############################"
        echo "${CYN}#       Uploading Logs     #"
        echo "${RED}############################"
clear
        TRANSFER Kernel.log
else
        echo "${GRN} #####################################"
        echo "${GRN} #                                   #"
        echo "${GRN} #  SUCCESSFULLY BUILDED KERNEL      #"
        echo "${GRN} #        -BY IMYUVRAJ               #"
        echo "${GRN} #                                   #"
        echo "${GRN} #####################################"
        echo "${ORNG} #  uploading Kernel                #"
        echo "${ORNG} #####################################"
        cp ${COMPILED_KERNEL} Kernel_${COMPILED_DATE}-${KNM}
	zip -r Kernel_${COMPILED_DATE}-${KNM}.zip Kernel_${COMPILED_DATE}-${KNM}
clear
        TRANSFER Kernel_${COMPILED_DATE}-${KNM}.zip
fi
}

function CLEAN() {
clear
        echo "${GRN}#        Cleaning Tree!      ${RST}"
        make clean >/dev/null
        make mrproper >/dev/null
clear
}

function HELP() {
	echo "${ORNG}options:${RST}"
	echo "${CYN}  b      Build Kernel${RST}"
	echo "${RST}  c      clean Kernel${RST}"
	echo "${GRN}  h      Options${RST}"
	read junk
}

function TRANSFER() {
        file="$1"
        zipname=$(echo "${file}" | awk -F '/' '{print $NF}')
        destination="$2"
        url=$(curl -# -T "${file}" https://transfer.sh/${destination})
        printf '\n'
        echo "${GRN}Download $zipname at $url${RST}"
}

function FORMAT_TIME() {
        MINS=$(((${1}-${2})/60))
        SECS=$(((${1}-${2})%60))
if [[ ${MINS} -ge 60 ]]; then
        HOURS=$((${MINS}/60))
        MINS=$((${MINS}%60))
fi

if [[ ${HOURS} -eq 1 ]]; then
        TIME_STRING+="1 HOUR, "
elif [[ ${HOURS} -ge 2 ]]; then
        TIME_STRING+="${HOURS} HOURS, "
fi

if [[ ${MINS} -eq 1 ]]; then
        TIME_STRING+="1 MINUTE"
else
        TIME_STRING+="${MINS} MINUTES"
fi

if [[ ${SECS} -eq 1 && -n ${HOURS} ]]; then
        TIME_STRING+=", AND 1 SECOND"
elif [[ ${SECS} -eq 1 && -z ${HOURS} ]]; then
        TIME_STRING+=" AND 1 SECOND"
elif [[ ${SECS} -ne 1 && -n ${HOURS} ]]; then
        TIME_STRING+=", AND ${SECS} SECONDS"
elif [[ ${SECS} -ne 1 && -z ${HOURS} ]]; then
        TIME_STRING+=" AND ${SECS} SECONDS"
fi

        echo ${TIME_STRING}
}
        
        BANNER

if [[ ${BUILDING_ON_CI} = 'y' ]]; then
        START_SCRIPT "${@}"
else
	START_SCRIPT
fi

if [[ "${BUILD_KERNEL}" = 'y' ]]; then
	TOOLCHAIN
        START=$(date +"%s")
        BUILD
	CHECK
        END=$(date +%s)
        TIME_STRING="$(FORMAT_TIME "${START}" "${END}")"
        echo "${GRN}Completed In: ${TIME_STRING}"
elif [[ "${CLEAN_KERNEL}" = 'y' ]]; then
        START=$(date +"%s")
        CLEAN
	END=$(date +%s)
	TIME_STRING="$(FORMAT_TIME "${START}" "${END}")"
	echo "${GRN}Completed In: ${TIME_STRING}"
else
if [[ "${HELP}" = 'y' ]]; then
        HELP
fi
fi

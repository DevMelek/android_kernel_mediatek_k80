#!/bin/bash
export KBUILD_BUILD_USER=melek
export KBUILD_BUILD_HOST=TRY
export CROSS_COMPILE=${PWD}/arm-linux-gnueabihf-linaro-6.3.1-mt6580/bin/arm-linux-gnueabihf
export USE_CCACHE=1
export ARCH=arm ARCH_MTK_PLATFORM=mt6580
export TARGET=out
make O=$TARGET ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE V060C6_defconfig
make O=$TARGET ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j4


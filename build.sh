#!/bin/bash
# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set -e
DIR_WRK=$(dirname $(readlink -e $0))

DIR_SDK=$DIR_WRK/output/sdk_aliyun
DIR_ENV=$DIR_WRK/output/env_aliyun

SRC_CONFIG_FILE=$DIR_WRK/config/config.ubuntu.arm
DST_CONFIG_FILE=$DIR_SDK/src/configs/config.ubuntu.arm

SRC_SAMPLE=$DIR_SDK/output/release/bin
DST_SAMPLE=$DIR_WRK/sample/binary

function sdk_build()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $DST_SAMPLE
    mkdir -p $DST_SAMPLE
    cp $SRC_CONFIG_FILE $DST_CONFIG_FILE
    echo "LDFLAGS += -liothinxio" >> $DST_CONFIG_FILE
    echo "LDFLAGS += -L$DIR_ENV/arm-linux-gnueabihf/libc/usr/lib/iothinx" >> $DST_CONFIG_FILE
    echo "RPATH_CFLAGS += -Wl,-rpath,/usr/lib/iothinx" >> $DST_CONFIG_FILE
    echo "OVERRIDE_CC = $DIR_ENV/bin/arm-linux-gnueabihf-gcc" >> $DST_CONFIG_FILE
    echo "OVERRIDE_AR = $DIR_ENV/bin/arm-linux-gnueabihf-gcc-ar" >> $DST_CONFIG_FILE
    pushd $DIR_SDK > /dev/null
    make reconfig <<< 1
    make
    popd > /dev/null
    cp $SRC_SAMPLE/moxa_sample_mqtt $DST_SAMPLE
    echo "-- Process: $FUNCNAME DONE!!"
}

sdk_build


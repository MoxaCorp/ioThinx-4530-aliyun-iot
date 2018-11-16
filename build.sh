#!/bin/bash
#
# Copyright © 2018 Moxa Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


DIR_WRK=$(dirname $(readlink -e $0))

CONFIG=config.ubuntu.arm

DIR_SDK=$DIR_WRK/output/sdk_aliyun
DIR_ENV=$DIR_WRK/output/env_aliyun

SRC_CONFIG=$DIR_WRK/config
DST_CONFIG=$DIR_SDK/src/configs

OVERRIDE_CC="OVERRIDE_CC = $DIR_ENV/bin/arm-linux-gnueabihf-gcc"
OVERRIDE_AR="OVERRIDE_AR = $DIR_ENV/bin/arm-linux-gnueabihf-gcc-ar"

function sdk_build()
{
    echo "-- Process: $FUNCNAME ..."
    cp $SRC_CONFIG/$CONFIG $DST_CONFIG/$CONFIG
    sed -i "s/OVERRIDE_CC.*/${OVERRIDE_CC//\//\\/}/g" $DST_CONFIG/$CONFIG
    sed -i "s/OVERRIDE_AR.*/${OVERRIDE_AR//\//\\/}/g" $DST_CONFIG/$CONFIG
    pushd $DIR_SDK > /dev/null
    make reconfig <<< 1
    make
    popd > /dev/null
    echo "-- Process: $FUNCNAME DONE!!"
}

sdk_build


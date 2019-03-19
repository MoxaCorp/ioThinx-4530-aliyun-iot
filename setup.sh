#!/bin/bash
# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

DIR_WRK=$(dirname $(readlink -e $0))
SCRIPT=$(basename $0)

GIT_SDK=https://github.com/aliyun/iotkit-embedded.git
VER_SDK=RELEASED_V2.03
SRC_TOOLCHAIN=/usr/local/arm-linux-gnueabihf

DST_SDK=$DIR_WRK/output/sdk_aliyun
DST_TOOLCHAIN=$DIR_WRK/output/env_aliyun

MAKE_SAMPLE=$DST_SDK/src/scripts/parse_make_settings.mk
SRC_SAMPLE=$DIR_WRK/sample/source

function sdk_update()
{
    echo "-- Process: $FUNCNAME ..."

    pushd $DST_SDK > /dev/null
    git checkout -q $VER_SDK
    git submodule update --init --recursive
    popd > /dev/null

    ln -s $SRC_SAMPLE $DST_SDK
    echo "SUBDIRS += source" >> $MAKE_SAMPLE

    echo "-- Process: $FUNCNAME DONE!!"
}

function sdk_clone()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $DST_SDK
    mkdir -p $DST_SDK
    git clone --recursive $GIT_SDK $DST_SDK
    echo "-- Process: $FUNCNAME DONE!!"
}

function sdk_env()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $DST_TOOLCHAIN
    mkdir -p $DST_TOOLCHAIN
    rsync -rvhL $SRC_TOOLCHAIN/* $DST_TOOLCHAIN
    echo "-- Process: $FUNCNAME DONE!!"
}

function sdk_arg()
{
    function usage()
    {
        echo
        echo "Usage: ./$SCRIPT [options]"
        echo
        echo "Options:"
        echo -e "\t-git\t\tGit repository of SDK."
        echo -e "\t\t\tDefault: $GIT_SDK"
        echo
        echo -e "\t-ver\t\tVersion of SDK."
        echo -e "\t\t\tDefault: $VER_SDK"
        echo
        echo -e "\t--toolchain\tGNU cross-toolchain directory."
        echo -e "\t\t\tDefault: $SRC_TOOLCHAIN"
        echo
        echo -e "\t--help\t\tDisplay this help and exit."
        echo
        echo "Examples:"
        echo -e "\tDefault\t\t./$SCRIPT"
        echo -e "\tSpecify\t\t./$SCRIPT -git $GIT_SDK -ver $VER_SDK"
        echo -e "\t\t\t./$SCRIPT --toolchain $SRC_TOOLCHAIN"
        echo
        exit
    }

    arg_id=0
    for arg in "$@"
    do
        case $arg_id in
            "0")
                case $arg in
                    "-git") arg_id=1;;
                    "-ver") arg_id=2;;
                    "--toolchain") arg_id=3;;
                    "--help" | *) usage;;
                esac
            ;;
            "1")
                arg_id=0
                GIT_SDK=$arg
            ;;
            "2")
                arg_id=0
                VER_SDK=$arg
            ;;
            "3")
                arg_id=0
                SRC_TOOLCHAIN=$arg
            ;;
        esac
    done
}

function main()
{
    sdk_arg "$@"
    sdk_env
    sdk_clone
    sdk_update
}

main "$@"


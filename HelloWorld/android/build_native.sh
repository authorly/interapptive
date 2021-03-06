#!/bin/bash

# Set PATH to include SDK and NDK executable paths

[[ -s "/etc/profile.d/android-ndk.sh" ]] && source "/etc/profile.d/android-ndk.sh"
[[ -s "/etc/profile.d/android-sdk.sh" ]] && source "/etc/profile.d/android-sdk.sh"

# set params
#NDK_ROOT_LOCAL=/cygdrive/d/programe/android/ndk/android-ndk-r6b
#COCOS2DX_ROOT_LOCAL=/Users/zhangkoumyou/SourceCode/interapptive
NDK_ROOT_LOCAL=/opt/android-ndk
COCOS2DX_ROOT_LOCAL=`pwd`/../..


# try to get global variable
if [ $NDK_ROOT"aaa" != "aaa" ]; then
    echo "use global definition of NDK_ROOT: $NDK_ROOT"
    NDK_ROOT_LOCAL=$NDK_ROOT
fi

#if [ $COCOS2DX_ROOT"aaa" != "aaa" ]; then
#    echo "use global definition of COCOS2DX_ROOT: $COCOS2DX_ROOT"
#    COCOS2DX_ROOT_LOCAL=$COCOS2DX_ROOT
#fi

#if [ $COCOS2DX_ROOT"aaa" != "aaa" ]; then
#    echo "use global definition of COCOS2DX_ROOT: $COCOS2DX_ROOT"
#    COCOS2DX_ROOT_LOCAL=$COCOS2DX_ROOT
#fi

HELLOWORLD_ROOT=$COCOS2DX_ROOT_LOCAL/HelloWorld/android

# make sure assets is exist
if [ -d $HELLOWORLD_ROOT/assets ]; then
    rm -rf $HELLOWORLD_ROOT/assets
fi
if [ -d $HELLOWORLD_ROOT/res/raw ]; then
    rm -rf $HELLOWORLD_ROOT/res/raw
fi

mkdir $HELLOWORLD_ROOT/assets
mkdir $HELLOWORLD_ROOT/res/raw

# copy resources
for file in $COCOS2DX_ROOT_LOCAL/HelloWorld/Resources/*
do
    if [ -d $file ]; then
        cp -rf $file $HELLOWORLD_ROOT/assets
    fi

    if [ -f $file ]; then
        if [ "${file##*.}" = "mp4" ]; then
            cp $file $HELLOWORLD_ROOT/res/raw
        else
            cp $file $HELLOWORLD_ROOT/assets
        fi        
    fi
done

# build
cd $NDK_ROOT_LOCAL
./ndk-build -C $HELLOWORLD_ROOT $*
cd -


if [ $# -eq 0 ];then
echo "generate apk"
android update project -p . --target android-17
ant debug
fi

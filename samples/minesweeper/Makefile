PROJECT_NAME=Minesweeper

# Android
ANDROID_PACKAGE_NAME:=net.oaknut.minesweeper
ANDROID_ABIS=x86 arm64-v8a
ANDROID_VER:=26
ANDROID_BUILDTOOLS:=26.0.2
ANDROID_KEYSTORE:=/Users/reuben/.android/debug.keystore
ANDROID_KEYSTORE_PASSWORD:=android
ANDROID_KEY_PASSWORD:=android
ANDROID_KEY_ALIAS:=androiddebugkey

# iOS
IOS_ABIS=arm64
IOS_CERT=AA35BF8660055BE507571702777D49D2609620E9
IOS_PROVISIONING_PROFILE=/Users/reuben/Library/MobileDevice/Provisioning\ Profiles/8973664a-c5d7-4972-92d9-9c9a1990499e.mobileprovision


# Include the Oaknut makefile
OAKNUT_DIR:=$(wildcard $(OAKNUT_DIR))
ifndef OAKNUT_DIR
$(error OAKNUT_DIR must point to the Oaknut root)
endif
include $(OAKNUT_DIR)/build/oaknut.make

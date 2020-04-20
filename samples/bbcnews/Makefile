PROJECT_NAME=BBCNews

# Android
ANDROID_PACKAGE_NAME:=org.oaknut.bbcnews
ANDROID_ABIS:=x86 arm64-v8a
ANDROID_VER:=28
ANDROID_BUILDTOOLS:=28.0.3
ANDROID_KEYSTORE:=/Users/reuben/.android/debug.keystore
ANDROID_KEYSTORE_PASSWORD:=android
ANDROID_KEY_PASSWORD:=android
ANDROID_KEY_ALIAS:=androiddebugkey

# Include the Oaknut makefile
OAKNUT_DIR:=$(wildcard $(OAKNUT_DIR))
ifndef OAKNUT_DIR
$(error OAKNUT_DIR must point to the Oaknut root)
endif
include $(OAKNUT_DIR)/build/oaknut.make

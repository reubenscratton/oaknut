PROJECT_NAME=Minesweeper

# Android
ANDROID_ABIS=x86 arm64-v8a
ANDROID_VER:=26
ANDROID_BUILDTOOLS:=26.0.2
ANDROID_KEYSTORE:=/Users/reuben/.android/debug.keystore
ANDROID_KEYSTORE_PASSWORD:=android
ANDROID_KEY_PASSWORD:=android
ANDROID_KEY_ALIAS:=androiddebugkey

# iOS
IOS_SDK=iPhoneOS11.1.sdk
IOS_ABIS=arm64
IOS_CERT=AA35BF8660055BE507571702777D49D2609620E9
IOS_PROVISIONING_PROFILE=/Users/reuben/Library/MobileDevice/Provisioning\ Profiles/8973664a-c5d7-4972-92d9-9c9a1990499e.mobileprovision

# macOS
MACOS_SDK=MacOSX10.13.sdk

# Include the Oaknut makefile
OAKNUT_DIR:=$(wildcard $(OAKNUT_DIR))
ifndef OAKNUT_DIR
$(error OAKNUT_DIR must point to the Oaknut root)
endif
include $(OAKNUT_DIR)/oaknut.make






$(PROJECT_NAME).xcproj/project.pbxproj:
	@mkdir -p $(dir $@)


#PBXFILEREFS:=
#PBXGROUPS:=
#define ADD_PBXGROUP
#PBXGROUPS+=$1
#$(foreach d,$(wildcard $1*),$(call ADD_PBXGROUP,$d/,$2) PBXFILEREFS+=$(filter $(subst *,%,$2),$d))
#endef

#$(call ADD_PBXGROUP,app,)

#FOO:=$(foreach s,$(SOURCES),$(shell md5 -q -s $(s)))
#$(info PBXGROUPS are $(PBXGROUPS))
#$(info PBXFILEREFS are $(PBXFILEREFS))


xcode: $(PROJECT_NAME).xcproj/project.pbxproj
	echo hello sailor | sed 's/hello/world/' > $<

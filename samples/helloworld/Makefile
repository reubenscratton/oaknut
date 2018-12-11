PROJECT_NAME=HelloWorld

# Android
ANDROID_PACKAGE_NAME:=net.oaknut.helloworld

# iOS
IOS_SDK=iPhoneOS11.2.sdk

# macOS
MACOS_SDK=MacOSX10.13.sdk

# Include the Oaknut makefile
OAKNUT_DIR:=$(wildcard $(OAKNUT_DIR))
ifndef OAKNUT_DIR
$(error OAKNUT_DIR must point to the Oaknut root)
endif
include $(OAKNUT_DIR)/build/oaknut.make

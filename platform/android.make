# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.


# Derived paths
JAVA_FILES := $(wildcard $(OAKNUT_DIR)/platform/android/java/org.oaknut.main/*.java)
RESOURCES_DIR:=$(PROJECT_ROOT)/platform/android/res
MANIFEST_FILE:=$(PROJECT_ROOT)/platform/android/AndroidManifest.xml
ANDROID_NDK_DIR:=$(ANDROID_SDK_DIR)/ndk-bundle
ANDROID_NDK_TOOLCHAIN:=$(ANDROID_NDK_DIR)/toolchains/llvm/prebuilt/darwin-x86_64
ANDROID_JAR:=$(ANDROID_SDK_DIR)/platforms/android-$(ANDROID_VER)/android.jar
ANDROID_BUILDTOOLS:=$(ANDROID_SDK_DIR)/build-tools/$(ANDROID_BUILDTOOLS)
TARGET_ABI:=$(ABI)
TOOLCHAIN_ABI:=$(ABI)
SYSROOT_ABI:=$(ABI)
ifeq ($(ABI),x86)
TARGET_ABI:=i686
endif
ifeq ($(ABI),arm64-v8a)
TARGET_ABI:=aarch64
TOOLCHAIN_ABI:=aarch64-linux-android
SYSROOT_ABI:=arm64
endif

# Output files
OBJ_DIR:=$(BUILD_DIR)/obj/$(ABI)
CPP_OBJ_FILES := $(patsubst %,$(OBJ_DIR)/%,$(SOURCES_CPP:.cpp=.o))
CPP_LIB := $(BUILD_DIR)/bin/lib/$(ABI)/liboaknutapp.so
DEX := $(BUILD_DIR)/bin/classes.dex
APK := $(BUILD_DIR)/oaknut.apk
BUILT_RES := $(BUILD_DIR)/bin/resources.ap_

CC := $(ANDROID_NDK_TOOLCHAIN)/bin/clang++
CFLAGS_COMMON := \
    --target=$(TARGET_ABI)-none-linux-android \
    --gcc-toolchain=$(ANDROID_NDK_DIR)/toolchains/$(TOOLCHAIN_ABI)-4.9/prebuilt/darwin-x86_64 \
    --sysroot=$(ANDROID_NDK_DIR)/sysroot \
    -isystem $(ANDROID_NDK_DIR)/sysroot/usr/include/$(TARGET_ABI)-linux-android \
    -D__ANDROID_API__=$(ANDROID_VER) \
    -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes \
    -Wa,--noexecstack -Wformat -Werror=format-security \
    -g -DANDROID \
    -fPIC \
    -std=c++11 \
    -O0

CFLAGS_COMPILE := \
    -isystem $(ANDROID_NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/include \
    -isystem $(ANDROID_NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/$(ABI)/include \
    -isystem $(ANDROID_NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/include/backward \
    -isystem $(OAKNUT_DIR)/src

CFLAGS_LINK := \
    -fno-limit-debug-info \
    -Wl,--exclude-libs,libgcc.a \
    -Wl,--exclude-libs,libatomic.a \
    --sysroot $(ANDROID_NDK_DIR)/platforms/android-$(ANDROID_VER)/arch-$(SYSROOT_ABI) \
    -Wl,--build-id \
    -Wl,--warn-shared-textrel \
    -Wl,--fatal-warnings \
    -Wl,--no-undefined \
    -Wl,-z,noexecstack \
    -Qunused-arguments \
    -Wl,-z,relro \
    -Wl,-z,now \
    -shared \
    -Wl,-soname,liboaknutapp.so \
    -landroid \
    -lEGL \
    -lGLESv2 \
    -ljnigraphics \
    -llog \
    -lOpenSLES \
    -latomic \
    -lm "$(ANDROID_NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/$(ABI)/libgnustl_static.a"


$(CPP_OBJ_FILES): $(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	@echo Compiling $(notdir $@)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_COMPILE) -o $@ -c $<

$(CPP_LIB) : $(CPP_OBJ_FILES)
	@echo Linking .so
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS_COMMON) -o $(CPP_LIB) $(CPP_OBJ_FILES) $(CFLAGS_LINK)

$(BUILT_RES): $(shell find $(RESOURCES_DIR) -type f) $(MANIFEST_FILE)
	@echo Compiling resources
	@mkdir -p $(BUILD_DIR)/bin
	@mkdir -p $(BUILD_DIR)/gen
	$(ANDROID_BUILDTOOLS)/aapt package -v -f -I "$(ANDROID_JAR)" -M $(MANIFEST_FILE) -S "$(RESOURCES_DIR)" -m -J "$(BUILD_DIR)/gen" -F "$@"

$(DEX): $(JAVA_FILES)
	@echo Compiling java $(JAVA_FILES)
	@mkdir -p $(BUILD_DIR)/classes
	@mkdir -p $(dir $(DEX))
	javac -classpath "$(ANDROID_JAR):$(BUILD_DIR)" $(JAVA_FILES) -d $(BUILD_DIR)/classes
	$(ANDROID_BUILDTOOLS)/dx --dex --output="$(DEX)" $(BUILD_DIR)/classes

$(APK): $(DEX) $(CPP_LIB) $(BUILT_RES)
	cd $(BUILD_DIR)/bin && \
	rm -f oaknut.ap_ && \
    cp resources.ap_ oaknut.ap_ && \
	$(ANDROID_BUILDTOOLS)/aapt add oaknut.ap_ classes.dex lib/$(ABI)/liboaknutapp.so && \
	cd $(PROJECT_ROOT) && \
	zip $(BUILD_DIR)/bin/oaknut.ap_ -r assets && \
	jarsigner -keystore $(ANDROID_KEYSTORE) -storepass "$(ANDROID_KEYSTORE_PASSWORD)" \
		-keypass "$(ANDROID_KEY_PASSWORD)" -signedjar "$(APK)" $(BUILD_DIR)/bin/oaknut.ap_ "$(ANDROID_KEY_ALIAS)"


android: $(APK)



$(BUILD_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@rm -f $(BUILD_DIR)/org/oaknut/main/*.class
	@rm -f $(CPP_OBJ_FILES)
	@rm -f $(CPP_LIB)
	@rm -f $(APK)

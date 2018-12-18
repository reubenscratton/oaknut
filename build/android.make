# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.

# If ANDROID_SDK_DIR not set explicitly, guess
ifndef ANDROID_SDK_DIR
ANDROID_SDK_DIR:=~/Library/Android/sdk
endif
ANDROID_SDK_DIR:=$(wildcard $(ANDROID_SDK_DIR))

ifndef ANDROID_SDK_DIR
$(error ANDROID_SDK_DIR must be set to point to the root of a valid Android SDK)
endif

# Derived paths
JAVA_FILES:=$(wildcard $(OAKNUT_DIR)/src/platform/android/java/org.oaknut.main/*.java)
RESOURCES_DIR:=$(PROJECT_ROOT)/platform/android/res
MANIFEST_FILE:=$(PROJECT_ROOT)/platform/android/AndroidManifest.xml
ANDROID_NDK_DIR:=$(ANDROID_SDK_DIR)/ndk-bundle
ANDROID_JAR:=$(ANDROID_SDK_DIR)/platforms/android-$(ANDROID_VER)/android.jar
ANDROID_BUILDTOOLS:=$(ANDROID_SDK_DIR)/build-tools/$(ANDROID_BUILDTOOLS)
DEX := $(BUILD_DIR)/bin/classes.dex
APK := $(BUILD_DIR)/oaknut.apk
BUILT_RES := $(BUILD_DIR)/bin/resources.ap_

# Filter out the .cpp files
SOURCES:=$(filter %.cpp,$(SOURCES))

DEPS:=
LIBS:=


CFLAGS_LINK := \
    -fno-limit-debug-info \
    -Wl,--exclude-libs,libgcc.a \
    -Wl,--exclude-libs,libatomic.a \
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
    -landroid -lEGL -lGLESv2 -ljnigraphics -llog -lOpenSLES -latomic

# Function that generates the targets for one ABI
define build_abi

$(1)_TARGET_ABI:=$(1)
$(1)_TOOLCHAIN_ABI:=$(1)
$(1)_SYSROOT_ABI:=$(1)
ifeq ($(1),x86)
$(1)_TARGET_ABI:=i686
endif
ifeq ($(1),arm64-v8a)
$(1)_TARGET_ABI:=aarch64
$(1)_TOOLCHAIN_ABI:=aarch64-linux-android
$(1)_SYSROOT_ABI:=arm64
endif


$(1)_OBJ_DIR:=$(OBJ_DIR)/$(1)
$(1)_OBJS:=$$(patsubst %,$$($(1)_OBJ_DIR)%.o,$(SOURCES))
$(1)_LIB := $(BUILD_DIR)/bin/lib/$(1)/liboaknutapp.so

LIBS+=$$($(1)_LIB)
DEPS+=$$($(1)_OBJS:.o=.dep)

$(1)_CC := $(ANDROID_NDK_DIR)/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang++
$(1)_CFLAGS_COMMON := \
    --target=$$($(1)_TARGET_ABI)-none-linux-android \
    --gcc-toolchain=$(ANDROID_NDK_DIR)/toolchains/$$($(1)_TOOLCHAIN_ABI)-4.9/prebuilt/darwin-x86_64 \
    --sysroot=$(ANDROID_NDK_DIR)/sysroot \
    -isystem $(ANDROID_NDK_DIR)/sysroot/usr/include/$$($(1)_TARGET_ABI)-linux-android \
    -DANDROID -D__ANDROID_API__=$(ANDROID_VER) -DPLATFORM_ANDROID=1 \
    -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes \
    -Wa,--noexecstack -Wformat -Werror=format-security \
    -fPIC -std=c++11

$$($(1)_OBJ_DIR)%.o : %
$$($(1)_OBJ_DIR)%.o : % $$($(1)_OBJ_DIR)%.dep
	@echo android\($(1)\): Compiling $$(notdir $$<)
	@mkdir -p $$(dir $$@)
	@$$($(1)_CC) $$($(1)_CFLAGS_COMMON) \
		  -MT $$@ -MD -MP -MF $$(@:.o=.Td) \
		  $(if $(DEBUG),-g -O0,-O3) \
		  -isystem $(ANDROID_NDK_DIR)/sources/cxx-stl/llvm-libc++/include \
		  -isystem $(ANDROID_NDK_DIR)/sources/cxx-stl/llvm-libc++/libs/$(1)/include \
		  -isystem $(OAKNUT_DIR)/src \
		  -o $$@ -c $$<
	@mv -f $$(@:.o=.Td) $$(@:.o=.dep) && touch $$@

$$($(1)_LIB) : $$($(1)_OBJS)
	@echo android\($(1)\): Linking .so
	@mkdir -p $$(dir $$@)
	@$$($(1)_CC) $$($(1)_CFLAGS_COMMON) -o $$($(1)_LIB) $$($(1)_OBJS) $(CFLAGS_LINK) \
	    --sysroot $(ANDROID_NDK_DIR)/platforms/android-$(ANDROID_VER)/arch-$$($(1)_SYSROOT_ABI) \
			-lm "$(ANDROID_NDK_DIR)/sources/cxx-stl/llvm-libc++/libs/$(1)/libc++_static.a" \
			-lm "$(ANDROID_NDK_DIR)/sources/cxx-stl/llvm-libc++/libs/$(1)/libc++abi.a"



endef

# Generate targets for all the ABIs
$(foreach abi,$(ANDROID_ABIS),$(eval $(call build_abi,$(abi))))


$(BUILT_RES): $(shell find $(RESOURCES_DIR) -type f) $(MANIFEST_FILE)
	@echo Compiling resources
	@mkdir -p $(BUILD_DIR)/bin
	@mkdir -p $(BUILD_DIR)/gen
	$(ANDROID_BUILDTOOLS)/aapt package -v -f -I "$(ANDROID_JAR)" -M $(MANIFEST_FILE) -S "$(RESOURCES_DIR)" -m -J "$(BUILD_DIR)/gen" -F "$@"

$(DEX): $(JAVA_FILES)
	@echo Compiling java
	@mkdir -p $(BUILD_DIR)/classes
	@mkdir -p $(dir $(DEX))
	@javac -classpath "$(ANDROID_JAR):$(BUILD_DIR)" $(JAVA_FILES) -d $(BUILD_DIR)/classes
	@$(ANDROID_BUILDTOOLS)/dx --dex --output="$(DEX)" $(BUILD_DIR)/classes

$(APK): $(DEX) $(LIBS) $(BUILT_RES)
	@cd $(BUILD_DIR)/bin && \
	rm -f oaknut.ap_ && \
    cp resources.ap_ oaknut.ap_ && \
	$(ANDROID_BUILDTOOLS)/aapt add oaknut.ap_ classes.dex $(patsubst %,lib/%/liboaknutapp.so,$(ANDROID_ABIS)) && \
	cd $(PROJECT_ROOT) && \
	zip $(BUILD_DIR)/bin/oaknut.ap_ -r assets && \
	jarsigner -keystore $(ANDROID_KEYSTORE) -storepass "$(ANDROID_KEYSTORE_PASSWORD)" \
		-keypass "$(ANDROID_KEY_PASSWORD)" -signedjar "$(APK)" $(BUILD_DIR)/bin/oaknut.ap_ "$(ANDROID_KEY_ALIAS)"


android: $(APK)

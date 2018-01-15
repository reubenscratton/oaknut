# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.

CFLAGS+= -DPLATFORM_IOS=1


include $(OAKNUT_DIR)/src/platform/apple/oaknut.make

CFLAGS_COMMON := \
    -stdlib=libc++ \
	-isysroot $(XCODE_APP)/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK) \
	$(FLAGS) \
	$(WARNINGS) \
	-miphoneos-version-min=8.0

CFLAGS_COMPILE := \
    -isystem $(OAKNUT_DIR)/src \
    -g \
	-c \
    -O0

CFLAGS_LINK := \
    -stdlib=libc++ \
    -fobjc-link-runtime \
    -dead_strip \
    -Xlinker -object_path_lto \
    -Xlinker $(BUILD_DIR)/app_lto.o


CFLAGS_COMPILE_CPP:=-x c++ -std=c++11 $(WARNINGS_CPP)
CFLAGS_COMPILE_C:=-x c -std=gnu99
CFLAGS_COMPILE_M:=-x objective-c -std=gnu99 -fobjc-arc $(WARNINGS_OBJC) -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0
CFLAGS_COMPILE_MM:=-x objective-c++ -std=c++11 -fobjc-arc $(WARNINGS_CPP) $(WARNINGS_OBJC) -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0


define ABI_template

$(1)_OBJ_DIR:=$(BUILD_DIR)/obj/$(1)
$(1)_OBJS_CPP := $(patsubst %,$$($(1)_OBJ_DIR)/%.o,$(SOURCES_CPP))
$(1)_OBJS_C := $(patsubst %,$$($(1)_OBJ_DIR)/%.o,$(SOURCES_C))
$(1)_OBJS_M := $(patsubst %,$$($(1)_OBJ_DIR)/%.o,$(SOURCES_M))
$(1)_OBJS_MM := $(patsubst %,$$($(1)_OBJ_DIR)/%.o,$(SOURCES_MM))
$(1)_OBJS := $$($(1)_OBJS_CPP) $$($(1)_OBJS_C) $$($(1)_OBJS_M) $$($(1)_OBJS_MM)
$(1)_APP:=$$($(1)_OBJ_DIR)/ios_app

$$($(1)_OBJ_DIR):
	@mkdir -p $$@

$$($(1)_OBJS_CPP): $$($(1)_OBJ_DIR)/%.o : % | $$($(1)_OBJ_DIR)
	@echo Compiling $$(notdir $$@)
	@mkdir -p $$(dir $$@)
	$(CLANG) -arch $(1) $(CFLAGS_COMMON) $(CFLAGS_COMPILE_CPP) $(CFLAGS_COMPILE) -o $$@ -c $$<

$$($(1)_OBJS_C): $$($(1)_OBJ_DIR)/%.o : % | $$($(1)_OBJ_DIR)
	@echo Compiling $$(notdir $$@)
	@mkdir -p $$(dir $$@)
	$(CLANG) -arch $(1) $(CFLAGS_COMMON) $(CFLAGS_COMPILE_C) $(CFLAGS_COMPILE) -o $$@ -c $$<

$$($(1)_OBJS_M): $$($(1)_OBJ_DIR)/%.o : % | $$($(1)_OBJ_DIR)
	@echo Compiling $$(notdir $$@)
	@mkdir -p $$(dir $$@)
	$(CLANG) -arch $(1) $(CFLAGS_COMMON) $(CFLAGS_COMPILE_M) $(CFLAGS_COMPILE) -o $$@ -c $$<

$$($(1)_OBJS_MM): $$($(1)_OBJ_DIR)/%.o : % | $$($(1)_OBJ_DIR)
	@echo Compiling $$(notdir $$@)
	@mkdir -p $$(dir $$@)
	$(CLANG) -arch $(1) $(CFLAGS_COMMON) $(CFLAGS_COMPILE_MM) $(CFLAGS_COMPILE) -o $$@ -c $$<

$$($(1)_APP) : $$($(1)_OBJS)
	@echo Linking $(1) app
	@mkdir -p $$(dir $$@)
	$(CLANG)++ -arch $(1) $(CFLAGS_COMMON) -o $$@ $(FRAMEWORKS) $$($(1)_OBJS) $(CFLAGS_LINK)


endef

BUNDLE_DIR:=$(BUILD_DIR)/$(PROJECT_NAME).app
EXECUTABLE:=$(BUNDLE_DIR)/ios_app


$(BUNDLE_DIR): $(BUILD_DIR)
	@mkdir -p $@

$(BUNDLE_DIR)/assets: $(BUNDLE_DIR)
	rsync -rupE --delete $(PROJECT_ROOT)/assets $(BUNDLE_DIR)

$(BUNDLE_DIR)/embedded.mobileprovision: $(IOS_PROVISIONING_PROFILE) $(BUNDLE_DIR)
	@cp $(IOS_PROVISIONING_PROFILE) $@

.PHONY: $(BUNDLE_DIR)/assets

# Generate the rules for building each ABI
$(foreach abi, $(IOS_ABIS), $(eval $(call ABI_template,$(abi))))


# Use lipo to create a fat binary from all the ABI-specific binaries
APP_ABIS:=
$(foreach abi, $(IOS_ABIS), $(eval APP_ABIS+=$($(abi)_APP)) )


$(EXECUTABLE): $(APP_ABIS) $(BUNDLE_DIR)/assets $(BUNDLE_DIR)/embedded.mobileprovision
	$(XCODE_TOOLCHAIN)/usr/bin/lipo -create $(APP_ABIS) -output $(EXECUTABLE)
	touch -c $(BUNDLE_DIR)
	plutil -convert binary1 $(PROJECT_ROOT)/platform/ios/Info.plist -o $(BUNDLE_DIR)/Info.plist
	CODESIGN_ALLOCATE=$(XCODE_TOOLCHAIN)/usr/bin/codesign_allocate \
	codesign --force \
			 --sign $(IOS_CERT) \
			 --entitlements $(PROJECT_ROOT)/platform/ios/Entitlements.plist \
			 --timestamp=none \
			 $(BUNDLE_DIR)


ios: $(EXECUTABLE)



$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -f $(CPP_OBJ_FILES)
	@rm -f $(CPP_LIB)

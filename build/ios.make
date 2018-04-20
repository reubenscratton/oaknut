# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.

CFLAGS+= -DPLATFORM_IOS=1
FRAMEWORKS+= UIKit CoreGraphics OpenGLES

include $(OAKNUT_DIR)/build/apple.make


CFLAGS_COMMON := \
	$(CFLAGS) \
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


DEPS:=

define ABI_template

$(1)_OBJ_DIR:=$(OBJ_DIR)/$(1)
$(1)_OBJS:=$$(patsubst %,$$($(1)_OBJ_DIR)%.o,$(SOURCES))
$(1)_APP:=$$($(1)_OBJ_DIR)/ios_app

DEPS+=$$($(1)_OBJS:.o=.dep)


$$($(1)_OBJ_DIR)%.o : %
$$($(1)_OBJ_DIR)%.o : % $$($(1)_OBJ_DIR)%.dep
	@echo iOS\($(1)\): Compiling $$(notdir $$<)
	@mkdir -p $$(dir $$@)
	$(CLANG) -arch $(1) -MT $$@ -MD -MP -MF $$(@:.o=.Td) \
		$(CFLAGS_COMMON) \
		$(if $$(filter $$(suffix $$<),.cpp),-x c++ -std=c++11 $(WARNINGS_CPP),) \
        $(if $$(filter $$(suffix $$<),.c),-x c -std=gnu99,) \
        $(if $$(filter $$(suffix $$<),.m),-x objective-c -std=gnu99 -fobjc-arc $(WARNINGS_OBJC) -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0,) \
        $(if $$(filter $$(suffix $$<),.mm),-x objective-c++ -std=c++11 -fobjc-arc $(WARNINGS_CPP) $(WARNINGS_OBJC) -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0,) \
		$(CFLAGS_COMPILE) \
		$(if $(DEBUG),-g -O0,-O3) \
		-o $$@ -c $$<
	@mv -f $$(@:.o=.Td) $$(@:.o=.dep) && touch $$@


$$($(1)_APP) : $$($(1)_OBJS)
	@echo Linking $(1) app
	@mkdir -p $$(dir $$@)
	$(CLANG)++ -arch $(1) $(CFLAGS_COMMON) -o $$@ $(addprefix -framework ,$(FRAMEWORKS)) $$($(1)_OBJS) $(CFLAGS_LINK)


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

# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.


include $(OAKNUT_DIR)/build/apple.make

CFLAGS+= -DPLATFORM_MACOS=1
FRAMEWORKS+= AppKit OpenGL

CFLAGS_COMMON := \
    -stdlib=libc++ \
	-isysroot $(XCODE_APP)/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/$(MACOS_SDK) \
	$(CFLAGS) \
	$(FLAGS) \
	$(WARNINGS) \
	-Wunguarded-availability

BUNDLE_DIR:=$(BUILD_DIR)/$(PROJECT_NAME).app
EXECUTABLE:=$(BUNDLE_DIR)/Contents/MacOS/$(PROJECT_NAME)

ASSETS_DIR:=$(BUNDLE_DIR)/Contents/Resources
BUNDLED_ASSETS:=$(addprefix $(ASSETS_DIR)/assets/,$(subst $(PROJECT_ROOT)/assets/,,$(ASSETS)))

$(BUNDLE_DIR): $(BUILD_DIR)
	@mkdir -p $@

$(BUNDLED_ASSETS): $(ASSETS)
	@mkdir -p $(ASSETS_DIR)
	@rsync -rup --inplace --delete $(PROJECT_ROOT)/assets $(ASSETS_DIR)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)%.o : %
$(OBJ_DIR)%.o : % $(OBJ_DIR)%.dep
	@echo macOS: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CLANG) -arch x86_64 \
	 	-MT $@ -MD -MP -MF $(@:.o=.Td) \
	 	$(CFLAGS_COMMON) \
		$(if $(filter $(suffix $<),.cpp),-x c++ -std=c++11 -fasm-blocks,) \
		$(if $(filter $(suffix $<),.c),-x c -std=gnu99,) \
		$(if $(filter $(suffix $<),.m),-x objective-c -std=gnu99 -fobjc-arc -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0,) \
		$(if $(filter $(suffix $<),.mm),-x objective-c++ -std=c++11 -fobjc-arc  -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0,) \
		-isystem $(OAKNUT_DIR)/src \
		$(if $(DEBUG),-g -O0,-O3) \
		-c -o $@ $<
	@mv -f $(@:.o=.Td) $(@:.o=.dep) && touch $@


$(EXECUTABLE) : $(OBJS) $(BUNDLE_DIR) $(BUNDLED_ASSETS)
	@echo macOS: Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	@$(CLANG)++ -arch x86_64 -fobjc-link-runtime -dead_strip \
               -Xlinker -object_path_lto -Xlinker $(BUILD_DIR)/app_lto.o \
               -o $@ $(addprefix -framework ,$(FRAMEWORKS)) $(OBJS)
	@plutil -convert binary1 $(PROJECT_ROOT)/platform/macos/Info.plist -o $(BUNDLE_DIR)/Contents/Info.plist
	@touch -c $(BUNDLE_DIR)
	@CODESIGN_ALLOCATE=$(XCODE_TOOLCHAIN)/usr/bin/codesign_allocate \
	codesign --force \
			 --sign - \
			 --timestamp=none \
			 $(BUNDLE_DIR)


macos: $(EXECUTABLE)



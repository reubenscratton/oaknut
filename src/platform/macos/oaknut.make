# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.


include $(OAKNUT_DIR)/src/platform/apple/oaknut.make

CFLAGS+= -DPLATFORM_MACOS=1

CFLAGS_COMMON := \
    -stdlib=libc++ \
	-isysroot $(XCODE_APP)/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/$(MACOS_SDK) \
	$(CFLAGS) \
	$(FLAGS) \
	$(WARNINGS) \
	-Wunguarded-availability


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


CFLAGS_COMPILE_CPP:=-x c++ -std=c++11 -fasm-blocks
CFLAGS_COMPILE_C:=-x c -std=gnu99
CFLAGS_COMPILE_M:=-x objective-c -std=gnu99 -fobjc-arc -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0
CFLAGS_COMPILE_MM:=-x objective-c++ -std=c++11 -fobjc-arc  -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0


BUNDLE_DIR:=$(BUILD_DIR)/$(PROJECT_NAME).app
EXECUTABLE:=$(BUNDLE_DIR)/Contents/MacOS/macos_app
ASSETS_DIR:=$(BUNDLE_DIR)/Contents/Resources
OBJ_DIR:=$(BUILD_DIR)/obj
OBJS_CPP := $(patsubst %,$(OBJ_DIR)/%.o,$(SOURCES_CPP))
OBJS_C := $(patsubst %,$(OBJ_DIR)/%.o,$(SOURCES_C))
OBJS_M := $(patsubst %,$(OBJ_DIR)/%.o,$(SOURCES_M))
OBJS_MM := $(patsubst %,$(OBJ_DIR)/%.o,$(SOURCES_MM))
OBJS := $(OBJS_CPP) $(OBJS_C) $(OBJS_M) $(OBJS_MM)

$(BUNDLE_DIR): $(BUILD_DIR)
	@mkdir -p $@

$(ASSETS_DIR): $(BUNDLE_DIR)
	@mkdir -p $@

$(ASSETS_DIR)/assets: $(ASSETS_DIR)
	rsync -rupE --delete $(PROJECT_ROOT)/assets $(ASSETS_DIR)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJS_CPP): $(OBJ_DIR)/%.o : % | $(OBJ_DIR)
	@echo macOS: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	$(CLANG) -arch x86_64 $(CFLAGS_COMMON) $(CFLAGS_COMPILE_CPP) $(CFLAGS_COMPILE) -o $@ -c $<

$(OBJS_C): $(OBJ_DIR)/%.o : % | $(OBJ_DIR)
	@echo macOS: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	$(CLANG) -arch x86_64 $(CFLAGS_COMMON) $(CFLAGS_COMPILE_C) $(CFLAGS_COMPILE) -o $@ -c $<

$(OBJS_M): $(OBJ_DIR)/%.o : % | $(OBJ_DIR)
	@echo macOS: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	$(CLANG) -arch x86_64 $(CFLAGS_COMMON) $(CFLAGS_COMPILE_M) $(CFLAGS_COMPILE) -o $@ -c $<

$(OBJS_MM): $(OBJ_DIR)/%.o : % | $(OBJ_DIR)
	@echo macOS: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	$(CLANG) -arch x86_64 $(CFLAGS_COMMON) $(CFLAGS_COMPILE_MM) $(CFLAGS_COMPILE) -o $@ -c $<

$(EXECUTABLE) : $(OBJS) $(BUNDLE_DIR) $(ASSETS_DIR)/assets
	@echo macOS: Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	$(CLANG)++ -arch x86_64 $(CFLAGS_COMMON) -o $@ $(FRAMEWORKS) $(OBJS) $(CFLAGS_LINK)
	plutil -convert binary1 $(PROJECT_ROOT)/platform/macos/Info.plist -o $(BUNDLE_DIR)/Contents/Info.plist
	touch -c $(BUNDLE_DIR)
	CODESIGN_ALLOCATE=$(XCODE_TOOLCHAIN)/usr/bin/codesign_allocate \
	codesign --force \
			 --sign - \
			 --timestamp=none \
			 $(BUNDLE_DIR)

.PHONY: $(ASSETS_DIR)/assets

macos: $(EXECUTABLE)


$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -f $(CPP_OBJ_FILES)
	@rm -f $(CPP_LIB)

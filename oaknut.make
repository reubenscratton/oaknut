
# If no PLATFORM specified, default to the native one
UNAME := $(shell uname -s)
ifndef PLATFORM
ifeq ($(UNAME),Darwin)
PLATFORM:=macos
endif
ifeq ($(UNAME),Linux)
PLATFORM:=linux
endif
endif
ifndef PLATFORM
$(error Unsupported platform)
endif

# If no CONFIG specified, default to debug (and set DEBUG variable on)
ifndef CONFIG
CONFIG=debug
endif
ifneq (,$(findstring debug,$(CONFIG)))
DEBUG:=1
endif

# Default target
default: $(PLATFORM)


# Gather information about project files
PROJECT_ROOT := $(abspath .)

rwildcard=$(foreach d,$(filter-out %/webworkers,$(wildcard $1*)),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

ifndef SOURCES
SOURCES := $(call rwildcard,$(PROJECT_ROOT)/app/,*)
SOURCES += $(call rwildcard,$(PROJECT_ROOT)/src/,*)
SOURCES += $(call rwildcard,$(OAKNUT_DIR)/src/,*)
endif

ASSETS:=$(call rwildcard,$(PROJECT_ROOT)/assets/,*)

SOURCES := $(filter %.cpp %.c %.m %.mm,$(SOURCES))



# This bit builds a single platform+config. This will need templating if we want to build multiple.

BUILD_DIR := $(PROJECT_ROOT)/.build/$(PLATFORM)/$(CONFIG)
OBJ_DIR:=$(BUILD_DIR)/obj
PCH:=$(BUILD_DIR)/oaknut.pch

OBJS := $(patsubst %,$(OBJ_DIR)%.o,$(SOURCES))
DEPS:=$(OBJS:.o=.dep)

include $(OAKNUT_DIR)/build/$(PLATFORM).make


# Project file generators
XCODE_PROJECT_FILE=$(PROJECT_NAME).xcodeproj/project.pbxproj

$(XCODE_PROJECT_FILE):
	@mkdir -p $(dir $@)

xcode: $(XCODE_PROJECT_FILE)
	@perl $(OAKNUT_DIR)/build/xcode.pl -projectname $(PROJECT_NAME) $(addprefix -framework ,$(FRAMEWORKS)) $(addprefix -framework_macos ,$(FRAMEWORKS_MACOS)) $(addprefix -framework_ios ,$(FRAMEWORKS_IOS)) > $<

cmake:
	@perl $(OAKNUT_DIR)/build/cmake.pl -projectname $(PROJECT_NAME) > CMakeLists.txt

androidstudio:
	@mkdir -p  $(PROJECT_NAME).androidstudio/app
	@perl $(OAKNUT_DIR)/build/androidstudio.pl -projectname $(PROJECT_NAME) \
		-android_ver $(ANDROID_VER) \
		-package_name $(ANDROID_PACKAGE_NAME)


# This prevents make from automatically deleting .dep files cos it regards them as intermediate
.PRECIOUS: $(DEPS)

$(OBJ_DIR)%.dep: ;

$(BUILD_DIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

include $(wildcard $(DEPS))

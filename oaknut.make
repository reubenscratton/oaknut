
PROJECT_ROOT := $(abspath .)
BUILD_DIR := $(PROJECT_ROOT)/build/$(PLATFORM)

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

ifndef SOURCES
SOURCES := $(call rwildcard,$(PROJECT_ROOT)/app/,*)
SOURCES += $(call rwildcard,$(OAKNUT_DIR)/src/,*)
endif


SOURCES_CPP := $(filter %.cpp,$(SOURCES))
SOURCES_C := $(filter %.c,$(SOURCES))
SOURCES_M := $(filter %.m,$(SOURCES))
SOURCES_MM := $(filter %.mm,$(SOURCES))

ifdef OAKNUT_WANT_CAMERA
	CFLAGS+= -DOAKNUT_WANT_CAMERA
endif
ifdef OAKNUT_WANT_AUDIOINPUT
	CFLAGS+= -DOAKNUT_WANT_AUDIOINPUT
endif


include $(OAKNUT_DIR)/src/platform/$(PLATFORM)/oaknut.make


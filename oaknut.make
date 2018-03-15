
PROJECT_ROOT := $(abspath .)
BUILD_CONFIG=$(if $(DEBUG),debug,release)
BUILD_DIR := $(PROJECT_ROOT)/build/$(PLATFORM)/$(BUILD_CONFIG)
OBJ_DIR:=$(BUILD_DIR)/obj

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

ifndef SOURCES
SOURCES := $(call rwildcard,$(PROJECT_ROOT)/app/,*)
SOURCES += $(call rwildcard,$(OAKNUT_DIR)/src/,*)
endif

SOURCES := $(filter %.cpp %.c %.m %.mm,$(SOURCES))

OBJS := $(patsubst %,$(OBJ_DIR)%.o,$(SOURCES))
DEPS:=$(OBJS:.o=.d)

ifdef OAKNUT_WANT_CAMERA
	CFLAGS+= -DOAKNUT_WANT_CAMERA
endif
ifdef OAKNUT_WANT_AUDIOINPUT
	CFLAGS+= -DOAKNUT_WANT_AUDIOINPUT
endif


include $(OAKNUT_DIR)/src/platform/$(PLATFORM)/oaknut.make

$(OBJ_DIR)%.d: ;

$(BUILD_DIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

include $(wildcard $(DEPS))

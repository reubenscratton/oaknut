# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.

# TODO: WTF Linux? These should all be completely unnecessary...
SYSINCS:= -isystem /usr/include/gtk-2.0
SYSINCS+= -isystem /usr/include/gtkgl-2.0
SYSINCS+= -isystem /usr/include/glib-2.0
SYSINCS+= -isystem /usr/include/cairo
SYSINCS+= -isystem /usr/include/pango-1.0
SYSINCS+= -isystem /usr/include/gdk-pixbuf-2.0
SYSINCS+= -isystem /usr/include/atk-1.0
SYSINCS+= -isystem /home/osboxes/gtkglext
SYSINCS+= -isystem /usr/lib/gtkglext-1.0/include
SYSINCS+= -isystem /usr/lib/x86_64-linux-gnu/glib-2.0/include
SYSINCS+= -isystem /usr/lib/x86_64-linux-gnu/gtk-2.0/include

# /usr/lib/x86_64-linux-gnu/
LIBS:= \
    -lGLU -lGL -lglib-2.0 -lgobject-2.0 -latk-1.0 \
    -lgio-2.0 -lgthread-2.0 -lgmodule-2.0 -lgdk_pixbuf-2.0 \
    -lcairo -lpango-1.0 -lpangocairo-1.0 -lpangoft2-1.0 -lpangoxft-1.0 \
    -lgdk-x11-2.0 -lgtk-x11-2.0 -lgtkgl-2.0 \
    -lopenal -lpthread -lrt

CFLAGS+= -DPLATFORM_LINUX=1

CFLAGS_COMMON := \
	$(CFLAGS)

CFLAGS_COMPILE := \
    -isystem $(OAKNUT_DIR)/src \
    $(SYSINCS) \
    -g \
	-c \
    -O0

CFLAGS_LINK :=
CFLAGS_COMPILE_CPP:=-x c++ -std=c++11
CFLAGS_COMPILE_C:=-x c -std=gnu99


EXECUTABLE:=$(BUILD_DIR)/$(PROJECT_NAME)
OBJ_DIR:=$(BUILD_DIR)/obj
OBJS_CPP := $(patsubst %,$(OBJ_DIR)/%.o,$(SOURCES_CPP))
OBJS_C := $(patsubst %,$(OBJ_DIR)/%.o,$(SOURCES_C))
OBJS := $(OBJS_CPP) $(OBJS_C)

$(BUNDLE_DIR): $(BUILD_DIR)
	@mkdir -p $@

#$(ASSETS_DIR): $(BUNDLE_DIR)
#	@mkdir -p $@

#$(ASSETS_DIR)/assets: $(ASSETS_DIR)
#	rsync -rupE --delete $(PROJECT_ROOT)/assets $(ASSETS_DIR)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJS_CPP): $(OBJ_DIR)/%.o : % | $(OBJ_DIR)
	@echo Compiling $(notdir $@)
	@mkdir -p $(dir $@)
	g++ $(CFLAGS_COMMON) $(CFLAGS_COMPILE_CPP) $(CFLAGS_COMPILE) -o $@ -c $<

$(OBJS_C): $(OBJ_DIR)/%.o : % | $(OBJ_DIR)
	@echo Compiling $(notdir $@)
	@mkdir -p $(dir $@)
	g++ $(CFLAGS_COMMON) $(CFLAGS_COMPILE_C) $(CFLAGS_COMPILE) -o $@ -c $<


$(EXECUTABLE) : $(OBJS) $(BUNDLE_DIR) $(ASSETS_DIR)/assets
	@echo Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	g++ $(CFLAGS_COMMON) -o $@ $(OBJS) $(LIBS) $(CFLAGS_LINK)

.PHONY: $(ASSETS_DIR)/assets

linux: $(EXECUTABLE)


$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -f $(CPP_OBJ_FILES)
	@rm -f $(CPP_LIB)

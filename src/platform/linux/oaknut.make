# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.

# TODO: WTF Linux? These should all be completely unnecessary...
SYSINCS:= -isystem /usr/include/gtk-3.0
SYSINCS+= -isystem /usr/include/glib-2.0
SYSINCS+= -isystem /usr/lib/x86_64-linux-gnu/glib-2.0/include
SYSINCS+= -isystem /usr/include/pango-1.0
SYSINCS+= -isystem /usr/include/cairo
SYSINCS+= -isystem /usr/include/harfbuzz
SYSINCS+= -isystem /usr/include/freetype2
SYSINCS+= -isystem /usr/include/gdk-pixbuf-2.0
SYSINCS+= -isystem /usr/include/atk-1.0

LIBS:= \
    -lGLU -lGL -lglib-2.0 -lgobject-2.0 -latk-1.0 \
    -lgtk-3 -lgdk-3 -lpango-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lfreetype -lharfbuzz \
    -lopenal -lpthread -lrt -lcurl


EXECUTABLE:=$(BUILD_DIR)/$(PROJECT_NAME)
ASSETS_DIR:=$(BUILD_DIR)

$(BUNDLE_DIR): $(BUILD_DIR)
	@mkdir -p $@

$(ASSETS_DIR)/assets: $(ASSETS_DIR)
	rsync -rupE --delete $(PROJECT_ROOT)/assets $(ASSETS_DIR)

$(OBJ_DIR)%.o : %
$(OBJ_DIR)%.o : % $(OBJ_DIR)%.d
	@echo linux: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	g++ $(CFLAGS) \
		$(if $(filter $(suffix $<),.cpp),-x c++ -std=c++11,) \
		$(if $(filter $(suffix $<),.c),-x c -std=gnu99,) \
		$(if $(DEBUG),-g -O0,-O3) \
		-DPLATFORM_LINUX=1 \
        -isystem $(OAKNUT_DIR)/src \
        $(SYSINCS) \
		-o $@ -c $<
	@mv -f $(@:.o=.Td) $(@:.o=.d) && touch $@


$(EXECUTABLE) : $(OBJS) $(BUNDLE_DIR) $(ASSETS_DIR)/assets
	@echo Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	g++ -o $@ $(OBJS) $(LIBS)


linux: $(EXECUTABLE)


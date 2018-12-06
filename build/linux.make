# This makefile is expected to be include'd by PROJECT_ROOT/proj/Makefile
# It should not be run directly.

# To set up clean Ubuntu for Oaknut :
# sudo apt-get install build-essential \
			libgtk-3-dev \
			libgl1-mesa-dev \
			freeglut3-dev \
			libcurl3-dev \
			libopenal-dev \
			clang

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
    -lGLU -lGL -lgobject-2.0 -latk-1.0 \
    -lgtk-3 -lgdk-3 -lpango-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lfreetype -lharfbuzz \
    -lopenal -lpthread -lrt -lcurl -lglib-2.0 -lgio-2.0

# Use clang! If you really must use GCC then remove the .pch stuff since
# GCC (a) does it differently, and (b) the .gch produced is so vast (132MB)
# that it ends up not saving any time at all.
CXX:=clang++

# Filter out everything not .cpp
OBJS:=$(filter %.cpp.o,$(OBJS))

EXECUTABLE:=$(BUILD_DIR)/$(PROJECT_NAME)
ASSETS_DIR:=$(BUILD_DIR)

$(BUNDLE_DIR): $(BUILD_DIR)
	@mkdir -p $@

$(ASSETS_DIR)/assets: $(ASSETS_DIR)
	rsync -rupE --delete $(PROJECT_ROOT)/assets $(ASSETS_DIR)

ALLOPTS:= $(CFLAGS) \
				  $(if $(DEBUG),-g -O0,-O3) \
					-DPLATFORM_LINUX=1 $(OPTS) \
					-isystem $(OAKNUT_DIR)/src \
					$(SYSINCS)

# GCC precompiled header has to be .gch, not .pch. The compiler will find it automatically.
#PCH:=$(BUILD_DIR)/oaknut.gch
#PCH_OPT:=-isystem $(BUILD_DIR)
PCH_OPT:=-include-pch $(PCH)

$(PCH).dep : $(OAKNUT_DIR)/src/oaknut.h
	@mkdir -p $(dir $@)
	@$(CXX) $(ALLOPTS) -std=c++11 -x c++-header -E -M -MT $(PCH) $< -o $(PCH).dep
	@echo "	$(CXX) $(ALLOPTS) -std=c++11 -x c++-header $(OAKNUT_DIR)/src/oaknut.h -emit-pch -o $(PCH)" >>$@

-include $(PCH).dep

$(OBJ_DIR)%.o : %
$(OBJ_DIR)%.o : % $(OBJ_DIR)%.dep
	@echo linux: Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CXX) \
	 	-MT $@ -MD -MP -MF $(@:.o=.Td) \
		$(if $(filter $(suffix $<),.cpp),-x c++ -std=c++11,) \
		$(PCH_OPT) \
		$(ALLOPTS) \
		-o $@ -c $<
	@mv -f $(@:.o=.Td) $(@:.o=.dep) && touch $@

$(EXECUTABLE) : $(PCH).dep $(PCH) $(OBJS) $(BUNDLE_DIR) $(ASSETS_DIR)/assets
	@echo Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	@$(CXX) -o $@ $(OBJS) $(LIBS)


linux: $(EXECUTABLE)

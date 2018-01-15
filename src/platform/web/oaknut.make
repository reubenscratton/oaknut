
CFLAGS+= -DPLATFORM_WEB=1

HTML_FILE := $(OAKNUT_DIR)/src/platform/web/web.html

OUTPUT_DIR := $(BUILD_DIR)/webroot
OBJ_DIR:=$(BUILD_DIR)/obj
OBJS_CPP := $(patsubst %,$(OBJ_DIR)%.bc,$(SOURCES_CPP))
OBJS_C := $(patsubst %,$(OBJ_DIR)%.bc,$(SOURCES_C))
OBJS := $(OBJS_CPP) $(OBJS_C)

#OPTS:=-O3 -s TOTAL_MEMORY=33554432
#OPTS:=-O2 -s TOTAL_MEMORY=33554432
OPTS:=-O0 --profiling -s USE_PTHREADS=1 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=33554432 -DEMSCRIPTEN
#OPTS:=-O3 -s TOTAL_MEMORY=33554432


$(OBJS_CPP): $(OBJ_DIR)%.bc : % | $(OBJ_DIR)
	@echo Compiling $(notdir $@)
	@mkdir -p $(dir $@)
	@$(EMSCRIPTEN_DIR)/emcc --bind -std=c++11 $(CFLAGS) $(OPTS) -I$(OAKNUT_DIR)/src $< -o $@

$(OBJS_C): $(OBJ_DIR)%.bc : % | $(OBJ_DIR)
	@echo Compiling $(notdir $@)
	@mkdir -p $(dir $@)
	@$(EMSCRIPTEN_DIR)/emcc --bind $(CFLAGS) $(OPTS) -I$(OAKNUT_DIR)/src $< -o $@

EXECUTABLE=$(OUTPUT_DIR)/xx.html

$(EXECUTABLE): $(OBJS) $(HTML_FILE)
	@echo Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	@$(EMSCRIPTEN_DIR)/emcc --bind $(OPTS) --emrun --preload-file $(PROJECT_ROOT)/assets@/assets --shell-file $(HTML_FILE) $(OBJS) -o $@



$(OBJ_DIR):
	@mkdir -p $@

web: $(EXECUTABLE)


clean:
	@rm -rf $(OBJS)

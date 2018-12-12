
ifndef EMSCRIPTEN_ROOT
$(error EMSCRIPTEN_ROOT must point to the root of the Emscripten SDK)
endif

HTML_FILE := $(OAKNUT_DIR)/src/platform/web/web.html
OUTPUT_DIR := $(BUILD_DIR)/webroot
OBJS:=$(OBJS:.o=.bc)

# Remove obj-c sources
OBJS:=$(filter-out %.m.bc,$(OBJS))
OBJS:=$(filter-out %.mm.bc,$(OBJS))

#OPTS+=$(if $(DEBUG),-O0 --profiling -s DEMANGLE_SUPPORT=1,-O3)
#OPTS+= -O0 --profiling -s DEMANGLE_SUPPORT=1
OPTS+= -O3

#PThreads is broken on WASM cos there's no Atomics support (it exists but is disabled cos of Spectre)
#OPTS+= -s USE_PTHREADS=1

ALLOPTS:= $(CFLAGS) -DPLATFORM_WEB=1 -DEMSCRIPTEN $(OPTS) \
         -I$(OAKNUT_DIR)/src \
         -isystem $(EMSCRIPTEN_ROOT)/system/include \
         -s TOTAL_MEMORY=33554432

$(PCH).dep : $(OAKNUT_DIR)/src/oaknut.h
	@mkdir -p $(dir $@)
	@$(EMSCRIPTEN_ROOT)/emcc $(ALLOPTS) -std=c++11 -x c++-header -E -M -MT $(PCH) $< -o $(PCH).dep
	echo "	$(EMSCRIPTEN_ROOT)/emcc $(ALLOPTS) -std=c++11 -x c++-header $(OAKNUT_DIR)/src/oaknut.h -emit-pch -o $(PCH)" >>$@

-include $(PCH).dep
#DEPS := $(PCH).dep $(DEPS)


$(OBJ_DIR)%.bc : % $(OBJ_DIR)%.dep
	@echo $(PLATFORM): Compiling $(notdir $<)
	@mkdir -p $(dir $@)
	@$(EMSCRIPTEN_ROOT)/emcc  \
        -MT $@ -MD -MP -MF $(@:.bc=.Td) \
        $(if $(filter $(suffix $<),.cpp),-std=c++11,) \
        $(ALLOPTS) \
	-include-pch $(PCH) \
	$< -o $@
	@mv -f $(@:.bc=.Td) $(@:.bc=.dep) && touch $@

lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))


# Function that generates the target for a worker
define build_worker

$(1)_JS=$(OUTPUT_DIR)/$(1).js
tmpvar=$$(foreach v,$$(SOURCES),$$(if $$(findstring worker,$$(v)),$$(v)))
$(1)_SOURCES=$$(foreach v,$$(tmpvar),$$(if $$(findstring $$(call lowercase,$(1)),$$v),$$v))

$$($(1)_JS) : $$($(1)_SOURCES)
	$(EMSCRIPTEN_ROOT)/emcc \
		-s EXPORTED_FUNCTIONS="['_ww_start', '_ww_process', '_ww_stop']" \
		-s BUILD_AS_WORKER=1 \
		$(OPTS) \
		-DPLATFORM_WEB=1 \
		-DBUILD_AS_WORKER=1 \
		-DUSE_WORKER_$(1)=1 \
		-isystem $(OAKNUT_DIR)/src \
		-std=c++11 \
		$$($(1)_SOURCES) \
		$(OAKNUT_DIR)/src/app/app.cpp \
		$(OAKNUT_DIR)/src/app/styles.cpp \
		$(OAKNUT_DIR)/src/app/worker.cpp \
		$(OAKNUT_DIR)/src/base/*.cpp \
		$(OAKNUT_DIR)/src/data/*.cpp \
		$(OAKNUT_DIR)/src/text/stringprocessor.cpp \
		-o $$@

endef


# Generate targets for workers
$(foreach worker,$(WORKERS),$(eval $(call build_worker,$(worker))))

#$(error FaceDetector sources is $(FaceDetector_SOURCES))

#WORKERS_JS=$(addsuffix .js,$(WORKERS))
WORKERS_JS=$(patsubst %,$(OUTPUT_DIR)/%.js,$(WORKERS))


ASSET_DIRS = $(shell find $(PROJECT_ROOT)/assets/ -type d 2>/dev/null)
ASSET_FILES = $(shell find $(PROJECT_ROOT)/assets -type f -name '*' 2>/dev/null)

EXECUTABLE=$(OUTPUT_DIR)/xx.html
$(EXECUTABLE): $(PCH).dep $(PCH) $(OBJS) $(HTML_FILE) $(ASSET_DIRS) $(ASSET_FILES) $(WORKERS_JS)
	@echo $(PLATFORM): Linking app
	@mkdir -p $(dir $(EXECUTABLE))
	@$(EMSCRIPTEN_ROOT)/emcc --bind $(OPTS) --emrun --preload-file $(PROJECT_ROOT)/assets@/assets --shell-file $(HTML_FILE) $(OBJS) -o $@

OPTS+= -s WASM=0

include $(OAKNUT_DIR)/build/web.make

web_asmjs: $(EXECUTABLE)

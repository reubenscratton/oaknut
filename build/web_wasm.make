OPTS+= -s BINARYEN_TRAP_MODE=clamp

include $(OAKNUT_DIR)/build/web.make

web_wasm: $(EXECUTABLE)

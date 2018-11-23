#!/bin/bash
if [ $# -ne 2 ]; then
    echo "Usage: build_worker myworker.cpp MyWorkerName.js"
    exit 0;
fi

OPTS="-O3 -s WASM=1 -s BINARYEN_TRAP_MODE=clamp"
#OPTS="-O0 -s WASM=0  "

emcc -s EXPORTED_FUNCTIONS="['_ww_start', '_ww_process', '_ww_stop']" \
  -s BUILD_AS_WORKER=1 \
  $OPTS \
  -DPLATFORM_WEB=1 \
  -DBUILD_AS_WORKER=1 \
  -isystem $OAKNUT_DIR/src \
  -std=c++11 \
  $1 \
  $OAKNUT_DIR/src/app/app.cpp \
  $OAKNUT_DIR/src/app/styles.cpp \
  $OAKNUT_DIR/src/app/worker.cpp \
  $OAKNUT_DIR/src/base/*.cpp \
  $OAKNUT_DIR/src/data/*.cpp \
  $OAKNUT_DIR/src/text/stringprocessor.cpp \
  -o $2

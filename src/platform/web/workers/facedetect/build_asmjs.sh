emcc -O0 -s EXPORTED_FUNCTIONS="['_ww_start', '_ww_process', '_ww_stop']" \
  -s BUILD_AS_WORKER=1 \
  -s WASM=0 \
  -DPLATFORM_WEB=1 \
  -DBUILD_AS_WORKER=1 \
  -isystem $OAKNUT_DIR/src \
  -std=c++11 \
  worker.cpp \
  $OAKNUT_DIR/src/app/app.cpp \
  $OAKNUT_DIR/src/app/styles.cpp \
  $OAKNUT_DIR/src/base/*.cpp \
  $OAKNUT_DIR/src/data/*.cpp \
  $OAKNUT_DIR/src/text/stringprocessor.cpp \
  -o facedetect.js

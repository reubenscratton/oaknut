# Oaknut's CMake file. This file is expected to be include()d by the CMakeLists.txt in your project root.
#
# CMake is only used to define the source tree and build targets for IDEs. Actual build
# rules are left to makefiles. This approach allows us to trivially generate project files
# for XCode et al when needed, while modern IDEs like CLion are supported out of the box.


# NB: CMake docs say to not use glob to build source lists but I don't see the harm in it.
# All you have to do is remember to reset your CMake cache when you add or remove source files.
# To me that's a faintly better deal than having to remember to add and remove things from
# CMakeLists explicitly.
file(GLOB_RECURSE SOURCES  "app/*.*" "assets/*.*" "platform/*.*" "${CMAKE_CURRENT_LIST_DIR}/src/*.*")

# Web
add_custom_target(${PROJECT_NAME}_web COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=web
        PROJECT_NAME=${PROJECT_NAME}
        DEBUG=${DEBUG}
        SOURCES ${SOURCES})

# Android
add_custom_target(${PROJECT_NAME}_android COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=android
        PROJECT_NAME=${PROJECT_NAME}
        DEBUG=${DEBUG}
        SOURCES ${SOURCES})

# Linux
if(UNIX AND NOT APPLE)
add_custom_target(${PROJECT_NAME}_linux COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=linux
        PROJECT_NAME=${PROJECT_NAME}
        DEBUG=${DEBUG}
        SOURCES ${SOURCES})
add_definitions(-DPLATFORM_LINUX=1)
endif()

if (APPLE)

    # MacOS
    add_custom_target(${PROJECT_NAME}_macos COMMAND make -C ${PROJECT_SOURCE_DIR}
            PLATFORM=macos
            PROJECT_NAME=${PROJECT_NAME}
            DEBUG=${DEBUG}
            SOURCES ${SOURCES})

    # iOS
    add_custom_target(${PROJECT_NAME}_ios COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=ios
        PROJECT_NAME=${PROJECT_NAME}
        DEBUG=${DEBUG}
        SOURCES ${SOURCES})

endif ()

# Ignore this target, it exists so CLion's Intellisense detects Oaknut source code
# Cmake does not let you specify include directories for a custom target, very silly
add_executable(${PROJECT_NAME}_do_not_build ${SOURCES})
target_include_directories(${PROJECT_NAME}_do_not_build SYSTEM PUBLIC $ENV{OAKNUT_DIR}/src
    /usr/include/gtk-3.0
        /usr/include/pango-1.0
        /usr/include/harfbuzz
        /usr/include/freetype2
        /usr/include/gdk-pixbuf-2.0
        /usr/include/cairo
        /usr/include/glib-2.0)



# Oaknut's CMake file. This file is expected to be include()d by the CMakeLists.txt in your project root.
#
# CMake is only used to define the source tree and build targets for IDEs. Actual build
# rules are left to makefiles. This approach allows us to trivially generate project files
# for XCode et al when needed, while modern IDEs like CLion are supported out of the box.


# NB: CMake docs say to not use glob to build source lists but I don't see the harm in it.
# All you have to do is remember to reset your CMake cache when you add or remove source files.
# To me that's a faintly better deal than having to remember to add and remove things from
# CMakeLists explicitly. Really CMake is broken in this regard, it should track source dirs
# rather than individual files and be run as a prebuild step. Alternatively IDEs could just be
# intelligent about makefiles, but I guess that'll never happen.
file(GLOB_RECURSE SOURCES_APP  "app/*.*")
file(GLOB_RECURSE SOURCES_ASSETS  "assets/*.*")
file(GLOB_RECURSE SOURCES_PLATFORM  "platform/*.*")
file(GLOB_RECURSE SOURCES_OAKNUT  "${CMAKE_CURRENT_LIST_DIR}/src/*.*")
set(SOURCES ${SOURCES_APP} ${SOURCES_ASSETS} ${SOURCES_PLATFORM} ${SOURCES_OAKNUT})

# Web
set(PROJECT_NAME_WEB ${PROJECT_NAME}_web)
add_custom_target(${PROJECT_NAME_WEB} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=web web SOURCES ${SOURCES})

# Android
set(PROJECT_NAME_ANDROID ${PROJECT_NAME}_android)
add_custom_target(${PROJECT_NAME_ANDROID} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=android android SOURCES ${SOURCES})

# Linux
set(PROJECT_NAME_LINUX ${PROJECT_NAME}_linux)
add_custom_target(${PROJECT_NAME_LINUX} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=linux
        PROJECT_NAME=${PROJECT_NAME_LINUX}
        linux SOURCES ${SOURCES})
add_definitions(-DPLATFORM_LINUX=1)

if (APPLE)

    # MacOS
    set(PROJECT_NAME_MACOS ${PROJECT_NAME}_macos)
    add_custom_target(${PROJECT_NAME_MACOS} COMMAND make -C ${PROJECT_SOURCE_DIR}
            PLATFORM=macos
            PROJECT_NAME=${PROJECT_NAME_MACOS}
            macos SOURCES ${SOURCES})

    # iOS
    set(PROJECT_NAME_IOS ${PROJECT_NAME}_ios)
    add_custom_target(${PROJECT_NAME_IOS} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=ios
        PROJECT_NAME=${PROJECT_NAME_IOS}
        ios SOURCES ${SOURCES})

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



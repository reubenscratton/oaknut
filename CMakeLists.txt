# Oaknut's CMake file. This file is expected to be include()d by the CMakeLists.txt in your project root.
#
# CMake is only used to define the source tree and build targets. Actual build rules
# are left to makefiles. This approach allows us to trivially generate project files
# for XCode et al when needed, while modern IDEs like CLion are supported out of the box.


file(GLOB_RECURSE SOURCE_FILES_APP  "app/*.*")
file(GLOB_RECURSE SOURCE_FILES_GENERIC  "${CMAKE_CURRENT_LIST_DIR}/src/*.*")
file(GLOB_RECURSE SOURCE_FILES_APPLE  "${CMAKE_CURRENT_LIST_DIR}/platform/apple/*.mm")
file(GLOB_RECURSE SOURCE_FILES_IOS  "${CMAKE_CURRENT_LIST_DIR}/platform/ios/*.m*")
file(GLOB_RECURSE SOURCE_FILES_MACOS  "${CMAKE_CURRENT_LIST_DIR}/platform/macos/*.m*")
file(GLOB_RECURSE SOURCE_FILES_WEB  "${CMAKE_CURRENT_LIST_DIR}/platform/web/*.cpp")
file(GLOB_RECURSE SOURCE_FILES_ANDROID  "${CMAKE_CURRENT_LIST_DIR}/platform/android/*.cpp")

set(ASSETS "${PROJECT_SOURCE_DIR}/assets")

include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/src)


# Web
set(PROJECT_NAME_WEB ${PROJECT_NAME}_web)
set(SOURCES_WEB ${SOURCE_FILES_GENERIC} ${SOURCE_FILES_WEB} ${SOURCE_FILES_APP} ${ASSET_FILES})
add_custom_target(${PROJECT_NAME_WEB} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=web
        SOURCES="${SOURCES_WEB}"
        web
        SOURCES ${SOURCES_WEB})

# Android
set(PROJECT_NAME_ANDROID ${PROJECT_NAME}_android)
set(SOURCES_ANDROID ${SOURCE_FILES_GENERIC} ${SOURCE_FILES_ANDROID} ${SOURCE_FILES_APP} ${ASSET_FILES})
add_custom_target(${PROJECT_NAME_ANDROID} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=android
        SOURCES="${SOURCES_ANDROID}"
        android
        SOURCES ${SOURCES_ANDROID})

if (APPLE)

    # MacOS
    set(PROJECT_NAME_MACOS ${PROJECT_NAME}_macos)
    set(SOURCES_MACOS ${SOURCE_FILES_GENERIC} ${SOURCE_FILES_APPLE} ${SOURCE_FILES_MACOS} ${SOURCE_FILES_APP} ${ASSET_FILES})
    add_custom_target(${PROJECT_NAME_MACOS} COMMAND make -C ${PROJECT_SOURCE_DIR}
            PLATFORM=macos
            PROJECT_NAME=${PROJECT_NAME_MACOS}
            SOURCES="${SOURCES_MACOS}"
            macos
            SOURCES ${SOURCES_MACOS})

    # iOS
    set(PROJECT_NAME_IOS ${PROJECT_NAME}_ios)
    set(SOURCES_IOS ${SOURCE_FILES_GENERIC} ${SOURCE_FILES_APPLE} ${SOURCE_FILES_IOS} ${SOURCE_FILES_APP} ${ASSET_FILES})
    add_custom_target(${PROJECT_NAME_IOS} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=ios
        PROJECT_NAME=${PROJECT_NAME_IOS}
        SOURCES="${SOURCES_IOS}"
        ios
        SOURCES ${SOURCES_IOS})

endif ()


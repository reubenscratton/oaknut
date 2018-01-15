# Oaknut's CMake file. This file is expected to be include()d by the CMakeLists.txt in your project root.
#
# CMake is only used to define the source tree and build targets for IDEs. Actual build
# rules are left to makefiles. This approach allows us to trivially generate project files
# for XCode et al when needed, while modern IDEs like CLion are supported out of the box.


file(GLOB_RECURSE SOURCES_APP  "app/*.*")
file(GLOB_RECURSE SOURCES_ASSETS  "assets/*.*")
file(GLOB_RECURSE SOURCES_PLATFORM  "platform/*.*")
file(GLOB_RECURSE SOURCES_OAKNUT  "${CMAKE_CURRENT_LIST_DIR}/src/*.*")
set(SOURCES ${SOURCES_APP} ${SOURCES_ASSETS} ${SOURCES_PLATFORM} ${SOURCES_OAKNUT})


include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/src)


# Web
set(PROJECT_NAME_WEB ${PROJECT_NAME}_web)
add_custom_target(${PROJECT_NAME_WEB} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=web web SOURCES ${SOURCES})

# Android
set(PROJECT_NAME_ANDROID ${PROJECT_NAME}_android)
add_custom_target(${PROJECT_NAME_ANDROID} COMMAND make -C ${PROJECT_SOURCE_DIR}
        PLATFORM=android android SOURCES ${SOURCES})

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


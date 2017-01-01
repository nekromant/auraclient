CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(PkgConfig)

set(PROJECT_VERSION   0.1.3)
set(PROJECT_DOC_VERSION   gitmaster)

set(CORE_SRCS
    src/auracore.c
    src/eventqueue.c
)

include_directories(include/)

file(GLOB TESTS src/*.c)
foreach(t ${TESTS})

    add_executable()
endforeach()

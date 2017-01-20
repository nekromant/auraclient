CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(PkgConfig)

set(PROJECT_VERSION   0.1.3)
set(PROJECT_DOC_VERSION   gitmaster)

set(CORE_SRCS
    src/auracore.c
    src/eventqueue.c
    src/backend-hostdummy.c
)

include_directories(include/)

add_library(auraclient STATIC ${CORE_SRCS})

file(GLOB TESTS src/tests/*.c)
foreach(t ${TESTS})
    get_filename_component(NAME ${t} NAME_WE)
    add_executable(${NAME} ${t})
    target_link_libraries(${NAME} auraclient)
endforeach()

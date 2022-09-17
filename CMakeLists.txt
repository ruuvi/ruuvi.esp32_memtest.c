# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)

set(EXPECTED_IDF_VERSION v4.4.2)

execute_process (
        COMMAND bash -c "cd $ENV{IDF_PATH} && git describe --always --tags --dirty --match ${EXPECTED_IDF_VERSION}*"
        OUTPUT_VARIABLE IDF_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if("${IDF_VERSION}" STREQUAL "${EXPECTED_IDF_VERSION}")
    message("esp-idf version: ${IDF_VERSION}")
elseif("${IDF_VERSION}" STREQUAL "${EXPECTED_IDF_VERSION}-dirty")
    message(WARNING "Expected esp-idf version is ${EXPECTED_IDF_VERSION}, but actual version is ${IDF_VERSION}")
else()
    message(FATAL_ERROR "Expected esp-idf version is ${EXPECTED_IDF_VERSION}, but actual version is ${IDF_VERSION}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffile-prefix-map=${CMAKE_CURRENT_LIST_DIR}/=")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffile-prefix-map=$ENV{IDF_PATH}/=esp-idf/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffile-prefix-map=${CMAKE_CURRENT_LIST_DIR}/=")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffile-prefix-map=$ENV{IDF_PATH}/=esp-idf/")

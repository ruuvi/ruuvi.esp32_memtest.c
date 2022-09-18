idf_component_register(SRCS
        "bootloader_start.c"
        "test.c"
        "test.h"
        "random.c"
        "memtest_random.h"
        "memtest.c"
        "memtest.h"
        REQUIRES bootloader bootloader_support)

idf_build_get_property(target IDF_TARGET)

set(scripts "./bootloader.ld"
        "./bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")

target_include_directories(${COMPONENT_LIB} PUBLIC "${IDF_PATH}/components/spi_flash/include")
target_include_directories(${COMPONENT_LIB} PUBLIC "${IDF_PATH}/components/bootloader_support/include_bootloader")

target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_0=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_1=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_3=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_5=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_6=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_8=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_9=1)
target_compile_definitions(${COMPONENT_LIB} PUBLIC MEMTEST_DO_TEST_10=0)

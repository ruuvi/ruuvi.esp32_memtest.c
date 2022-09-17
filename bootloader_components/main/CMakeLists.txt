idf_component_register(SRCS
        "bootloader_start.c"
        "test.c"
        "test.h"
        "random.c"
        "memtest_random.h"
        REQUIRES bootloader bootloader_support)

idf_build_get_property(target IDF_TARGET)

set(scripts "./bootloader.ld"
        "./bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")

target_include_directories(${COMPONENT_LIB} PUBLIC "${IDF_PATH}/components/spi_flash/include")
target_include_directories(${COMPONENT_LIB} PUBLIC "${IDF_PATH}/components/bootloader_support/include_bootloader")

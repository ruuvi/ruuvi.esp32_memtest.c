# ruuvi.esp32_memtest.c
A memory test for ESP32 chip. Does not test 100 % of memory.

This memory test is based on the open source memory testing tool [MemTest86+](https://www.memtest.org).
Here is the original archive with the source code: [memtest86+-5.01](https://www.memtest.org/download/archives/5.01/memtest86+-5.01.tar.gz).

The memory test firmware is implemented as a second stage bootloader.
It is based on an example from ESP-IDF v4.4.2: "examples/custom_bootloader/bootloader_override"

## How to use it

Simply compile it:
```
idf.py build
```

And flash it with the following commands (it flashes the test app, which is not necessary):
```
idf.py flash [-p <port>] monitor
```
Press **'Ctrl+]'** twice to quit from UART monitoring.

To flash only the bootloader with the MemTest, use following command:
```
esptool.py -p (PORT) -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin
```

## Organisation of the second stage bootloader with the MemTest

This project contains an application, in the `main` directory that represents a user program.
It also contains a `bootloader_components` directory that, as it name states, 
contains a component that will override the current bootloader implementation.

Below is a short explanation of files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c                   - User application (unused)
├── bootloader_components        - bootloader with the MemTest
│   └── main
│       ├── CMakeLists.txt   
│       ├── ld/                  - linker scripts
│       │   └── ...
│       ├── bootloader_start.c   - Implementation of the second stage bootloader
│       ├── memtest.c            - the glue code between the bootloader and the MemTest code
│       ├── memtest.h            - the glue code between the bootloader and the MemTest code
│       ├── memtest_random.h     - MemTest86+ port for ESP32
│       ├── random.c             - MemTest86+ port for ESP32
│       ├── test.c               - MemTest86+ port for ESP32
│       └── test.h               - MemTest86+ port for ESP32
└── README.md                    - This is the file you are currently reading
```

As stated in the `README.md` file in the upper level, when the bootloader components is named `main`, it overrides
the whole second stage bootloader code.

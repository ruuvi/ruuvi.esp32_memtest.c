# ruuvi.esp32_memtest.c
A memory test for ESP32 chip. Does not test 100 % of memory.

The memory test firmware is implemented as a second stage bootloader.
It is based on an example from ESP-IDF v4.4.2: "examples/custom_bootloader/bootloader_override"

## How to use it

Simply compile it:
```
idf.py build
```

And flash it with the following commands:
```
idf.py flash
```

## Organisation of the original second stage bootloader example

This project contains an application, in the `main` directory that represents a user program.
It also contains a `bootloader_components` directory that, as it name states, 
contains a component that will override the current bootloader implementation.

Below is a short explanation of files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c                 User application
├── bootloader_components
│   └── main
│       ├── component.mk          
│       ├── CMakeLists.txt   
│       ├── ld/
│       │   └── ...
│       └── bootloader_start.c Implementation of the second stage bootloader
└── README.md                  This is the file you are currently reading
```

As stated in the `README.md` file in the upper level, when the bootloader components is named `main`, it overrides
the whole second stage bootloader code.

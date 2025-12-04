# Cross-compilation для ARM
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Компиляторы
set(CMAKE_C_COMPILER "C:/SysGCC/arm-eabi/bin/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "C:/SysGCC/arm-eabi/bin/arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "C:/SysGCC/arm-eabi/bin/arm-none-eabi-gcc.exe")

# ОТКЛЮЧАЕМ тест компилятора - это ключевой момент!
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Настройки поиска
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Флаги по умолчанию (для STM32 Cortex-M)
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")
set(CMAKE_ASM_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-specs=nosys.specs -T${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld")
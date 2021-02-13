MCU = atmega8
MCU_CORE = m8
DEVICE_PATH = /dev/ttyUSB0

COMPILER = avr-g++
COMPILER_FLAGS = -mmcu=$(MCU) -Wall -Os
COMPILER_FLAGS_DEBUG = -mmcu=$(MCU) -Wall -g3 -O0 -funsigned-char -funsigned-bitfields
MAIN_ELF = main.elf
MAIN_OBJ = main.o
MAIN_HEX = main.hex

all: to_hex check_size clean

# главный файл
main: main.cpp base.hpp tm1637.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c main.cpp -o $(MAIN_OBJ)
	$(COMPILER) $(COMPILER_FLAGS) $(MAIN_OBJ) -o $(MAIN_ELF)

# преобразование в формат для записи в мк
to_hex: main
	avr-objcopy -O ihex $(MAIN_ELF) $(MAIN_HEX)

# вывод размера программы
check_size: main
	avr-size --mcu=$(MCU) --format=avr $(MAIN_ELF)

# удаление ненужных файлов
clean: check_size
	rm $(MAIN_OBJ) $(MAIN_ELF)

firmware: $(MAIN_HEX)
	avrdude -c arduino -p $(MCU_CORE) -P $(DEVICE_PATH) -b 19200 -U flash:w:$(MAIN_HEX)

debug_build: main.cpp base.hpp tm1637.cpp
	$(COMPILER) $(COMPILER_FLAGS_DEBUG) -c main.cpp -o main_dbg.elf

read_eeprom:
	avrdude -c arduino -p $(MCU_CORE) -P $(DEVICE_PATH) -b 19200 -Ueeprom:r:"eeprom_backup".eep:i

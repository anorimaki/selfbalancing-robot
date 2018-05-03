ARDUINO_ESP8288_TAG=2.4.0

# Directories
BUILD_DIR=build
ESP_ROOT=$(BUILD_DIR)/esp8266
FS_DIR=data

#Other libraries
PIC_CODE_API_DIR=../pic/include/api
HTML_APP_DIR=../../client/html/dist

ARDUINO_CORE_LIBS:=Wire ESP8266WiFi ArduinoOTA ESP8266mDNS
PRIVATE_LIBS:=$(filter-out lib/readme.txt,$(wildcard lib/*))
LIBS:=$(patsubst %,$(ESP_ROOT)/libraries/%,$(ARDUINO_CORE_LIBS)) $(PRIVATE_LIBS)

UPLOAD_PORT?=COM6			#Default COM6

default:
	make all

$(FS_DIR): $(HTML_APP_DIR)
	rm -rf $(FS_DIR)
	mkdir -p $(FS_DIR)
	cp -r $(HTML_APP_DIR) $(FS_DIR)/app

flash_fs: $(FS_DIR)

$(ESP_ROOT): |  $(BUILD_DIR)
	git clone --depth 1 --branch $(ARDUINO_ESP8288_TAG) https://github.com/esp8266/Arduino.git $(ESP_ROOT)

$(ESP_ROOT)/tools/xtensa-lx106-elf/bin: | $(ESP_ROOT)
	cd $(ESP_ROOT)/tools && python get.py
ifeq ($(OS),Windows_NT)	
	chmod +x $(ESP_ROOT)/tools/esptool/esptool.exe
	chmod +x $(ESP_ROOT)/tools/mkspiffs/mkspiffs.exe
endif

$(ESP_ROOT)/.patch: $(ESP_ROOT)/tools/xtensa-lx106-elf/bin arduino_esp8266.patch
	cd $(ESP_ROOT) && patch -p1 < ../../arduino_esp8266.patch 
	touch $(ESP_ROOT)/.patch

$(filter-out clean data,$(MAKECMDGOALS)): $(ESP_ROOT)/.patch
	make -f makeEspArduino.mk BUILD_DIR=$(BUILD_DIR)/bin \
		BOARD="nodemcuv2" \
		ESP_ROOT=$(ESP_ROOT) \
		EXCLUDE_DIRS=$(BUILD_DIR) \
		LIBS="$(LIBS)" \
		BUILD_EXTRA_FLAGS="-Iinclude -I$(PIC_CODE_API_DIR)" \
		IGNORE_PATTERN=dummy \
		F_CPU=160000000L \
		SKETCH=src/selfbalancing.cpp \
		FLASH_DEF=4M3M \
		FS_DIR=$(FS_DIR) \
		UPLOAD_SPEED=115200 \
		UPLOAD_PORT=$(UPLOAD_PORT) \
		$(MAKECMDGOALS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
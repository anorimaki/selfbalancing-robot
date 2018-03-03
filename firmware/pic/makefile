#########################################
## Directories definitions
INCLUDE_DIRS := include mpl/mcc_generated_files
SRC_DIRS := src mpl/mcc_generated_files
BUILD_DIR := build
XC16_DIR := "/c/app/Microchip/xc16/v1.33/bin/"
IPE_DIR := "/c/app/Microchip/MPLABX/v4.05/mplab_ipe/"


#########################################
### Options
ifndef SINGLE_THREAD
  # Use multithreaded builds by default
  MAKEFLAGS += -j
endif

ifndef VERBOSE
  # Set silent mode as default
  MAKEFLAGS += --silent
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

PIC_PROCESSOR=24FJ32GA002


#########################################
## Build toolchain
XC_CC=$(XC16_DIR)xc16-gcc.exe
XC_AS=$(XC16_DIR)xc16-as.exe
XC_LD=$(XC16_DIR)xc16-ld.exe
XC_AR=$(XC16_DIR)xc16-ar.exe
XC_BIN2HEX=$(XC16_DIR)xc16-bin2hex

PK3CMD=$(IPE_DIR)pk3cmd.exe


#########################################
## Extensions
OBJ_EXT = o
DEP_EXT = d
EXE_EXT = elf
OUTPUT_SUFFIX = hex

#########################################
# Targets
APP_NAME=selfbalancing
FINAL_IMAGE=$(BUILD_DIR)/$(APP_NAME).$(OUTPUT_SUFFIX)

.PHONY: all
all: $(FINAL_IMAGE)

flash: $(FINAL_IMAGE)
	$(PK3CMD) -P$(PIC_PROCESSOR) -F$(FINAL_IMAGE) -V3.25 -M
	$(PK3CMD) -P$(PIC_PROCESSOR) -L


#########################################
## Compiler stage
CND_CONF=default
SRC_FILES := $(shell find $(SRC_DIRS) -name "*.s" -o -name "*.c")
SRC_DIRS := $(sort $(dir $(SRC_FILES)))

INCLUDES_FLAGS := $(foreach dir,$(INCLUDE_DIRS),-I$(dir))

MP_PROCESSOR_OPTION=$(PIC_PROCESSOR)

VPATH += $(SRC_DIRS)

C_FLAGS=-c -mcpu=$(MP_PROCESSOR_OPTION) -MMD -MF "$@.d" -g -omf=elf \
  -save-temps=obj -DXPRJ_default=$(CND_CONF) \
  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections \
  -fdata-sections -msmall-code -O2 $(INCLUDES_FLAGS) -msmart-io=1 -Wall -msfr-warn=off

AS_FLAGS=-c -mcpu=$(MP_PROCESSOR_OPTION) -omf=elf -save-temps=obj -DXPRJ_default=default \
  -legacy-libc  \
  -Wa,-MD,"$@.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax

$(BUILD_DIR)/%.c.$(OBJ_EXT): %.c | $(BUILD_DIR)
	echo  $(<F)
	$(XC_CC) $(C_FLAGS) $< -o $@

$(BUILD_DIR)/%.s.$(OBJ_EXT): %.s | $(BUILD_DIR)
	echo  $(<F)
	$(XC_CC) $(AS_FLAGS) $< -o $@

# Include all available dependencies
-include $(wildcard $(BUILD_DIR)/*.$(DEP_EXT))


#########################################
## Linker stage
OBJ_FILES := $(patsubst %,$(BUILD_DIR)/%.$(OBJ_EXT),$(notdir $(SRC_FILES)))

MP_LINKER_FILE_OPTION=,--script=p$(PIC_PROCESSOR).gld

L_GCC_FLAGS=-mcpu=$(MP_PROCESSOR_OPTION) -omf=elf -save-temps=obj -DXPRJ_default=$(CND_CONF) \
  -legacy-libc  $(COMPARISON_BUILD)

L_LD_FLAGS=--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--no-isr,--gc-sections,--fill-upper=0,--stackguard=16,--library=q,--no-force-link,--smart-io,-Map="$(BUILD_DIR)/$(APP_NAME).map",--report-mem,--memorysummary,$(BUILD_DIR)/memoryfile.xml

L_FLAGS = $(L_GCC_FLAGS) -Wl,,,$(L_LD_FLAGS)

$(FINAL_IMAGE): $(OBJ_FILES)   
	 ${XC_CC} $(OBJ_FILES) $(L_FLAGS) -o $(BUILD_DIR)/$(APP_NAME).$(EXE_EXT)
	${XC_BIN2HEX} $(BUILD_DIR)/$(APP_NAME).$(EXE_EXT) -a  -omf=elf 


#########################################
## Directory targets
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


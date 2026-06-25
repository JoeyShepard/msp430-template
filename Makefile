PROJECT=project
DEVICE=MSP430G2553

SRC_DIR=src
BUILD_DIR=build

MAP=$(BUILD_DIR)/$(PROJECT).map

GCC_DIR = /opt/msp430-gcc/bin
SUPPORT_FILE_DIRECTORY = /opt/msp430-gcc/include

AS      = $(GCC_DIR)/msp430-elf-as
CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb
OBJCOPY = $(GCC_DIR)/msp430-elf-objcopy

#a - turn on listings
#g - include general information, like as version and options passed
#h - include high-level source
#l - include assembly
#n - omit forms processing
#s - include symbols
ASFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -Iinclude -mmcu=$(DEVICE) -g -aghlns=$(BUILD_DIR)/$(notdir $<.lst)
CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -Iinclude -mmcu=$(DEVICE) -O2 -Wall -g -Wa,-aghlns=$(BUILD_DIR)/$(notdir $<.lst)
#Recompile if file includes header that changed
CFLAGS += -MMD -MP
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -Wl,-Map,$(MAP),--gc-sections 
GDBFLAGS = --quiet --ex "layout src" --ex "layout regs" --ex "break main"
#GDBFLAGS = --quiet -x gdb.txt

#Source files
C_FILES=$(filter-out $(PRIM_C_FILES), $(wildcard $(SRC_DIR)/*.c))
ASM_FILES=$(wildcard $(SRC_DIR)/*.S)
OBJS=$(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS_ASM=$(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

#For recompiling when headers have changed
DEPS=$(OBJS:.o=.d)

all: $(BUILD_DIR)/$(PROJECT)
	$(OBJCOPY) -O ihex $(BUILD_DIR)/$(PROJECT).out $(BUILD_DIR)/$(PROJECT).hex
	size $(BUILD_DIR)/$(PROJECT).out

$(BUILD_DIR)/$(PROJECT): $(OBJS) $(OBJS_ASM)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(PROJECT).out $^ $(LFLAGS)

$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(OBJS_ASM): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	$(AS) $(ASFLAGS) -o $@ $< 

sim: all
	mspdebug sim "prog $(BUILD_DIR)/$(PROJECT).out" "opt quiet true" gdb > /dev/null &
	$(GDB) --se $(BUILD_DIR)/$(PROJECT).out $(GDBFLAGS) --ex "target remote localhost:2000" --ex continue

debug: all
	sudo -v
	sudo mspdebug tilib "prog $(BUILD_DIR)/$(PROJECT).out" "opt quiet true" gdb

connect: all
	$(GDB) --se $(BUILD_DIR)/$(PROJECT).out $(GDBFLAGS) --ex "target remote localhost:2000" --ex continue

flash: all
	sudo -v
	sudo mspdebug tilib "prog $(BUILD_DIR)/$(PROJECT).out"


.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*

-include $(DEPS)

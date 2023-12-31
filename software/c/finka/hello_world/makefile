# This is a minimal hello world using -nostdlib and -ffreestanding
# showing custom C run time, linker script etc.

PROJ_NAME=hello_world
DEBUG=yes
BENCH=no
MULDIV=yes

SRCS = 	$(wildcard ../hal/*.c) \
		$(wildcard src/*.c) \
		$(wildcard src/*.cpp)  \
		$(wildcard src/*.S)

OBJDIR = build

INC  = -I../hal
LIBS =
LIBSINC = -L$(OBJDIR)
LDSCRIPT = ./src/linker.ld

OPENOCD = /opt/openocd-vexriscv/bin/openocd

RISCV_PATH ?= /opt/x-tools/riscv32-unknown-elf
RISCV_NAME ?= riscv32-unknown-elf

MABI=ilp32
MARCH := rv32i
ifeq ($(MULDIV),yes)
	MARCH := $(MARCH)m
endif
ifeq ($(COMPRESSED),yes)
	MARCH := $(MARCH)ac
endif
MARCH := $(MARCH)_zicsr_zifencei

CFLAGS +=  -march=$(MARCH) -mabi=$(MABI) -DNDEBUG
LDFLAGS += -march=$(MARCH) -mabi=$(MABI)

#include ../../../resources/subproject.mk

ifeq ($(DEBUG),yes)
	CFLAGS += -g3 -O0
endif

ifeq ($(DEBUG),no)
	CFLAGS += -g -Os
endif

ifeq ($(BENCH),yes)
	CFLAGS += -fno-inline
endif

RISCV_CLIB=$(RISCV_PATH)/$(RISCV_NAME)/lib/

RISCV_OBJCOPY = $(RISCV_PATH)/bin/$(RISCV_NAME)-objcopy
RISCV_OBJDUMP = $(RISCV_PATH)/bin/$(RISCV_NAME)-objdump
RISCV_CC=$(RISCV_PATH)/bin/$(RISCV_NAME)-gcc
RISCV_GDB=$(RISCV_PATH)/bin/$(RISCV_NAME)-gdb

CFLAGS +=  -MD -fstrict-volatile-bitfields -fno-strict-aliasing -save-temps
LDFLAGS +=  -nostdlib -lgcc -mcmodel=medany -nostartfiles -ffreestanding -Wl,-Bstatic,-T,$(LDSCRIPT),-Map,$(OBJDIR)/$(PROJ_NAME).map,--print-memory-usage
#LDFLAGS += -lgcc -lc -lg -nostdlib  -lgcc  -msave-restore    --strip-debug,

OBJS := $(SRCS)
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.S=.o)
OBJS := $(OBJS:..=miaou)
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))


all: $(OBJDIR)/$(PROJ_NAME).elf $(OBJDIR)/$(PROJ_NAME).hex $(OBJDIR)/$(PROJ_NAME).asm $(OBJDIR)/$(PROJ_NAME).v

$(OBJS): | $(OBJDIR)

$(OBJDIR)/%.elf: $(OBJS)
	$(RISCV_CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBSINC) $(LIBS)

%.hex: %.elf
	$(RISCV_OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(RISCV_OBJCOPY) -O binary $^ $@

%.v: %.elf
	$(RISCV_OBJCOPY) -O verilog $^ $@

%.asm: %.elf
	$(RISCV_OBJDUMP) -S -d $^ > $@

$(OBJDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS)  $(INC) -o $@ $^
	$(RISCV_CC) -S $(CFLAGS)  $(INC) -o $@.disasm $^

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS)  $(INC) -o $@ $^

$(OBJDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS) -o $@ $^ -D__ASSEMBLY__=1

$(OBJDIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/src
	rm -f $(OBJDIR)/$(PROJ_NAME).elf
	rm -f $(OBJDIR)/$(PROJ_NAME).hex
	rm -f $(OBJDIR)/$(PROJ_NAME).map
	rm -f $(OBJDIR)/$(PROJ_NAME).v
	rm -f $(OBJDIR)/$(PROJ_NAME).asm
	find $(OBJDIR) -type f -name '*.o' -print0 | xargs -0 -r rm
	find $(OBJDIR) -type f -name '*.d' -print0 | xargs -0 -r rm

clean-all : clean 

.SECONDARY: $(OBJS)

# Debug the program using GDB, which starts OpenOCD ((VexRiscv) with GDB pipe
debug: $(OBJDIR)/$(PROJ_NAME).elf
	$(RISCV_GDB) \
	-ex "set remotetimeout 30" \
	-ex "target extended-remote | $(OPENOCD) -c \"gdb_port pipe; log_output openocd.log\" -c \"set MURAX_CPU0_YAML ../../../../cpu0.yaml\" -f \"interface/jtag_tcp.cfg\" -f \"./finka.cfg\""  \
	-ex "set arch riscv:rv32" \
	-ex "monitor reset halt" \
	-ex "set pagination off" \
	-ex "layout split" \
	-ex "load" \
	-ex "echo \\n" \
	-ex "echo \\n" \
	-ex "break main" \
	-ex "x/4i 0x800000" \
	$(OBJDIR)/$(PROJ_NAME).elf

#	-ex "cont" \
#	-ex "cont" \
#	-ex "cont" \
#	-ex "cont" \
#	-ex "quit" \

#	-ex "cont" \
#	-ex "cont" \
#	-ex "display/i $$pc" \
#	-ex "cont" \
#	-ex "cont" \
#	-ex "stepi" \
#	-ex "stepi" \
#	-ex "stepi" \
#	-ex "stepi" \
	-ex "echo >>> GDB will break at main. Type 'next' <enter> to proceed to the next C line. <<<\\n" \

#	-ex "set remote hardware-breakpoint-limit 2" \

# Debug the program using GDB, which starts OpenOCD (VexRiscv) with GDB pipe
batch_debug: $(OBJDIR)/$(PROJ_NAME).elf
	$(RISCV_GDB) --batch \
	-ex "set remotetimeout 30" \
	-ex "target extended-remote | $(OPENOCD) -c \"gdb_port pipe; log_output openocd.log\" -c \"set MURAX_CPU0_YAML ../../../../cpu0.yaml\" -f \"interface/jtag_tcp.cfg\" -f \"./finka.cfg\""  \
	-ex "set arch riscv:rv32" \
	-ex "monitor reset halt" \
	-ex "set pagination off" \
	-ex "load" \
	-ex "x/4i 0x800000" \
	-ex "cont" \
	$(OBJDIR)/$(PROJ_NAME).elf

upload: $(OBJDIR)/$(PROJ_NAME).elf
	$(RISCV_GDB) --batch \
	-ex "set remotetimeout 30" \
	-ex "target extended-remote | $(OPENOCD) -c \"gdb_port pipe; log_output openocd.log\" -c \"set MURAX_CPU0_YAML ../../../../cpu0.yaml\" -f \"interface/jtag_tcp.cfg\" -f \"./finka.cfg\""  \
	-ex "set arch riscv:rv32" \
	-ex "monitor reset halt" \
	-ex "set pagination off" \
	-ex "load" \
	-ex "x/4i 0x800000" \
	-ex "quit" \
	$(OBJDIR)/$(PROJ_NAME).elf

# Debug the program using GDB, which starts OpenOCD ((VexRiscv) with GDB pipe
attach: $(OBJDIR)/$(PROJ_NAME).elf
	$(RISCV_GDB) \
	-ex "set remotetimeout 30" \
	-ex "target extended-remote | $(OPENOCD) -c \"gdb_port pipe; log_output openocd.log\" -c \"set MURAX_CPU0_YAML ../../../../cpu0.yaml\" -f \"interface/jtag_tcp.cfg\" -f \"./finka.cfg\""  \
	-ex "set arch riscv:rv32" \
	-ex "monitor reset halt" \
	$(OBJDIR)/$(PROJ_NAME).elf

openocd:
	$(OPENOCD) -c "set MURAX_CPU0_YAML ../../../../cpu0.yaml" -f interface/jtag_tcp.cfg -f ./finka.cfg

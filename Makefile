.PHONY: all clean flash
.DEFAULT_GOAL = all

PROGRAM = fan

MCU = attiny25
CLOCK = 8000000

AVRDUDE = avrdude -v -P /dev/ttyUSB0 -c buspirate -p $(MCU)

CC = avr-gcc
OBJCOPY = avr-objcopy
CPUFLAGS = -mmcu=$(MCU) -DF_CPU=$(CLOCK)
LDFLAGS = $(CPUFLAGS) -Os -flto
CFLAGS = $(LDFLAGS)
CFLAGS += -std=c11 -pedantic -pedantic-errors
CFLAGS += -Wall -Wextra -Werror

SRCS = main.c io.c adc.c led.c fan.c

OBJS=$(SRCS:%.c=%.o)
DEPS = $(OBJS:.o=.d)
ifneq ($(MAKECMDGOALS),clean)
  -include $(DEPS)
endif

all: $(PROGRAM).elf
clean:
	-rm -f $(OBJS) $(DEPS) $(PROGRAM).elf

flash: $(PROGRAM).elf
	$(AVRDUDE) -U flash:w:$<:e -U lfuse:w:$<:e -U hfuse:w:$<:e -U efuse:w:$<:e

$(PROGRAM).elf : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	chmod -x $@
	@echo -e "Firmware size is\033[1m" `avr-size --format=GNU fan.elf | cut -c 33-43 | tail -n 1` "\033[0mbytes ("$@")"

%.d : %.c
	$(CC) $(CPUFLAGS) -M -MP -MG -MQ $@ -MQ $(<:%.c=%.o) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

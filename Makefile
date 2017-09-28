TARGET		= bbvm 
CC 			= gcc
INCLUDE		= ./include
R_CFLAGS 	= -I$(INCLUDE) -Wall -std=c99 -O3 -flto
D_CFLAGS 	= -I$(INCLUDE) -std=c99 -O0 -g
R_LFLAGS	= -lffi
D_LFLAGS	= -lffi 
SRC			= $(wildcard src/*.c)
R_OBJ 		= $(patsubst %.c,release/obj/%.o,$(SRC))
D_OBJ 		= $(patsubst %.c,debug/obj/%.o,$(SRC))

CORES ?= $(shell sysctl -n hw.ncpu || echo 1)

all:; @$(MAKE) _all -j$(CORES)
_all: debug release
.PHONY: all _all

debug:; @$(MAKE) _debug -j$(CORES)
_debug: $(D_OBJ)
	@mkdir -p bin
	@echo "----------------------------"
	@echo "Building Debug Target $(TARGET)"
	@echo
	@$(CC) $(D_LFLAGS) -o bin/$(TARGET) $?

.PHONY: debug _debug

release:; @$(MAKE) _release -j$(CORES)
_release: $(R_OBJ)
	@mkdir -p bin
	@echo "------------------------------"
	@echo "Building Release Target $(TARGET)"
	@echo
	@$(CC) $(R_LFLAGS) -o bin/$(TARGET) $?

.PHONY: release _release

debug/obj/%.o: %.c
	@mkdir -p debug/obj/src
	@echo "Compiling $<"
	@$(CC) $(D_CFLAGS) -c -o $@ $<	

release/obj/%.o: %.c
	@mkdir -p release/obj/src
	@echo "Compiling $<"
	@$(CC) $(R_CFLAGS) -c -o $@ $<

clean:
	@\rm -f debug/obj/src/*.o
	@\rm -f release/obj/src/*.o
	@\rm -f bin/$(TARGET)

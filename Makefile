TARGET		= bbvm 
CC 			= gcc
INCLUDE		= ./include
R_CFLAGS 	= -I$(INCLUDE) -Wall -std=c99 -O3 -flto
D_CFLAGS 	= -I$(INCLUDE) -std=c99 -O0 -g
R_LFLAGS	= -lffi -ll
D_LFLAGS	= -lffi -ll
SRC			= $(wildcard src/*.c)
R_OBJ 		= $(patsubst %.c,obj/release/%.o,$(SRC))
D_OBJ 		= $(patsubst %.c,obj/debug/%.o,$(SRC))
LEX_PRE		= lex.yy
YACC_PRE	= y.tab

CORES ?= $(shell sysctl -n hw.ncpu || echo 1)

all:; @$(MAKE) _all -j$(CORES)
_all: debug release
.PHONY: all _all

debug:; @$(MAKE) _debug -j$(CORES)
_debug: $(D_OBJ) obj/debug/$(LEX_PRE).o obj/debug/$(YACC_PRE).o
	@mkdir -p bin
	@echo "----------------------------"
	@echo "Building Debug Target $(TARGET)"
	@echo
	@$(CC) $(D_LFLAGS) -o bin/$(TARGET) $?

.PHONY: debug _debug

release:; @$(MAKE) _release -j$(CORES)
_release: $(R_OBJ) obj/release/$(LEX_PRE).o obj/release/$(YACC_PRE).o
	@mkdir -p bin
	@echo "------------------------------"
	@echo "Building Release Target $(TARGET)"
	@echo
	@$(CC) $(R_LFLAGS) -o bin/$(TARGET) $?


.PHONY: release _release

obj/debug/$(LEX_PRE).o: $(LEX_PRE).c
	@mkdir -p obj/debug/src
	@echo "Compiling $<"
	@$(CC) $(D_CFLAGS) -c -o $@ $<	

obj/release/$(LEX_PRE).o: $(LEX_PRE).c
	@mkdir -p obj/debug/src
	@echo "Compiling $<"
	@$(CC) $(R_CFLAGS) -c -o $@ $<	

$(LEX_PRE).c: $(YACC_PRE).c
	@echo "Creating Lexer from BBVM.l"
	@lex BBVM.l

obj/debug/$(YACC_PRE).o: $(YACC_PRE).c
	@mkdir -p obj/debug/src
	@echo "Compiling $<"
	@$(CC) $(D_CFLAGS) -c -o $@ $<	

obj/release/$(YACC_PRE).o: $(YACC_PRE).c
	@mkdir -p obj/debug/src
	@echo "Compiling $<"
	@$(CC) $(R_CFLAGS) -c -o $@ $<	

$(YACC_PRE).c:
	@echo "Creating Parser from BBVM.y"
	@yacc -d BBVM.y

obj/debug/%.o: %.c
	@mkdir -p obj/debug/src
	@echo "Compiling $<"
	@$(CC) $(D_CFLAGS) -c -o $@ $<

obj/release/%.o: %.c
	@mkdir -p obj/release/src
	@echo "Compiling $<"
	@$(CC) $(R_CFLAGS) -c -o $@ $<

clean:
	@\rm -f obj/debug/src/*.o
	@\rm -f obj/debug/*.o
	@\rm -f obj/release/src/*.o
	@\rm -f obj/release/*.o
	@\rm -f $(LEX_PRE).c
	@\rm -f $(YACC_PRE).c
	@\rm -f $(YACC_PRE).h
	@\rm -f bin/$(TARGET)

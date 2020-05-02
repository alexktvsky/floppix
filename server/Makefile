BIN_FILE = server

SRC_FILES += main.c
SRC_FILES += errors.c
SRC_FILES += cycle.c
SRC_FILES += log.c
SRC_FILES += config.c
SRC_FILES += connection.c
SRC_FILES += mempool.c
SRC_FILES += list.c
SRC_FILES += memory.c
SRC_FILES += files.c
SRC_FILES += errno.c
SRC_FILES += process.c
SRC_FILES += sockets.c
SRC_FILES += time.c
SRC_FILES += threads.c
SRC_FILES += events.c
SRC_FILES += select.c
SRC_FILES += epoll.c
SRC_FILES += event_connect.c
SRC_FILES += event_read.c
SRC_FILES += event_write.c
SRC_FILES += event_timer.c

VPATH += src/server
VPATH += src/event
VPATH += src/os
VPATH += src/os/events

INSTALL_DIR = /usr/local/bin

override CFLAGS += -Iinclude
override CFLAGS += -Wall -Wextra -Wuninitialized -pipe
build: override CFLAGS += -g0 -s -O3 -DNDEBUG
debug: override CFLAGS += -g3 -ggdb3 -O0 -DDEBUG

override LDFLAGS += -lpcre
build: override LDFLAGS += -static -static-libgcc

CC = gcc

ifeq ($(CC), gcc)
	GCC_VER = $(shell $(CC) -v 2>&1 | grep 'gcc version' 2>&1 \
		| sed -e 's/^.* version \(.*\)/\1/')
	override CFLAGS += -DHCNSE_COMPILER="\"gcc $(GCC_VER)\""
	override LDFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

ifeq ($(CC), clang)
	CLANG_VER = $(shell $(CC) -v 2>&1 | grep 'version' 2>&1 \
		| sed -n -e 's/^.*clang version \(.*\)/\1/p' \
		-e 's/^.*LLVM version \(.*\)/\1/p')
	override CFLAGS += -DHCNSE_COMPILER="\"clang $(CLANG_VER)\""
	override CFLAGS += -Qunused-arguments
	override LDFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

ifeq ($(CC), x86_64-w64-mingw32-gcc)
	MINGW_VER = $(shell $(CC) -v 2>&1 | grep 'gcc version' 2>&1 \
		| sed -e 's/^.* version \(.*\)/\1/')
	override CFLAGS += -DHCNSE_COMPILER="\"mingw $(MINGW_VER)\""
	override CFLAGS += -D__USE_MINGW_ANSI_STDIO
	override LDFLAGS += -lws2_32
endif

ifeq ($(CC), i686-w64-mingw32-gcc)
	MINGW_VER = $(shell $(CC) -v 2>&1 | grep 'gcc version' 2>&1 \
		| sed -e 's/^.* version \(.*\)/\1/')
	override CFLAGS += -DHCNSE_COMPILER="\"mingw $(MINGW_VER)\""
	override CFLAGS += -D__USE_MINGW_ANSI_STDIO
	override LDFLAGS += -lws2_32
endif


OBJ_FILES := $(patsubst %.c,obj/%.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

#.ONESHELL:
.PHONY: build debug test docs

all: build
build: mkdirs _build
debug: mkdirs _debug

_build: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

_debug: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

test:
	$(warning Tests now is not available!)

docs:
	doxygen docs/Doxyfile

obj/%.o: %.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)

install: mkdirs build
	install bin/$(BIN_FILE) $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(BIN_FILE)

clean:
	rm -f obj/*
	rm -f bin/*

mkdirs: 
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi

	@if [ ! -d obj ]; then \
		mkdir obj; \
	fi

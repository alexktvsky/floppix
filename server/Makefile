BIN_FILE = server

SRC_FILES += main.c
SRC_FILES += hcnse_cycle.c
SRC_FILES += hcnse_log.c
SRC_FILES += hcnse_config.c
SRC_FILES += hcnse_connection.c
SRC_FILES += hcnse_mempool.c
SRC_FILES += hcnse_list.c
SRC_FILES += hcnse_memory.c
SRC_FILES += hcnse_string.c
SRC_FILES += hcnse_file.c
SRC_FILES += hcnse_errno.c
SRC_FILES += hcnse_process.c
SRC_FILES += hcnse_socket.c
SRC_FILES += hcnse_time.c
SRC_FILES += hcnse_thread.c
SRC_FILES += hcnse_mutex.c
SRC_FILES += hcnse_semaphore.c
SRC_FILES += hcnse_signal.c
SRC_FILES += hcnse_event.c
SRC_FILES += hcnse_select.c
SRC_FILES += hcnse_epoll.c
SRC_FILES += hcnse_event_connect.c
SRC_FILES += hcnse_event_read.c
SRC_FILES += hcnse_event_write.c
SRC_FILES += hcnse_event_timer.c

VPATH += src/core
VPATH += src/event
VPATH += src/os

INSTALL_DIR = /usr/local/bin

override CFLAGS += -Iinclude
override CFLAGS += -Wall -Wextra -Wuninitialized -pipe
build: override CFLAGS += -g0 -s -O3
debug: override CFLAGS += -g3 -ggdb3 -O0
debug: override CFLAGS +=  -DHCNSE_DEBUG -DHCNSE_HAVE_VALGRIND

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

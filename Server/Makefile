BIN_FILE = server

SRC_FILES += main.c
SRC_FILES += errors.c
SRC_FILES += cycle.c
# SRC_FILES += log.c
SRC_FILES += config.c
SRC_FILES += connection.c
SRC_FILES += memory.c
SRC_FILES += mempool.c
SRC_FILES += list.c
SRC_FILES += sys_files.c
SRC_FILES += sys_errno.c
SRC_FILES += sys_process.c
SRC_FILES += sys_sockets.c
SRC_FILES += event_connect.c
SRC_FILES += event_read.c
SRC_FILES += event_write.c
SRC_FILES += event_timer.c

CC = gcc

CFLAGS += -Iinclude -Wall -Wextra -Wuninitialized -pipe
build: CFLAGS += -g0 -s -O3 -DNDEBUG
debug: CFLAGS += -g3 -ggdb3 -O0 -DDEBUG

LDFLAGS += -lpcre
build: LDFLAGS += -static -static-libgcc

ifeq ($(CC), gcc)
	LDFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

ifeq ($(CC), clang)
	CFLAGS += -Qunused-arguments
	LDFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

ifeq ($(CC), x86_64-w64-mingw32-gcc)
	LDFLAGS += -lws2_32
endif

ifeq ($(CC), i686-w64-mingw32-gcc)
	LDFLAGS += -lws2_32
endif



OBJ_FILES := $(patsubst %.c,obj/%.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

VPATH := src/core src/event src/os

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
	install bin/$(BIN_FILE) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(BIN_FILE)

clean:
	rm -f obj/*
	rm -f bin/*

mkdirs: 
	@if [ ! -d bin ] ; then \
		mkdir bin ; \
	fi

	@if [ ! -d obj ] ; then \
		mkdir obj ; \
	fi

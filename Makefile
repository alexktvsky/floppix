SRC_FILES += main.c
SRC_FILES += hcnse_memory.c
SRC_FILES += hcnse_string.c
SRC_FILES += hcnse_filesys.c
SRC_FILES += hcnse_errno.c
SRC_FILES += hcnse_socket.c
SRC_FILES += hcnse_sockopt.c
SRC_FILES += hcnse_sockaddr.c
SRC_FILES += hcnse_time.c
SRC_FILES += hcnse_thread.c
SRC_FILES += hcnse_mutex.c
SRC_FILES += hcnse_semaphore.c
SRC_FILES += hcnse_cpuinfo.c
SRC_FILES += hcnse_pool.c
SRC_FILES += hcnse_log.c
SRC_FILES += hcnse_list.c
SRC_FILES += hcnse_array.c
SRC_FILES += hcnse_config.c
SRC_FILES += hcnse_parse.c
SRC_FILES += hcnse_core.c
SRC_FILES += hcnse_listen.c
SRC_FILES += hcnse_connection.c
SRC_FILES += hcnse_process.c
SRC_FILES += hcnse_dso.c
SRC_FILES += hcnse_module.c
SRC_FILES += hcnse_cycle.c

# SRC_FILES += hcnse_signal.c
# SRC_FILES += hcnse_mmap.c
# SRC_FILES += hcnse_send.c
# SRC_FILES += hcnse_rbtree.c
# SRC_FILES += hcnse_select.c
# SRC_FILES += hcnse_epoll.c
# SRC_FILES += hcnse_iocp.c
# SRC_FILES += hcnse_event.c
# SRC_FILES += hcnse_event_connect.c
# SRC_FILES += hcnse_event_read.c
# SRC_FILES += hcnse_event_write.c
# SRC_FILES += hcnse_event_timer.c

# ifeq ($(target), linux)
# 	VPATH += src/os/unix
# endif

# ifeq ($(target), win32)
# 	VPATH += src/os/win32
# endif

VPATH += src/core
VPATH += src/os

MODULE_DIRS += src/module/test1
MODULE_DIRS += src/module/test2

BIN_FILE = hcnse
INSTALL_DIR = /usr/local/bin

override CFLAGS += -Iinclude
override CFLAGS += -Wall -Wextra
# override CFLAGS += -Wuninitialized
override CFLAGS += -Wno-uninitialized
override CFLAGS += -std=c99
# override CFLAGS += -pedantic
override CFLAGS += -pipe
override CFLAGS += -DHCNSE_POOL_THREAD_SAFETY

build: override CFLAGS += -g0 -O3 -s
debug: override CFLAGS += -g3 -O0 -ggdb3
debug: override CFLAGS += -DHCNSE_DEBUG -DHCNSE_HAVE_VALGRIND
test:  override CFLAGS += -DHCNSE_TEST

# override LDFLAGS += -lssl -lcrypto
# build: override LDFLAGS += -static -static-libgcc

CC = gcc

ifeq ($(CC), gcc)
	GCC_VER = $(shell $(CC) -v 2>&1 | grep 'gcc version' 2>&1 \
		| sed -e 's/^.* version \(.*\)/\1/')
	override CFLAGS += -DHCNSE_COMPILER="\"gcc $(GCC_VER)\""
	override LDFLAGS += -ldl
	override LDFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

ifeq ($(CC), clang)
	CLANG_VER = $(shell $(CC) -v 2>&1 | grep 'version' 2>&1 \
		| sed -n -e 's/^.*clang version \(.*\)/\1/p' \
		-e 's/^.*LLVM version \(.*\)/\1/p')
	override CFLAGS += -DHCNSE_COMPILER="\"clang $(CLANG_VER)\""
	override CFLAGS += -Qunused-arguments
	override LDFLAGS += -ldl
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
MAKEFLAGS += --no-print-directory

export CC
export SRC_FILES
export CFLAGS
export LDFLAGS

#.ONESHELL:
.PHONY: build debug modules test docs

all: build
build: mkdirs _build
debug: mkdirs _debug
test: mkdirs _test

_build: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

_debug: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

#	cd $(MODULE_DIR) && $(MAKE)
modules:
	@for dir in $(MODULE_DIRS); do \
		if [ -d $$dir ]; then \
			$(MAKE) -C $$dir build; \
		fi; \
	done;

_test: $(OBJ_FILES)
	cd test && $(MAKE)

docs:
	doxygen docs/Doxyfile

obj/%.o: %.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)

install: mkdirs build
	install bin/$(BIN_FILE) $(INSTALL_DIR)
	@if [ ! -d /etc/hcnse ]; then \
		mkdir /etc/hcnse; \
	fi
	cp -R sample/config /etc/hcnse

uninstall:
	rm -f $(INSTALL_DIR)/$(BIN_FILE)

clean:
	rm -f obj/* bin/*
# 	@for dir in $(MODULE_DIRS); do \
# 		if [ -d $$dir ]; then \
# 			$(MAKE) -C $$dir clean; \
# 		fi; \
# 	done;
# 	@$(MAKE) -C test clean

mkdirs:
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi

	@if [ ! -d obj ]; then \
		mkdir obj; \
	fi

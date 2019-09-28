BIN_FILE = server

SRC_FILES += main.c
SRC_FILES += daemon.c
SRC_FILES += error_proc.c
SRC_FILES += pools.c
SRC_FILES += config.c
SRC_FILES += listen.c
SRC_FILES += sockets.c
SRC_FILES += syslog.c

CFLAGS += -Iinclude -Wall -pipe
build: CFLAGS += -g0 -s -O3
debug: CFLAGS += -g3 -ggdb3 -O0 -DDEBUG

LDFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
LDFLAGS += -lpcre
build: LDFLAGS += -static -static-libgcc

OBJ_FILES := $(patsubst %.c,obj/%.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

VPATH := src/core src/os

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

mkdirs: 
	@if [ ! -d bin ] ; then \
		mkdir bin ; \
	fi

	@if [ ! -d obj ] ; then \
		mkdir obj ; \
	fi

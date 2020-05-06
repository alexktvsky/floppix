BIN_FILE = dashboard

SRC_FILES += main.cpp
SRC_FILES += 
SRC_FILES += 

VPATH += src

INSTALL_DIR = /usr/local/bin

override CFLAGS += -Iinclude
override CFLAGS += -Wall -Wextra -Wuninitialized -pipe
override CFLAGS += -fno-exceptions
build: override CFLAGS += -g0 -s -O3
debug: override CFLAGS += -g3 -ggdb3 -O0 -DDEBUG

override LDFLAGS +=
build: override LDFLAGS += -static

CXX = g++

ifeq ($(CXX), g++)
	override LDFLAGS +=
endif

ifeq ($(CXX), clang++)
	override CFLAGS += -Qunused-arguments
	override LDFLAGS +=
endif

ifeq ($(CXX), x86_64-w64-mingw32-g++)
	override CFLAGS +=
	override LDFLAGS += -lws2_32
endif

ifeq ($(CXX), i686-w64-mingw32-g++)
	override CFLAGS +=
	override LDFLAGS += -lws2_32
endif

OBJ_FILES := $(patsubst %.cpp,obj/%.o,$(SRC_FILES))
QUIET_CXX = @echo '   ' CXX $(notdir $@);

#.ONESHELL:
.PHONY: build debug test docs

all: build

build: mkdirs _build

debug: mkdirs _debug

_build: $(OBJ_FILES)
	$(CXX) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

_debug: $(OBJ_FILES)
	$(CXX) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

test:
	$(warning Tests now is not available!)

docs:
	doxygen docs/Doxyfile

obj/%.o: %.cpp
	$(QUIET_CXX) $(CXX) -c $< -o $@ $(CFLAGS)

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

BIN_FILE = client

SRC_FILES += main.cpp
SRC_FILES += 
SRC_FILES += 

CXX = g++

INSTALL_DIR = /usr/local/bin

CFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Wuninitialized -pipe
CFLAGS += -fno-exceptions
build: CFLAGS += -g0 -s -O3 -DNDEBUG
debug: CFLAGS += -g3 -ggdb3 -O0 -DDEBUG

LDFLAGS +=
build: LDFLAGS += -static

ifeq ($(CXX), g++)
	LDFLAGS +=
endif

ifeq ($(CXX), clang++)
	CFLAGS += -Qunused-arguments
	LDFLAGS +=
endif

ifeq ($(CXX), x86_64-w64-mingw32-g++)
	CFLAGS +=
	LDFLAGS += -lws2_32
endif

ifeq ($(CXX), i686-w64-mingw32-g++)
	CFLAGS +=
	LDFLAGS += -lws2_32
endif

OBJ_FILES := $(patsubst %.cpp,obj/%.o,$(SRC_FILES))
QUIET_CXX = @echo '   ' CXX $(notdir $@);

VPATH := src

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

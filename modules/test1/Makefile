BIN_FILE = module_test1.so
SRC_FILES += fpx_test1.c

CFLAGS += -DFPX_BUILDING_DSO
CFLAGS += -Iinclude -I../../../include
CFLAGS += -Wall -Wextra -Wuninitialized
CFLAGS += -fPIC
LDFLAGS += -shared

ifeq ($(CC), gcc)
	LD = ld
endif

ifeq ($(CC), clang)
	LD = ld
endif

ifeq ($(CC), x86_64-w64-mingw32-gcc)
	BIN_FILE = libtest1.dll
	LD = x86_64-w64-mingw32-ld
endif

ifeq ($(CC), i686-w64-mingw32-gcc)
	BIN_FILE = libtest1.dll
	LD = i686-w64-mingw32-ld
endif

OBJ_FILES := $(patsubst %.c,obj/%.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

.PHONE: build debug

all: mkdirs build

build: $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o bin/$(BIN_FILE) $(OBJ_FILES)

obj/%.o: src/%.c
	$(QUIET_CC)$(CC) -c $< -o $@ $(CFLAGS)

install: mkdirs build
	cp bin/$(BIN_FILE) /usr/bin
	/sbin/ldconf

uninstall:
	rm -f /usr/bin/$(BIN_FILE)
	/sbin/ldconf

clean:
	rm -f obj/* bin/*

mkdirs:
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi

	@if [ ! -d obj ]; then \
		mkdir obj; \
	fi

BIN_FILE = libtest1.so
SRC_FILES += hcnse_test1.c

CFLAGS += -DHCNSE_BUILDING_DSO
CFLAGS += -Iinclude -I../../../include
CFLAGS += -fPIC
LDFLAGS += -shared

ifeq ($(CC), gcc)
	LD = ld
endif

ifeq ($(CC), clang)
	LD = ld
endif

ifeq ($(CC), x86_64-w64-mingw32-gcc)
	BIN_FILE = libtest1.exe
	LD = x86_64-w64-mingw32-ld
endif

ifeq ($(CC), i686-w64-mingw32-gcc)
	BIN_FILE = libtest1.exe
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
	/sbin/ldconfig

uninstall:
	rm -f /usr/bin/$(BIN_FILE)
	/sbin/ldconfig

clean:
	rm -f obj/* bin/*

mkdirs: 
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi

	@if [ ! -d obj ]; then \
		mkdir obj; \
	fi

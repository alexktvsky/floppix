SRC_FILES += main.c
SRC_FILES += connect.c

CFLAGS += -Iinc -Wall -pipe
build: CFLAGS += -s -O3 -static -static-libgcc
debug: CFLAGS += -g -O0 -DDEBUG

# Define compilation variables for Linux
ifeq ($(CC),gcc)
	BIN_FILE = client_linux
	LDFLAGS += 
	OBJ_FILES := $(patsubst %.c,obj/%.elf.o,$(SRC_FILES))
obj/%.elf.o: src/%.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)
# Define compilation variables for Win32
else ifeq ($(CC),x86_64-w64-mingw32-gcc)
	BIN_FILE = client_win32.exe
	LDFLAGS += -lws2_32
	OBJ_FILES := $(patsubst %.c,obj/%.exe.o,$(SRC_FILES))
obj/%.exe.o: src/%.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)
endif

QUIET_CC = @echo '   ' CC $(notdir $@);

.ONESHELL:
.PHONY: build debug test

all: mkdirs build

build: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(CFLAGS) $(LDFLAGS)

debug: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(CFLAGS) $(LDFLAGS)

test:
	cd test && $(MAKE)

docs:
	doxygen docs/Doxyfile

install: mkdirs build
	install bin/$(BIN_FILE) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(BIN_FILE)

clean:
	rm -f obj/*.o

mkdirs: 
	@if [ ! -d bin ] ; then
		mkdir bin
	fi

	@if [ ! -d obj ] ; then
		mkdir obj
	fi

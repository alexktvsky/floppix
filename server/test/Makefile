BIN_FILE = test

SRC_FILES += test.c
SRC_FILES += hcnse_unit_test.c
SRC_FILES += test_hcnse_list.c

VPATH += ../src/core
VPATH += ../src/event
VPATH += ../src/os

override CFLAGS += -I../include
override LDFLAGS += -Wl,--wrap=main

OBJ_FILES := $(patsubst %.c,../obj/%.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

.PHONY: test

all: test
test: mkdirs _test

_test: $(OBJ_FILES)
	$(CC) $^ -o bin/$(BIN_FILE) $(LDFLAGS)

../obj/%.o: %.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f ../obj/* bin/*

mkdirs:
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi

	@if [ ! -d ../obj ]; then \
		mkdir ../obj; \
	fi

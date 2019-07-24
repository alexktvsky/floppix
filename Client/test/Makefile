BIN_FILE = tests
SRC_FILES += tests.c
SRC_FILES += connect_tests.c
CFLAGS += -I../inc -I../src -Wall -g -O0 #-ftest-coverage -pg -no-pie -fprofile-arcs
CFLAGS += -DTEST
LDFLAGS += -lcunit

OBJ_FILES := $(patsubst %.c,../obj/%.elf.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

.ONESHELL:
.PHONY: build


all: build run

build: $(OBJ_FILES)
	$(CC) $^ -o ../bin/$(BIN_FILE) $(CFLAGS) $(LDFLAGS) 

../obj/%.elf.o: %.c
	$(QUIET_CC)$(CC) -c $< -o $@ $(CFLAGS)

run: build
	./../bin/$(BIN_FILE) | tee $(LOG_DIR)/client.tests

mkdirs: 
	@if [ ! -d ../bin ] ; then
		mkdir ../bin
	fi

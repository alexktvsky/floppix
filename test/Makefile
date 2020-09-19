SRC_FILES += hcnse_test.c
SRC_FILES += hcnse_stubs.c
SRC_FILES += test_hcnse_pool.c

#
# Don't forget to wrap your stub by linker
#
override LDFLAGS += -Wl,--wrap=hcnse_malloc
override LDFLAGS += -Wl,--wrap=hcnse_palloc


BIN_FILE = test
override CFLAGS += -I../include
OBJ_FILES := $(patsubst %.c,../obj/%.o,$(SRC_FILES))
QUIET_CC = @echo '   ' CC $(notdir $@);

.PHONY: build_test

all: build_test
build_test: _build_test

_build_test: $(OBJ_FILES)
	$(CC) $^ -o ../bin/$(BIN_FILE) $(LDFLAGS)

../obj/%.o: %.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)

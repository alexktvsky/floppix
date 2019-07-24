LOG_DIR = ../../Logs
export LOG_DIR

PREFIX = /usr/local/bin
export PREFIX


all: build_all
build_all: build_client build_server
debug_all: debug_client debug_server

build_client:
	cd Client && $(MAKE) \
		CC=gcc
	cd Client && $(MAKE) \
		CC=x86_64-w64-mingw32-gcc

build_server:
	cd Server && $(MAKE) \
		CC=gcc

debug_client:
	cd Client && $(MAKE) debug \
		CC=gcc
	cd Client && $(MAKE) debug \
		CC=x86_64-w64-mingw32-gcc

debug_server:
	cd Server && $(MAKE) debug \
		CC=gcc

test_client:
	cd Client && $(MAKE) test \
		CC=gcc

test_server:
	cd Server && $(MAKE) test \
		CC=gcc

docs_client: 
	cd Client && $(MAKE) docs

docs_server: 
	cd Server && $(MAKE) docs

clean:
	cd Client && $(MAKE) clean
	cd Server && $(MAKE) clean

mkdirs: 
	@if [ ! -d Logs ] ; then \
		mkdir Logs ; \
	fi

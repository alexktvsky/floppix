all: build_all
build_all: build_client build_server
debug_all: debug_client debug_server

build_client:
	cd Client && $(MAKE)

build_server:
	cd Server && $(MAKE)

debug_client:
	cd Client && $(MAKE) debug

debug_server:
	cd Server && $(MAKE) debug

test_client:
	cd Client && $(MAKE) test

test_server:
	cd Server && $(MAKE) test

docs_client: 
	cd Client && $(MAKE) docs

docs_server: 
	cd Server && $(MAKE) docs

clean:
	cd Client && $(MAKE) clean
	cd Server && $(MAKE) clean

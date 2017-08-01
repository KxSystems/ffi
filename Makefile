ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
      FFI_INCLUDE=`pkg-config --cflags libffi`
			FFI_LIBS=`pkg-config --libs libffi`
			OPTS=-g3 -shared -fPIC -DKXVER=3 -D_GNU_SOURCE -ldl -Wall
			CC=gcc
    endif
    ifeq ($(UNAME_S),Darwin)
      FFI_INCLUDE=`pkg-config --cflags  /usr/local/Cellar/libffi/3.2.1/lib/pkgconfig/libffi.pc`
			FFI_LIBS=`pkg-config --libs /usr/local/Cellar/libffi/3.2.1/lib/pkgconfig/libffi.pc`
			OPTS=-g -glldb -shared  -undefined dynamic_lookup  -mmacosx-version-min=10.12 -DKXVER=3 -ldl -Wall
			CC=clang
    endif
endif


all:
	$(CC) ffi.c $(FFI_INCLUDE) $(FFI_LIBS) $(OPTS) -o ffi.so
all32:
	$(CC) ffi.c $(FFI_INCLUDE) $(FFI_LIBS) $(OPTS) -o ffi.so -m32 	
fmt:
	clang-format -style=file ffi.c -i
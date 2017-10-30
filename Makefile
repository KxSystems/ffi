UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  FFI_INCLUDE=`pkg-config --cflags libffi`
	FFI_LIBS=`pkg-config --libs libffi`
	OPTS=-g3 -shared -fPIC -D_GNU_SOURCE -ldl -Wall
	CC=gcc
	QP=l
endif
ifeq ($(UNAME_S),Darwin)
  FFI_INCLUDE=-I /usr/include/ffi
	FFI_LIBS=-lffi
	OPTS=-g -shared  -undefined dynamic_lookup  -mmacosx-version-min=10.12 -ldl -Wall
	CC=clang
	QP=m
endif


all64:
	mkdir -p $(QP)64
	$(CC) ffi.c $(FFI_INCLUDE) $(FFI_LIBS) $(OPTS) -o $(QP)64/ffi.so
all32:
	mkdir -p $(QP)32
	$(CC) ffi.c $(FFI_INCLUDE) $(FFI_LIBS) $(OPTS) -o $(QP)32/ffi.so -m32 
fmt:
	clang-format -style=file ffi.c -i
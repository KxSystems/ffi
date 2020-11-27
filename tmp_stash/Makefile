UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  FFI_INCLUDE=`pkg-config --cflags libffi`
	FFI_LIBS=`pkg-config --libs libffi`
	OPTS=-g3 -shared -fPIC -D_GNU_SOURCE -ldl -Wall -include _memcpy.h
	CC=gcc
	QP=l
endif
ifeq ($(UNAME_S),Darwin)
  FFI_INCLUDE=-I /usr/include/ffi
	FFI_LIBS=-lffi
	OPTS=-g -shared -DSYSFFI -undefined dynamic_lookup -mmacosx-version-min=10.12 -ldl -Wall # -arch i386 -arch x86_64
	CC=clang
	QP=m
endif

all: all32 all64

all64:
	mkdir -p $(QP)64
	$(CC) ffi.c $(FFI_INCLUDE) $(FFI_LIBS) $(OPTS) -o $(QP)64/ffi.so -m64
all32:
	mkdir -p $(QP)32
	$(CC) ffi.c $(FFI_INCLUDE) $(FFI_LIBS) $(OPTS) -o $(QP)32/ffi.so -m32 
fmt:
	clang-format -style=file ffi.c -i

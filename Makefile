FFI_INCLUDE=`pkg-config --cflags  /usr/local/Cellar/libffi/3.2.1/lib/pkgconfig/libffi.pc`
FFI_LIBS=`pkg-config --libs /usr/local/Cellar/libffi/3.2.1/lib/pkgconfig/libffi.pc`

all:
	gcc -g -glldb ffi.c $(FFI_INCLUDE) $(FFI_LIBS) -shared  -undefined dynamic_lookup  -mmacosx-version-min=10.12 -DKXVER=3 -ldl -o ffi.so
fmt:
	clang-format -style=file ffi.c -i
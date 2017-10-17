all:
	if not exist w64 mkdir w64
	cl /LD /MT /Iwin\x64-windows-static\include ffi.c win\w64\q.lib win\x64-windows-static\lib\libffi.lib win\x64-windows-static\lib\dl.lib	psapi.lib /Few64\ffi.dll
all32:
	if not exist w32 mkdir w32
  cl /LD /MT /Iwin\x86-windows-static\include ffi.c win\w32\q.lib win\x86-windows-static\lib\libffi.lib win\x86-windows-static\lib\dl.lib	psapi.lib /Few32\ffi.dll
alld:
	if not exist w64 mkdir w64
	cl /LD /MTd /Iwin\x64-windows-static\include ffi.c win\w64\q.lib win\x64-windows-static\debug\lib\libffi.lib win\x64-windows-static\debug\lib\dl.lib	psapi.lib /Few64\ffi.dll
all32d:
	if not exist w32 mkdir w32
  cl /LD /MTd /Iwin\x86-windows-static\include ffi.c win\w32\q.lib win\x86-windows-static\debug\lib\libffi.lib win\x86-windows-static\debug\lib\dl.lib	psapi.lib /Few32\ffi.dll

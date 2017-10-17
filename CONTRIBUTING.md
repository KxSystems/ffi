### Installation
#### Dependencies
On Ubuntu 
```
sudo apt-get install libffi-dev
sudo apt install libffi-dev:i386 # to install 32bit version on 64bit OS
```
On masOS
```
brew install libffi     // at the time of writing is libffi 3.2.1
```
Optional: To install MKL go ahead to https://software.intel.com/en-us/mkl
```
/opt/intel/mkl/bin/mklvars.sh intel64
```

On Windows

Windows dependencies are pre-packaged with repository as static libraries. 
You only need to download latest version of q bindings for windows(q.lib).
```
mkdir -p win/w32 win/w64
wget https://github.com/KxSystems/kdb/raw/master/w64/q.lib -O win/w64/q.lib
wget https://github.com/KxSystems/kdb/raw/master/w32/q.lib -O win/w32/q.lib
```

Prepackaged libraries were compiled and exported using `vcpkg` using following commands
```
.\vcpkg install libffi:x64-windows-static dlfcn-win32:x64-windows-static
.\vcpkg install libffi:x86-windows-static dlfcn-win32:x86-windows-static

.\vcpkg --raw export dlfcn-win32:x86-windows-static  dlfcn-win32:x64-windows-static libffi:x86-windows-static libffi:x64-windows-static

cp -rp vcpkg-export-<>/installed/* $FFIQBUILD/win/
```

Note: that it is possible to compile ffiq with libffi provided by nuget(and using `nuget restore`). Unfortunately, this will require additional modifications to `ffi.c` as nuget libffi doesn't export several critical constants. This is fixed in [vcpkg](https://github.com/Microsoft/vcpkg/blob/master/ports/libffi/export-global-data.patch). `dlfcn` is not available on nuget and will need additional stubs in `ffi.c`.

#### Library
On Linux and macOS

```
make all   # default OS combo(x64)
make all32 # compile as 32 bit libraries
```

Windows

Make sure you use correct VisualC++ compiler version. `x86` for w32 and `x64` for w64.
```
# to compile 32bit dll
nmake /fwin\Makefile w32 
# to compile 64bit dll
nmake /fwin\Makefile w64
```

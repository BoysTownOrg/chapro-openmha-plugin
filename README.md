# Building
## Clone repository and submodules
```
git clone --recursive https://github.com/boystownorg/chapro-openmha-plugin
```
## Generate buildsystem with cmake
```
cd chapro-openmha-plugin
mkdir build
cd build
cmake ..
```
Note - to specify a non-default installation directory use CMAKE_INSTALL_PREFIX (if, for instance, MHA_LIBRARY_PATH is not on the system library path)
```
cmake -DCMAKE_INSTALL_PREFIX=path/to/openmha ..
```
## Build
```
cmake --build . --target chapro-openmha-plugin
```
## Install
```
make install
```
# Running in openMHA
```
mha --interactive
?read:chapro.cfg
cmd=start
```
# Cross-compiling plugins for ARM
```
mkdir build-ARM
cd build-ARM
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-arm-linux-gnueabihf.cmake ..
cmake --build . --target chapro-openmha-plugin
cmake --build . --target chapro-afc-openmha-plugin
```

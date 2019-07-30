# Build
## Clone repositories and submodules
```
git clone --recurse-submodules -j8 https://github.com/boystownorg/chapro-openmha-plugin
```
or

```
git clone --recursive https://github.com/boystownorg/chapro-openmha-plugin
```
or

```
git clone https://github.com/boystownorg/chapro-openmha-plugin
cd chapro-openmha-plugin
git submodule update --init --recursive
```
## Generate buildsystem with cmake
For "out-of-source builds"
```
cd chapro-openmha-plugin
mkdir build
cd build
cmake ..
```
or "in-source builds"
```
cd chapro-openmha-plugin
cmake .
```
## Build
```
cmake --build . --target chapro-openmha-plugin
```
## Install
```
cmake --install chapro-openmha-plugin
```
or
```
make install
```

# Running in openMHA
```
mha --interactive
?read:chapro.cfg
cmd=start
```

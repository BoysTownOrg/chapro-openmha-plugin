# Build
```
git clone --recurse-submodules -j8 https://github.com/boystownorg/chapro-openmha-plugin
cd chapro-openmha-plugin
mkdir build
cd build
cmake ..
make
make install
```

# Running in openMHA
```
mha --interactive
?read:chapro.cfg
cmd=start
```

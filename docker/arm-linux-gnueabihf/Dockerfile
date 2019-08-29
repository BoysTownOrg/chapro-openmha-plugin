FROM ubuntu:latest
RUN apt-get update && apt-get install -y \
    make \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    wget
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

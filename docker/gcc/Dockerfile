FROM ubuntu:latest
RUN apt-get update && apt-get install -y \
    make \
    gcc \
    g++ \
    wget
RUN wget -qO- "https://cmake.org/files/v3.10/cmake-3.10.3-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

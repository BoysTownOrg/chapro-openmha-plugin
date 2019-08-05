FROM ubuntu:latest

RUN apt-get -y update && apt-get install -y
RUN apt-get -y install wget make gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

ENV CC=arm-linux-gnueabihf-gcc \
    CXX=arm-linux-gnueabihf-g++

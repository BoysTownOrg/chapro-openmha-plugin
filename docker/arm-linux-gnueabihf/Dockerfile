FROM ubuntu:latest
RUN apt-get -y update
RUN apt-get install -y software-properties-common
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys B7D6CDF547DA4ABD
RUN apt-add-repository 'deb http://apt.hoertech.de bionic universe'
RUN apt-get -y update
RUN apt-get install -y libopenmha-dev
RUN apt-get -y install git wget make gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local
ENV PATH="/usr/arm-linux-gnueabihf/bin:${PATH}"
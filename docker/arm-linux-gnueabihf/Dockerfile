FROM ubuntu:latest
RUN apt-get update
RUN apt-get install -y software-properties-common
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys B7D6CDF547DA4ABD
RUN apt-add-repository 'deb http://apt.hoertech.de bionic universe'
RUN apt-get update
RUN apt-get install -y \
    git \
    wget \
    make \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    libsndfile1-dev \
    libjack-jackd2-dev \
    portaudio19-dev \
    libopenmha-dev
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local
ENV PATH="/usr/arm-linux-gnueabihf/bin:${PATH}"
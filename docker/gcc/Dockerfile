FROM alpine:latest
RUN apk update && apk add \
    make \
    gcc \
    g++ \
    tar \
    wget \
    bash
RUN wget -qO- "https://cmake.org/files/v3.10/cmake-3.10.3-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

FROM gcc:7
RUN apt-get update && apt-get -y install sudo
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

RUN adduser --disabled-password --gecos '' docker
RUN adduser docker sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER docker
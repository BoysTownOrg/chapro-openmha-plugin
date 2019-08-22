FROM gcc:7
RUN apt-get -y update && apt-get install -y libsndfile1-dev libjack-jackd2-dev portaudio19-dev
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

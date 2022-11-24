FROM debian:bullseye-slim

ENV HTTPS_PROXY=socks5h://192.168.1.214:9995
RUN apt update && apt upgrade -y
RUN apt install -y wget cmake git lsb-release software-properties-common gnupg2
RUN wget https://apt.llvm.org/llvm.sh && chmod +x ./llvm.sh && ./llvm.sh 16
RUN apt install -y llvm-16-dev libclang-common-16-dev libclang-16-dev clang-16

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && mkdir build \
      && cd build \
      && cmake ../ -DLLVM_CONF=llvm-config-16 -DCMAKE_CXX_COMPILER=clang++-16 -DUSE_MONOLITH_LIBTOOLING=OFF\
      && make

RUN rm -rf /var/apt/cache

FROM debian:bullseye-slim

RUN apt update && apt upgrade -y
RUN apt install -y wget cmake git lsb-release software-properties-common gnupg2
RUN wget apt.llvm.org/llvm.sh && chmod +x ./llvm.sh && ./llvm.sh 13
RUN apt install -y llvm-13-dev libclang-common-13-dev libclang-13-dev clang-13

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && mkdir build \
      && cd build \
      && cmake ../ -DLLVM_CONF=llvm-config-13 -DCMAKE_CXX_COMPILER=clang++-13 -DUSE_MONOLITH_LIBTOOLING=OFF\
      && make

RUN rm -rf /var/apt/cache

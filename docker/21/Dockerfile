FROM debian:bullseye-slim

RUN apt update && apt upgrade -y
RUN apt install -y wget cmake git lsb-release software-properties-common gnupg2
RUN wget https://apt.llvm.org/llvm.sh && chmod +x ./llvm.sh && ./llvm.sh 21
RUN apt install -y llvm-21-dev libclang-common-21-dev libclang-21-dev clang-21 libclang-cpp21-dev

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && mkdir build \
      && cd build \
      && cmake ../ -DLLVM_CONF=llvm-config-21 -DCMAKE_CXX_COMPILER=clang++-21 -DUSE_MONOLITH_LIBTOOLING=ON\
      && make

RUN rm -rf /var/apt/cache

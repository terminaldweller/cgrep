FROM debian:bullseye-slim

RUN apt update && apt upgrade -y
RUN apt install -y wget cmake git lsb-release software-properties-common gnupg2
RUN wget https://apt.llvm.org/llvm.sh && chmod +x ./llvm.sh && ./llvm.sh 19
RUN apt install -y llvm-19-dev libclang-common-19-dev libclang-19-dev clang-19 libclang-cpp19-dev

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && mkdir build \
      && cd build \
      && cmake ../ -DLLVM_CONF=llvm-config-19 -DCMAKE_CXX_COMPILER=clang++-19 -DUSE_MONOLITH_LIBTOOLING=ON\
      && make

RUN rm -rf /var/apt/cache

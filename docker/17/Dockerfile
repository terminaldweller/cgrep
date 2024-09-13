FROM debian:bullseye-slim

RUN apt update && apt upgrade -y
RUN apt install -y wget cmake git lsb-release software-properties-common gpg
RUN wget https://apt.llvm.org/llvm.sh && chmod +x ./llvm.sh && ./llvm.sh 17
RUN apt install -y llvm-17-dev libclang-common-17-dev libclang-17-dev clang-17 libclang-cpp17-dev

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && mkdir build \
      && cd build \
      && cmake ../ -DLLVM_CONF=llvm-config-17 -DCMAKE_CXX_COMPILER=clang++-17 -DUSE_MONOLITH_LIBTOOLING=ON\
      && make

RUN rm -rf /var/apt/cache

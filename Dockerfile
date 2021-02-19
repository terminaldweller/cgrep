FROM debian:bullseye-slim

RUN apt update && apt upgrade -y
RUN apt install -y clang-11 llvm-11-dev libclang-common-11-dev libclang-11-dev libllvm11
RUN apt install -y git make libstdc++6 -y

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && make CXX=clang-11 LLVM_CONF=llvm-config-11

RUN rm -rf /var/apt/cache

FROM archlinux:base-devel-20210725.0.29770

RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm wget cmake git gnupg llvm-libs llvm openmp clang

RUN git clone https://github.com/bloodstalker/cgrep \
      && cd cgrep \
      && git submodule init \
      && git submodule update \
      && mkdir build \
      && cd build \
      && cmake ../ -DLLVM_CONF=llvm-config -DCMAKE_CXX_COMPILER=clang++ -DUSE_MONOLITH_LIBTOOLING=ON\
      && make

RUN pacman -Scc

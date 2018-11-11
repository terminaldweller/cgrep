# cgrep
`grep` for C/C++ source files.<br/>
cgrep uses libtooling, the latest tested version of LLVM is trunk 340121.<br/>

#Bulding
assuming you have the llvm/clang libraries, just run:<br/>
```bash
git clone https://github.com/bloodstalker/cgrep
git submodule init
git submodule update
make
```

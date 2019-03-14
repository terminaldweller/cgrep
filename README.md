[![Build Status](https://travis-ci.org/bloodstalker/cgrep.svg?branch=master)](https://travis-ci.org/bloodstalker/cgrep)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/bloodstalker/cgrep.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/bloodstalker/cgrep/context:cpp)

# cgrep
`grep` for C/C++ source files.<br/>
cgrep uses libtooling, the latest tested version of LLVM is trunk 340121.<br/>
Should be more or less fine for other C-family languages as well but I haven't tested for those since I don't use those. Let me know if you run into trouble using cgrep on those.<br/>

## Bulding
Assuming you have the llvm/clang libraries(the build file will read your llvm options using `llvm-config` so make sure it's in path), just run:<br/>
```bash
git clone https://github.com/bloodstalker/cgrep
git submodule init
git submodule update
make
```
`cgrep` supports LLVM 6,8, and for 9 the latest tested trunk version currently is:355787.<br/>
The makefile assumes clang is called `clang` and llvm-config is called `llvm-config`. On some distros, the names might not be the same. In those cases use `CXX` and `LLVM_CONF` to pass the values to the makefile.<br/>

## Usage
A simple usage example:<br/>
```bash
cgrep -A 1 -B 1 --func --var --regex n[aA]m ./cgrep.cpp
```
Pleade do note that the regex will pass through both C++ and the regex engine, so if you would want to escape `\`, the regex you pass as the commandline arg would be `\\\\` instead of the normal `\\`.<br/>
In order for cgrep to work, you need to have a compilation database, tools like cmake can generate one for you.<br/>
If your build tool doesn't do that, you can just use [bear](https://github.com/rizsotto/Bear).<br/>
You can also skip the compilation database alltogether passing cgrep `--` after the input file name.<br/>

## Options
Here's an option list though it might not be necessarily up-to-date.<br/>
For an up-to-date list, you can run `crep --help`.<br/>

```bash
  -A=<int>                   - same as grep, how many lines after the matched line to print
  -B=<int>                   - same as grep, howm many lines before the matched line to print
  -all                       - turns on all switches other than nameddecl
  -awk                       - outputs location in a gawk freidnly format
  -call                      - match function calls only
  -class                     - match class declrations only
  -cxxcall                   - match member function calls only
  -declrefexpr               - matches declrefexpr
  -dir=<string>              - recursively goes through all the files and directories. assumes compilation databases are present for all source files.
  -func                      - match functions only
  -header                    - match headers in header inclusions
  -macro                     - match macro definitions
  -mainfile                  - mathc identifiers in the main file only
  -memfunc                   - match member functions only
  -memvar                    - match member variables only
  -nameddecl                 - matches all named declrations
  -regex=<string>            - the regex to match against
  -struct                    - match structures only
  -syshdr                    - match identifiers in system header as well
  -union                     - match unions only
  -var                       - match variables only
```
`cgrep` is a clang tool, so it will accecpt all valid clang commandline options.<br/>

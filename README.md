[![Build Status](https://travis-ci.org/bloodstalker/cgrep.svg?branch=master)](https://travis-ci.org/bloodstalker/cgrep)
[![Build status](https://ci.appveyor.com/api/projects/status/caab8oxmgljb87te?svg=true)](https://ci.appveyor.com/project/bloodstalker/cgrep)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/bloodstalker/cgrep.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/bloodstalker/cgrep/context:cpp)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep?ref=badge_shield)
<a href="https://scan.coverity.com/projects/bloodstalker-cgrep">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/19431/badge.svg"/>
</a>

# cgrep
`grep` for C/C++ source files.<br/>
Should be more or less fine for other C-family languages as well but I haven't tested for those since I don't use those. Let me know if you run into trouble using cgrep on those.<br/>
The goal is to make the options and features as similar to `grep` as possible for ease of use.<br/>

## Building
Assuming you have the llvm/clang libraries(the build file will read your llvm options using `llvm-config` so make sure it's in path), just run:<br/>
```bash
git clone https://github.com/bloodstalker/cgrep
cd cgrep
git submodule init
git submodule update
make
```
If you have installed LLVM but don't have `llvm-config`, you are missing the dev package for LLVM.<br/>
`cgrep` supports LLVM 5,6,7,8 and 9. For 10. the latest tested trunk version is:374971.<br/>
The makefile assumes clang is called `clang` and llvm-config is called `llvm-config`. On some distros, the names might not be the same. In those cases use `CXX` and `LLVM_CONF` to pass the values to the makefile like so:<br/>
```bash
make CXX=clang-9.0 LLVM_CONF=llvm-config-9.0
```
For windows builds, cygwin builds are supported. get llvm and clang along with their sources and build like usual.<br/>

## Usage
A simple usage example:<br/>
```bash
cgrep -A 1 -B 1 --func --var --regex n[aA]m ./cgrep.cpp
```
Pleade do note that the regex will pass through both C++ and the regex engine, so if you would want to escape `\`, the regex you pass as the commandline arg would be `\\\\` instead of the normal `\\`.<br/>
In order for cgrep to work, you need to have a compilation database, tools like `cmake` can generate one for you.<br/>
If your build tool doesn't do that, you can just use [bear](https://github.com/rizsotto/Bear) or [scan-build](https://github.com/rizsotto/scan-build).<br/>
You can also skip the compilation database alltogether passing cgrep `--` after the input file name which means you have chosen not to pass it anything.<br/>
You can pass the options by hand since cgrep is a Clang instance so it recognizes every option clang has.<br/>

cgrep uses ANSI escape sequences for colors so your terminal should support those.<br/>

## Options
Here's an option list though it might not be necessarily up-to-date.<br/>
For an up-to-date list, you can run `crep --help`.<br/>

```bash
  -A=<int>                    - same as grep, how many lines after the matched line to print
  -B=<int>                    - same as grep, howm many lines before the matched line to print
  --all                       - turns on all switches other than nameddecl
  --awk                       - outputs location in a gawk freidnly format
  --call                      - match function calls only
  --class                     - match class declrations only
  --cxxcall                   - match member function calls only
  --declrefexpr               - matches declrefexpr
  --dir=<string>              - recursively goes through all the files and directories. assumes compilation databases are present for all source files.
  --extra-arg=<string>        - Additional argument to append to the compiler command line
  --extra-arg-before=<string> - Additional argument to prepend to the compiler command line
  --func                      - match functions only
  --header                    - match headers in header inclusions
  --macro                     - match macro definitions
  --mainfile                  - match identifiers in the main file only
  --memfunc                   - match member functions only
  --memvar                    - match member variables only
  --nameddecl                 - matches all named declrations
  -p=<string>                 - Build path
  --regex=<string>            - the regex to match against
  --struct                    - match structures only
  --syshdr                    - match identifiers in system header as well
  --union                     - match unions only
  --var                       - match variables only
```
`cgrep` is a clang tool, so it will accecpt all valid clang commandline options.<br/>

## Known Issues
* bulding cgrep with `-j` will not work because bad makefile.<br/>
* the coloring is off right now and doesn't work properly.<br/>
=======

## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep?ref=badge_large)
=======

[![Build Status](https://travis-ci.org/bloodstalker/cgrep.svg?branch=master)](https://travis-ci.org/bloodstalker/cgrep)
[![Build status](https://ci.appveyor.com/api/projects/status/caab8oxmgljb87te?svg=true)](https://ci.appveyor.com/project/bloodstalker/cgrep)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/bloodstalker/cgrep.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/bloodstalker/cgrep/context:cpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/726c70d0e3294f149036fa6134998fe8)](https://www.codacy.com/manual/bloodstalker/cgrep?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bloodstalker/cgrep&amp;utm_campaign=Badge_Grade)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep?ref=badge_shield)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/19431/badge.svg)](https://scan.coverity.com/projects/bloodstalker-cgrep)
[![Coverage Status](https://coveralls.io/repos/github/bloodstalker/cgrep/badge.svg?branch=master)](https://coveralls.io/github/bloodstalker/cgrep?branch=master)
[![Gitter](https://badges.gitter.im/mutatortools/community.svg)](https://gitter.im/mutatortools/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

# cgrep

`grep` for C/C++ source files.
Should be more or less fine for other C-family languages as well but I haven't tested for those since I don't use those. Let me know if you run into trouble using cgrep on those.
The goal is to make the options and features as similar to `grep` as possible for ease of use.

## Building

Assuming you have the llvm/clang libraries (the build file will read your llvm options using `llvm-config` so make sure it's in path), just run:

```bash
git clone https://github.com/bloodstalker/cgrep
cd cgrep
git submodule init
git submodule update
make
```

If you have installed LLVM but don't have `llvm-config`, you are missing the dev package for LLVM.
`cgrep` supports LLVM 5,6,7,8 and 9. For 10 the latest tested trunk version is: 374971.
The makefile assumes clang is called `clang` and llvm-config is called `llvm-config`. On some distros, the names might not be the same. In those cases use `CXX` and `LLVM_CONF` to pass the values to the makefile like so:

```bash
make CXX=clang-9.0 LLVM_CONF=llvm-config-9.0
```

For windows builds, cygwin builds are supported. Get llvm and clang along with their sources and build like usual. If you run into problems while bulding on cygwin, you can take a look at the `appveyor.yml` file under the repository root.

## Usage

A simple usage example:

```bash
cgrep -A 1 -B 1 --func --declrefexpr --regex n[aA]m --noclor --nodecl ./cgrep.cpp
```

Please do note that the regex will pass through both C++ and the regex engine, so if you would want to escape `\`, the regex you pass as the command line arg would be `\\\\` instead of the normal `\\`.
In order for cgrep to work, you need to have a compilation database, tools like `cmake` can generate one for you.
If your build tool doesn't do that, you can just use [bear](https://github.com/rizsotto/Bear) or [scan-build](https://github.com/rizsotto/scan-build).
You can also skip the compilation database altogether passing cgrep `--` after the input file name which means you have chosen not to pass it anything.
You can pass the options by hand since cgrep is a Clang instance so it recognizes every option clang has.

cgrep uses ANSI escape sequences for colors so your terminal should support those.

## Options

Here's an option list, though it might not be necessarily up-to-date.
For an up-to-date list, you can run `cgrep --help`.

```bash
  -A=<int>                    - Same as grep, how many lines after the matched line to print. Defaults to 0.
  -B=<int>                    - Same as grep, howm many lines before the matched line to print. Defaults to 0.
  --all                       - Turns on all switches other than nameddecl.
  --awk                       - Outputs location in a gawk freidnly format, not meant for human consumption. Defaults to false.
  --call                      - Match function calls.
  --class                     - Match class declrations.
  --cxxcall                   - Match member function calls.
  --declrefexpr               - Matches declrefexpr.
  --dir=<string>              - recursively goes through all the files and directories. assumes compilation databases are present for all source files.
  --extra-arg=<string>        - Additional argument to append to the compiler command line
  --extra-arg-before=<string> - Additional argument to prepend to the compiler command line
  --func                      - Match functions.
  --header                    - Match headers in header inclusions.
  --macro                     - Match macro definitions.
  --mainfile                  - Match identifiers in the main file only. Defaults to true.
  --memfunc                   - Match member functions.
  --memvar                    - Match member variables.
  --nameddecl                 - Matches all named declrations.
  --nocolor                   - For terminals that don't supprt ANSI escape sequences. The default is to false.
  --nodecl                    - For switches that are not declarations, don't print declarations. Defaults to false.
  -p=<string>                 - Build path
  --regex=<string>            - The regex to match against.
  --struct                    - Match structures.
  --syshdr                    - Match identifiers in system header as well. Defaults to true.
  --union                     - Match unions.
  --var                       - Match variables.
```

`cgrep` is a clang tool, so it will accept all valid clang command line options.

## Known Issues

## License

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep?ref=badge_large)

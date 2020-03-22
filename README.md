[![Build Status](https://travis-ci.org/bloodstalker/cgrep.svg?branch=master)](https://travis-ci.org/bloodstalker/cgrep)
[![Build status](https://ci.appveyor.com/api/projects/status/caab8oxmgljb87te?svg=true)](https://ci.appveyor.com/project/bloodstalker/cgrep)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/bloodstalker/cgrep.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/bloodstalker/cgrep/context:cpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/726c70d0e3294f149036fa6134998fe8)](https://www.codacy.com/manual/bloodstalker/cgrep?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bloodstalker/cgrep&amp;utm_campaign=Badge_Grade)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep?ref=badge_shield)
[![Gitter](https://badges.gitter.im/mutatortools/community.svg)](https://gitter.im/mutatortools/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

# cgrep

`grep` for C-family source files.

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
cgrep -A 1 -B 1 --func --declrefexpr --regex n[aA]m --nocolor --nodecl ./myawesomecode.cpp
```

In order for cgrep to work, you need to have a compilation database, tools like `cmake` can generate one for you.<br/>
You can, by all means, run cgrep without a compilation databse but whether that works or not really depends on your source file. Can you build your source file with clang without passing it any options?
If the answer to that is yes, then you can just run cgrep without a compilation databse like so:<br/>
```bash
cgrep -A 1 -B 1 --func --declrefexpr --regex n[aA]m --nocolor --nodecl ./myawesomecode.cpp --
```
Otherwise you need a compilation database.<br/>

Please do note that the regex will pass through both C++ and the regex engine, so if you would want to escape `\`, the regex you pass as the command line arg would be `\\\\` instead of the normal `\\`.<br/>
If your build tool doesn't do that, you can just use [bear](https://github.com/rizsotto/Bear) or [scan-build](https://github.com/rizsotto/scan-build).
You can also skip the compilation database altogether passing cgrep `--` after the input file name which means you have chosen not to pass it anything.
You can pass the options by hand since cgrep is a Clang instance so it recognizes every option clang has.

cgrep uses ANSI escape sequences for colors so your terminal should support those. In case your terminal does not support ANSI escape sequences, you can silence those using the `--nocolor` option.

By default, cgrep will print out the declaration location for a match. In case you don't want those in the output, you can pass cgrep the `--nodecl` switch.

You can use `--extra-arg==--std=` to tell cgrep which C-family language the source file is supposed to be in.

## Options

Here's an option list, though it might not be necessarily up-to-date.
For an up-to-date list, you can run `cgrep --help` or look at the man page.

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
  --syshdr                    - Match identifiers in system header as well. Defaults to false.
  --union                     - Match unions.
  --var                       - Match variables.
```

`cgrep` is a clang tool, so it will accept all valid clang command line options.

## Example
You can run the coverage report, `make covrep`, to see some examples. Before running make sure to generate the compilation databases for `cgrep.cpp` and `./test/main.cpp`. The paths need to be right for your system. Please note that even though cgrep's source file is small, the actual code itself isn't. cgrep is clang. So cgrep will be relatively slow in the examples since its running on a Very big codebase.

## Known Issues
`cgrep` will not print out warnings or errors related to the source code so please make sure that clang can successfully build your file before running cgrep on it.

## License

cgrep is licensed under GPL-3.0.


[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fbloodstalker%2Fcgrep?ref=badge_large)

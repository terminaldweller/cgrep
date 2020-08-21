#!/usr/bin/env python3
# _*_ coding=utf-8 _*_

import argparse
import subprocess
import os
import sys
import re

test_files = ["/home/bloodstalker/extra/cgrep/cgrep.cpp"]

test_files_2 = [
    "./test/function.cpp",
    "./test/fielddecl.cpp",
    "./test/cxxmethoddecl.cpp",
    "./test/vardecl.cpp",
    "./test/classdecl.cpp",
    "./test/structdecl.cpp",
    "./test/uniondecdef.cpp",
    "./test/nameddecldef.cpp",
    "./test/declrefexpr.cpp",
    "./test/callexpr.cpp",
    "./test/cxxmembercallexpr.cpp",
]

cgrep_test_args = [
    "-A 1 -B 1 --func --var --regex n[aA]m",
    "-A 1 -B 1 --func --var --awk --regex n[aA]m",
    "-A 1 -B 1 --func --declrefexpr --regex n[aA]m --nocolor",
    "-A 1 -B 1 --func --declrefexpr --memfunc --call --cxxcall --var --regex run",
    "-A 1 -B 1 --macro --header --regex n[aA]m",
    "-A 1 -B 1 --class --regex and",
    "-A 1 -B 1 --struct --union --regex n[aA]m",
    "-A 1 -B 1 --nameddecl --regex n[aA]m",
    "-A 1 -B 1 --cxxcall --call --regex add",
    "-A 1 -B 1 --cfield --regex ite",
    "--union --regex [Uu]nion ./test/main.cpp",
    "--struct --regex [sS]truct ./test/main.cpp",
    "--dir ./ --regex run --func", ]

cgrep_test_args_2 = [
    "--func --regex test",
    "--cxxfield --regex test",
    "--memfunc --regex test",
    "--var --regex test",
    "--class --regex test",
    "--struct --regex test",
    "--union --regex test",
    "--nameddecl --regex test",
    "--declrefexpr --regex test",
    "--call --regex test",
    "--cxxcall --regex test",
]


class Argparser(object):
    def __init__(self):
        parser = argparse.ArgumentParser()
        parser.add_argument("--string", type=str, help="string")
        parser.add_argument("--capturereference", type=str,
                            help="capture a new reference for the test outputs using the current outputs")
        parser.add_argument("--compare", action="store_true",
                            help="compare the current test results with the reference", default=False)
        self.args = parser.parse_args()


def call_from_shell(command, *command_args):
    command_list = [arg for arg in command_args]
    command_list.insert(0, command)
    if sys.version_info < (3, 7):
        return subprocess.run(command_list, stdout=subprocess.PIPE)
    else:
        return subprocess.run(command_list, capture_output=True)


def call_from_shell_list(command_list):
    if sys.version_info < (3, 7):
        return subprocess.run(command_list, stdout=subprocess.PIPE)
    else:
        return subprocess.run(command_list, capture_output=True)


def call_from_shell_pprint(command, *command_args):
    print(call_from_shell(command, *command_args).stdout.decode("utf-8"))


def call_from_shell_list_pprint(command_list):
    print(call_from_shell_list(command_list).stdout.decode("utf-8"))


def main():
    # argparser = Argparser()
    cgrep_exe = "cgrep"
    os.chdir("../")
    # get LLVM libdir
    llvm_libdir = call_from_shell_list(["llvm-config", "--libdir"])
    # get LLVM version. upstream builds can have extra unwanted text attached.
    llvm_version = re.findall("[0-9]*\.[0-9]*\.[0-9]*", call_from_shell_list(
        ["llvm-config", "--version"]).stdout.decode("utf-8"))
    # buld the magic sause. we dont wanna get stddef.h not found.
    clang_builtin_headers = "--extra-arg=-I" + \
        llvm_libdir.stdout.decode(
            "utf-8")[:-1] + "/clang/" + llvm_version[0] + "/include"
    for cgrep_test_arg, cgrep_test_files in zip(cgrep_test_args_2, test_files_2):
        arg_list = cgrep_test_arg.split()
        arg_list.insert(0, cgrep_exe)
        arg_list.insert(1, clang_builtin_headers)
        arg_list.append(cgrep_test_files)
        print(arg_list)
        ret = call_from_shell_list(arg_list)
        print("ret:", ret.stdout.decode("utf-8"), end="")
        print("ret:", ret.stderr.decode("utf-8"), end="")


if __name__ == "__main__":
    main()

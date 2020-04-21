#!/usr/bin/python3
# _*_ coding=utf-8 _*_

import argparse
import subprocess
import os
import sys

class Argparser(object):
    def __init__(self):
        parser = argparse.ArgumentParser()
        parser.add_argument("--string", type=str, help="string")
        parser.add_argument("--bool", action="store_true", help="bool", default=False)
        self.args = parser.parse_args()

def main():
    argparser = Argparser()

if __name__ == "__main__":
    main()

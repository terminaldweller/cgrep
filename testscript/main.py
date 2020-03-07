#!/usr/bin/python3
# _*_ coding=utf-8 _*_

import argparse

class Argparser(object):
    def __init__(self):
        parser = argparse.ArgumentParser()
        parser.add_argument("--string", type=str, help="string")
        parser.add_argument("--bool", action="store_true", help="bool", default=False)
        parser.add_argument("--dbg", action="store_true", help="debug", default=False)
        self.args = parser.parse_args()

def main():
    argparser = Argparser()

if __name__ == "__main__":
    main()

#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Takes as input a directory containing the CRM files provided by Perú and
generates as output a file with a document(conversation) per line
"""
import sys
import argparse
import codecs
from os import listdir
from os.path import isfile, join


def generate(inputpath, outputfile):
    ofile = open(outputfile, 'w+')
    onlyfiles = [filename for filename in listdir(inputpath) if isfile(join(inputpath, filename))]
    for f in onlyfiles:
        with codecs.open(join(inputpath, f), encoding='iso-8859-1', mode='rb') as ifile:
            textout = f
            for line in ifile:
                word, col2, col3, col4, col5 = line.split('\t')
                textout += ' ' + word

            if textout != '':
                ofile.write(textout.strip() + '\n')


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("ipath", help="input directory")
    parser.add_argument("ofile", help="CSV output file")

    args = parser.parse_args()

    generate(args.ipath, args.ofile)

if __name__ == "__main__":
    main(sys.argv[1:])

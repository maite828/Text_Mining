#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Generates the input file expected by Tableau
"""
import sys
import argparse
import csv
import time

HEADER = ["source", "host", "link", "time", "auth", "age", "gender", "country", \
          "location", "sentiment", "title", "snippet", "contents", "uniqueid", "authname"]
INPUT_TIME_FORMAT = '%a %b %d %H:%M:%S +0000 %Y'
OUTPUT_TIME_FORMAT = '%Y-%m-%dT%H:%M:%SZ'


def buildLink(user_id, tweet_id):
    return 'http://twitter.com/' + user_id + '/statuses/' + tweet_id


def buildTime(created_at):
    t = time.strptime(created_at, INPUT_TIME_FORMAT)
    return time.strftime(OUTPUT_TIME_FORMAT, t)


def generate(inputfile, outputfile, isep, iquo, osep, oquo):
    inputcsv = open(inputfile, 'rb')
    outputcsv = open(outputfile, 'wb')
    reader = csv.reader(inputcsv, delimiter=isep, quotechar=iquo)
    writer = csv.writer(outputcsv, delimiter=osep, quotechar=oquo, quoting=csv.QUOTE_ALL)
    writer.writerow(HEADER)
    try:
        for row in reader:
            orow = []
            orow.append('TWITTER')
            orow.append('twitter.com')
            orow.append(buildLink(row[0].strip(), row[2].strip()))
            orow.append(buildTime(row[1].strip()))
            orow.append('')
            orow.append('')
            orow.append('')
            orow.append('')
            orow.append('')
            orow.append('')
            orow.append('')
            orow.append('')
            orow.append(row[3].strip())
            orow.append(row[2].strip())
            orow.append('')

            writer.writerow(orow)
    except Exception as e:
        print e.message
        print 'Exception:' + ''.join(row)


def main(argv):
    isep = '|'
    iquo = None
    osep = ','
    oquo = '"'
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", help="CSV input file")
    parser.add_argument("--isep", help="CSV input file separator, if not provided then ',' is used as default")
    parser.add_argument("--iquo", help="CSV input file quotechar, if not provided then no quoterchar is used as default")
    parser.add_argument("ofile", help="CSV output file")

    args = parser.parse_args()
    if args.isep:
        isep = args.isep
    if args.iquo:
        iquo = args.iquo

    generate(args.ifile, args.ofile, isep, iquo, osep, oquo)

if __name__ == "__main__":
    main(sys.argv[1:])

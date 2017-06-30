__author__ = 'zoraida'
import codecs
import json
import argparse
import sys
import re

DEFAULT_ENCODING = 'UTF-8'
OUTPUT_SEPARATOR = '|'

def getFromDict(dataDict, mapList):
    return reduce(lambda d, k: d[k], mapList, dataDict)

def generate(inputfile, outputfile, fields):
    """
    """
    tweets = {}
    ofile = codecs.open(outputfile, 'w', DEFAULT_ENCODING)
    fields_list = fields.split(',')
    with codecs.open(inputfile, encoding=DEFAULT_ENCODING, mode='rb') as ifile:
        for line in ifile:
            csv_line = []
            try:
                tweet = json.loads(line)
                for field in fields_list:
                    field_value = getFromDict(tweet, field.split('.'))
                    if isinstance(field_value, ( int, long )):
                        csv_line.append(str(field_value))
                    elif isinstance(field_value, dict):
                        csv_line.append(unicode(json.dumps(field_value), DEFAULT_ENCODING))
                    else:
                        csv_line.append(re.sub('[\t\n\r\f\v]',u' ', field_value))
                ofile.write(OUTPUT_SEPARATOR.join(csv_line) + u'\n')

            except ValueError as e:
                print e.message
                print line
            except TypeError as e:
                print e.message
                print line

    ofile.close()


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", help="CSV input file")
    parser.add_argument("ofile", help="CSV output file")
    parser.add_argument("fields", help="Fields to output to CSV")

    args = parser.parse_args()

    generate(args.ifile, args.ofile, args.fields)


if __name__ == "__main__":
    main(sys.argv[1:])

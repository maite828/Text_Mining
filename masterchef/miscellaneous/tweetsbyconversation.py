#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
This script groups tweets by conversation. A conversation is defined as a directed graph of tweets built throught
the field 'in_reply_to_status_id_str'. For any tweet different that the root, the field 'in_reply_to_status_id_str'
is not empty. There is only a Root has the field in_reply_to_status_id_str' empty. Cycles can exist.

Input: json file with tweets
Output: json file where:
    'text': string that contains the collection of all 'text' fields order by 'created_at' from old to new.
    'id_str': string that root tweet id
    'ids': list of strings that contains the ids of the tweets order by 'created_at' from old to new.
"""
import codecs
import json
import argparse
import sys

DEFAULT_ENCODING = 'UTF-8'


def generate(inputfile, outputfile, minTweets):
    """
    Tweets must be listed by date
    """
    tweets = {}

    with codecs.open(inputfile, encoding=DEFAULT_ENCODING, mode='rb') as ifile:
        for line in ifile:
            aux = line.encode('ascii', 'ignore')
            #aux = aux.decode('utf-8')
            try:
                tweet = json.loads(aux)
                id_str = tweet['id_str']
                tweets[id_str] = tweet
            except ValueError as e:
                print e.message
                print line
            except TypeError as e:
                print e.message
                print line

    for tweetId, tweet in tweets.iteritems():
        if 'ids' not in tweet:
            tweet['ids'] = set([])
        tweet['ids'] = tweet['ids'].union(set([tweetId]))

        tweetIdAux = tweetId
        # while current tweet is a reply to other tweet, keep grouping tweets
        while 'in_reply_to_status_id_str' in tweets[tweetIdAux] and tweets[tweetIdAux]['in_reply_to_status_id_str']:
            # if the replied tweet exist into tweets:
            if tweets[tweetIdAux]['in_reply_to_status_id_str'] in tweets:
                if 'ids' not in tweets[tweets[tweetIdAux]['in_reply_to_status_id_str']]:
                    tweets[tweets[tweetIdAux]['in_reply_to_status_id_str']]['ids'] = set([])
                tweets[tweets[tweetIdAux]['in_reply_to_status_id_str']]['ids'] = tweets[tweets[tweetIdAux]['in_reply_to_status_id_str']]['ids'] | tweets[tweetIdAux]['ids']
                tweetIdAux = tweets[tweetIdAux]['in_reply_to_status_id_str']
            else:
                # current tweet becomes a root tweet
                tweets[tweetIdAux]['in_reply_to_status_id_str'] = None

    ofile = codecs.open(outputfile, 'w', DEFAULT_ENCODING)
    for tweet in tweets.itervalues():
        # For those tweets root (those that do not have in_reply_to_status_id_str)
        if 'in_reply_to_status_id_str' not in tweet or not tweet['in_reply_to_status_id_str']:
            if len(tweet['ids']) >= int(minTweets):
                tweet['ids'] = sorted(list(tweet['ids']))
                doc = ''
                print tweet['ids']
                for tweetReplyId in tweet['ids']:
                    try:
                        doc += tweets[tweetReplyId]['text_out'] + '. '
                    except KeyError as e:
                        print e.message
                        print tweetReplyId
                tweet['doc'] = doc
                ofile.write(json.dumps(tweet, ensure_ascii=False) + u'\n')

    ofile.close()


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", help="CSV input file")
    parser.add_argument("ofile", help="CSV output file")
    parser.add_argument("minTweets", help="Minimum number of tweets by conversation")

    args = parser.parse_args()

    generate(args.ifile, args.ofile, args.minTweets)


if __name__ == "__main__":
    main(sys.argv[1:])

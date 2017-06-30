#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
This script groups tweets by conversation. A conversation is defined as a directed graph of tweets built throught
the field 'in_reply_to_status_id'. For any tweet different that the root, the field 'in_reply_to_status_id'
is not empty. There is only a Root has the field in_reply_to_status_id' empty. Cycles can exist.

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


def generate(inputfile, outputfile, minTweets):
    """
    Tweets must be listed by date
    """
    tweets = {}

    with codecs.open(inputfile, encoding='utf-8', mode='rb') as ifile:
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
                
#    tweetsfile = [{'id_str':'5000', 'in_reply_to_status_id':'3000', 'text_out':'Text 5'},
#                  {'id_str':'4000', 'in_reply_to_status_id':'3000', 'text_out':'Text 4'},
#                  {'id_str':'3000', 'in_reply_to_status_id':'2000', 'text_out':'Text 3'},
#                  {'id_str':'2000', 'in_reply_to_status_id':'', 'text_out':'Text 2'},
#                  {'id_str':'1000', 'in_reply_to_status_id':'', 'text_out':'Text 1'}]
#    for tweet in tweetsfile:
#        tweets[tweet['id_str']] = tweet

    print 'Tweets done'

    for tweetId, tweet in tweets.iteritems():
        if 'ids' not in tweet:
            tweet['ids'] = set([])
        tweet['ids'] = tweet['ids'].union(set([tweetId]))

        tweetIdAux = tweetId
        # while current tweet is a reply to other tweet, keep grouping tweets
        while 'in_reply_to_status_id' in tweets[tweetIdAux] and tweets[tweetIdAux]['in_reply_to_status_id']:
            # if the replied tweet exist into tweets:
            if str(tweets[tweetIdAux]['in_reply_to_status_id']) in tweets:
                if 'ids' not in tweets[str(tweets[tweetIdAux]['in_reply_to_status_id'])]:
                    tweets[str(tweets[tweetIdAux]['in_reply_to_status_id'])]['ids'] = set([])
                tweets[str(tweets[tweetIdAux]['in_reply_to_status_id'])]['ids'] = tweets[str(tweets[tweetIdAux]['in_reply_to_status_id'])]['ids'] | tweets[tweetIdAux]['ids']
                tweetIdAux = str(tweets[tweetIdAux]['in_reply_to_status_id'])
            else:
                # current tweet becomes a root tweet
                tweets[tweetIdAux]['in_reply_to_status_id'] = None

    print 'Conversations grouped'
    print len(tweets)
    ofile = codecs.open(outputfile, 'w', 'utf-8')
    for tweet in tweets.itervalues():
        # For those tweets root (those that do not have in_reply_to_status_id)
        if 'in_reply_to_status_id' not in tweet or not tweet['in_reply_to_status_id']:
            if len(tweet['ids']) >= int(minTweets):
                print 'Root: ' + tweet['id_str'] + ' #replies: ' + str(len(tweet['ids']))
                tweet['ids'] = sorted(list(tweet['ids']))
                doc = ''
                for tweetReplyId in tweet['ids']:
                    try:
                        doc += tweets[tweetReplyId]['text'] + '. '
                    except KeyError as e:
                        print e.message
                        print tweetReplyId
                tweet['doc'] = doc
                ofile.write(json.dumps(tweet, ensure_ascii=False) + u'\n')

  #      print 'tweets in conversation: ' + str(tweet['ids'])
  #      print 'tweets id: ' + tweet['id_str']
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

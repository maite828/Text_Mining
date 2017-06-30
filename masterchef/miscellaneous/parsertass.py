#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Given an xml file: http://www.daedalus.es/TASS2013/data/general-tweets-train-tagged.xml,
for each tweet, the original status is retrieved and an output file is generated merging
the status information along with the Sentimental Analisis and Topic Detection tagging from
TASS
"""
import twitter
import argparse
import xml.etree.ElementTree as etree
import sys
import codecs
import json
import time
import urllib2

#Setting up Twitter API
api = twitter.Api(
 consumer_key='ZYcGyJBbyJBiLr8sgHWdQ',
 consumer_secret='j6NDl5AsqXNZITN1EGMvWYWioBVJmlNiH1DxH1akGIw',
 access_token_key='14225969-tNrFG8l3XIjf2gLiqUnQ0YMjWeth2xBn3RMQaA1xU',
 access_token_secret='HA0NCdrsa6Z8vLhWm2ml051dserM6YAg9ORsMalJ7Q'
 )

RATE_LIMIT_EXCEEDED = 88
DEFAULT_ENCODING = 'UTF-8'


def getStatusFromId(tweetId):
    """
    Gets status from id
    """
    statusDict = None
    try:
        status = api.GetStatus(tweetId)
        statusDict = status.AsDict()

    except twitter.TwitterError as e:
        print  'Tweet id [%s] %s' % (tweetId, str(e))
        if e.message[0]['code'] == RATE_LIMIT_EXCEEDED:
            time.sleep(900)
        else:
            statusDict = {}

    except urllib2.HTTPError as e:
        print  'Tweet id [%s] %s' % (tweetId, str(e))
        time.sleep(1)

    except urllib2.URLError as e:
        print  'Tweet id [%s] %s' % (tweetId, str(e))
        time.sleep(1)

    return statusDict


def getTassInfo(tassTweetDict, tassTweetXml):
    """
        Param 'tweet' is a dict representing an object of type Twitter.Status
        Param 'tasstweet' is a node of the xml representing the same status(tweet)
    """
    tassTweetDict['topics'] = []
    tassTweetDict['sentiments'] = []
    sentiments = tassTweetXml.find('{http://www.daedalus.es/TASS/tweets}sentiments')
    for polarity in sentiments.iter('{http://www.daedalus.es/TASS/tweets}polarity'):
        polarityDict = {}
        if polarity.find('{http://www.daedalus.es/TASS/tweets}entity') is not None:
            polarityDict['entity'] = polarity.find('{http://www.daedalus.es/TASS/tweets}entity').text
        polarityDict['value'] = polarity.find('{http://www.daedalus.es/TASS/tweets}value').text
        polarityDict['type'] = polarity.find('{http://www.daedalus.es/TASS/tweets}type').text
        tassTweetDict['sentiments'].append(polarityDict)

    topics = tassTweetXml.find('{http://www.daedalus.es/TASS/tweets}topics')
    for topic in topics.iter('{http://www.daedalus.es/TASS/tweets}topic'):
        tassTweetDict['topics'].append(topic.text)


def generate(inputfile, outputfile, lastTweetId):
    tweets = []
    parser = etree.XMLParser(encoding="utf-8")
    doc = etree.parse(inputfile, parser)
    # tag 'tweets'
    root = doc.getroot()

    # for each tag 'tweet'
    for tassTweetXml in root:
        tassTweetDict = {}
        tweetId = tassTweetXml.find('{http://www.daedalus.es/TASS/tweets}tweetid').text
        if tweetId > lastTweetId:
            tassTweetDict['id_str'] = tweetId
            getTassInfo(tassTweetDict, tassTweetXml)
            tweets.append(tassTweetDict)

    pendingTweets = len(tweets)

    while pendingTweets > 0:
        print pendingTweets
        for tassTweetDict in sorted(tweets, key=lambda k: k['id_str']):
            if not 'processed' in tassTweetDict:
                statusDict = getStatusFromId(tassTweetDict['id_str'])
                if statusDict is not None:
                    tassTweetDict.update(statusDict)
                    tassTweetDict['processed'] = True
                    pendingTweets -= 1

    ofile = codecs.open(outputfile, 'w', DEFAULT_ENCODING)
    for tweetDict in tweets:
        if 'text' in tweetDict:
            ofile.write(json.dumps(tweetDict, ensure_ascii=False) + u'\n')
    ofile.close()


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", help="CSV input file")
    parser.add_argument("ofile", help="CSV output file")
    parser.add_argument("tweetId", help="Last tweet id")

    args = parser.parse_args()

    generate(args.ifile, args.ofile, args.tweetId)

if __name__ == "__main__":
    main(sys.argv[1:])

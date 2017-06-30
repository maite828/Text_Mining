#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Util functions to process tweet specifics
"""
import re
from coo2cfg import oficial_users, text_field_out, hashtag_regex, mention_regex


def replaceMentions(tweet):
    """
    Replaces mentions by 'MENTION' + originalmention_value
    """
    if 'entities' in tweet:
        mentions = tweet['entities']['user_mentions']
        for mention in mentions:
            tweet[text_field_out] = tweet[text_field_out].replace(u'@' + mention['screen_name'].lower(), u'MENTION' + mention['screen_name'].lower())
    else:
        tweet[text_field_out] = re.sub(mention_regex, '', tweet[text_field_out])


def replaceHashtags(tweet):
    """
    Replaces hashtags by 'HASHTAG' + originalhashtag_value
    """
    if 'entities' in tweet:
        hashtags = tweet['entities']['hashtags']
        for hashtag in hashtags:
            tweet[text_field_out] = tweet[text_field_out].replace(u'#' + hashtag['text'].lower(), u'HASHTAG' + hashtag['text'].lower())
    else:
        tweet[text_field_out] = re.sub(hashtag_regex, '', tweet[text_field_out])


def excludeTweetsFromOficialUsers(tweet):
    """
    Discard tweets from oficial users.
    Tweets from oficial users that have been retweeted
    by no oficial users are taken into account
    """
    if tweet['user']['screen_name'].lower() in oficial_users:
        tweet[text_field_out] = u''


def excludeRetweets(tweet):
    """
    If the tweet is retweeted then it is discarted
    """
    if 'retweeted_status' in tweet:
        tweet[text_field_out] = u''


def processRetweets(tweet):
    """
    If the tweet is retweeted then the original text
    is the one to be processed
    """
    if 'retweeted_status' in tweet:
            tweet[text_field_out] = tweet['retweeted_status']['text'].lower()

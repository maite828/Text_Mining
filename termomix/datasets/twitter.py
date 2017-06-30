#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

""""
Dataset representation for tweets.
(legacy) Task 1: Sentiment Analysis at global level

This task consists on performing an automatic sentiment analysis to determine the global polarity (using 5 levels) of
each message in the test set of the General corpus (see below). This task is a reedition of the task in the previous
years.
(legacy) Task 2: Topic classification

The challenge is to build a classifier to automatically identify the topic of each message in the test set of the
General corpus. Again, a reedition of the same task in previous years.

Results for all tasks must be submitted in a plain text file with the following format:

id \t output

where:

    id is the tweet ID for Tasks 1, 2 and 3, and the combination of tweet ID and aspect for Task 4
    (such as 142378325086715906-play).
    output refers to the expected output of each task (polarity values, topic or aspect)

Regarding the polarity values, there are 6 valid tags (P+, P, NEU, N, N+ and NONE). Although the polarity level must be
classified into those levels and results will be evaluated for the 5 of them, the evaluation will include metrics that
consider just 3 levels (POSITIVE, NEUTRAL and NEGATIVE).

Regarding Task 2, a given tweet ID can be repeated in different lines if it is assigned more than one topic.
"""
import json
import codecs

from datasets.base_dataset import BaseDataset

from transformations.feature_extractors import extract_vectors

from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfVectorizer

import numpy as np

INPUT_ENCODING = 'UTF-8'
TEST_SIZE = 0.33
RANDOM_STATE = 42

# regex to substitute
REGEX_REPLACE = [('[\t\n\r\f\v]',u' '),('https://[^\s]+',u' '),('http://[^\s]+',u' ')]


def _replace_mentions(tweet):
    """
    Replaces mentions by 'MENTION' + originalmention_value
    """
    if 'entities' in tweet:
        mentions = tweet['entities']['user_mentions']
        for mention in mentions:
            tweet['text'] = tweet['text'].replace(u'@' + mention['screen_name'].lower(), \
                                                  u'Mention_' + mention['screen_name'].lower())


def _replace_hashtags(tweet):
    """
        Replaces hashtags by 'HASHTAG' + originalhashtag_value
    """
    if 'entities' in tweet:
        hashtags = tweet['entities']['hashtags']
        for hashtag in hashtags:
            tweet['text'] = tweet['text'].replace(u'#' + hashtag['text'].lower(), u'Hashtag_' + hashtag['text'].lower())


def _count_emoticon(tweet, simbol):
    """
    Counts the number of times the simbol appears in a tweet
    """
    return tweet['text'].count(simbol)


class TwitterDataset(BaseDataset):

    # List of extra features(part from ngrams). (feature_name, function_to_build_it, array_of_args)
    # tweet json instance is always passed to the function that initializes an extra feature
    EXTRA_FEATURES = [('happy', _count_emoticon,':)'),('laugh', _count_emoticon,':D'),
                ('sad', _count_emoticon,':('),('wink', _count_emoticon,';)'),
                ('happy_nose', _count_emoticon,':-)'),('tongue', _count_emoticon, ':P')]


    def __init__(self, train_file):
        BaseDataset.__init__(self, train_file)
        # original documents for test
        self.train_documents = []
        # vectorizers to apply
        self.vectorizer = TfidfVectorizer(min_df=1, ngram_range=(1,1))
        # score functions for feature_selector

        # for extra features
        self.extra_features_names = ['0'] * len(self.EXTRA_FEATURES)
        # for extra features. We do not know in advance the number of instances
        self.extra_features_values = None

        self._load()

    def _load(self):
        """
        Data files for this dataset are json.
        Loads dataset files. If test file is provided, then test_documents is filled with it.
        """
        y_train = []
        extra_features_values = []

        with codecs.open(self.train_file, encoding=INPUT_ENCODING, mode='rb') as ifile:
            for i, line in enumerate(ifile):
                tweet = json.loads(line)
                # Applies all text transformations (replace mentions, urls, hashtags, lemmatizer, break lines ...)
                TwitterDataset._text_transformation(tweet)
                self.train_documents.append(tweet['text'])
                # target value for this tweet
                y_train.append(tweet['sentiments'][0]['value'])
                extra_features_values.append([0] * len(self.EXTRA_FEATURES))
                # generate additonal features for this tweet
                for j, extra_feature in enumerate(self.EXTRA_FEATURES):
                    self.extra_features_names[j] = extra_feature[0]
                    extra_features_values[i][j] = extra_feature[1](tweet, extra_feature[2])

            # self.y_train must be a numpy array and we do not know in advance the number of instances
            self.y_train = np.array(y_train)
            self.extra_features_values = np.array(extra_features_values)
            X_tmp = extract_vectors(self.vectorizer, self.train_documents)
            self.X_train = np.concatenate((X_tmp.toarray(), self.extra_features_values),axis=1)
            self.X_names = self.vectorizer.get_feature_names() + self.extra_features_names

    @staticmethod
    def _text_transformation(tweet):
        """
        This function applies all required transformations to the text of the tweet.
        Lemmatizer will be called from here.
        :param tweet: json with tweet
        """
        _replace_mentions(tweet)
        _replace_hashtags(tweet)




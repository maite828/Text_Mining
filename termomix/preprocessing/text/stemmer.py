#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014
"""
This class wrapps NLKT Stemmer and implements
__call__ function to be called from Vectorizer Scipy classes:

    Usage: vect = CountVectorizer(tokenizer=StemTokenizer())
"""
from nltk import word_tokenize
from nltk.stem import SnowballStemmer


class StemTokenizer(object):

    def __init__(self, language):
        self.stemmer = SnowballStemmer(language)

    def __call__(self, doc):
        return [self.stemmer.stem(t) for t in word_tokenize(doc)]
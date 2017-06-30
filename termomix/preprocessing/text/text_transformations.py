#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014
""""
Functions for text transformations
"""
import re
import string

from nltk import word_tokenize


def get_words(document, lowercase=False, remove_punctuation=True):
    if remove_punctuation:
        remove_punctuation_map = dict((ord(char), None) for char in string.punctuation)
        document = document.translate(remove_punctuation_map)

    if lowercase:
        document = document.lower()

    return word_tokenize(document)


def strip_stopwords(document, stopwords):
    """
    Strips stopwords
    """
    return ' '.join([w for w in document.split(' ') if not w in stopwords])


def replace_regex(document, delete):
    """
    Replace words from list of tuplas 'delete'
    """
    for elem in delete:
        document = re.sub(elem[0], elem[1], document)
    return document


def replace_numbers(document, replace_str=""):
    """
    Replace each number occurrence in a document by a given string.
    To remove numbers in text use the StopWords class
    :param document: the original text document
    :param replace_str: string to substitute the numbers (figures and/or text) in the document
    :return the document with numbers replaced by replace_str
    """
    document = replace_regex(document, [[r"[0-9]", replace_str]])
    return document

#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: fmrm
# Created: 06/08/2014

from nltk.corpus import WordListCorpusReader
from nltk import data
import os
import logging
import timeit

from preprocessing.text import text_transformations

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s %(levelname)s %(message)s')

BASE_SW_PATH = os.path.dirname(os.path.realpath(__file__)) + "/conf/stopwords/"
DEFAULT_SW_FILE = BASE_SW_PATH + "stopwords.zip"


class StopWords():
    def __init__(self, language, sw_files=[], load_default=True):
        self.language = language
        self.stopwords = []

        if load_default:
            wlcr = WordListCorpusReader(data.GzipFileSystemPathPointer(DEFAULT_SW_FILE), [language], encoding="utf-8")
            self.stopwords = wlcr.words(language)
            logging.info("Loaded default stopwords from file %s" % DEFAULT_SW_FILE)

        path = BASE_SW_PATH + language
        for sw_file in sw_files:
            wlcr = WordListCorpusReader(data.FileSystemPathPointer(path), sw_file, encoding="utf-8")
            self.stopwords += wlcr.words(sw_file)
            logging.info("Loaded stopwords from file '%s'" % sw_file)

    def get_stopwords(self):
        return self.stopwords

    def remove_sw_from_text(self, doc, as_string=True, lowercase=False, remove_punctuation=True):
        """
        :param doc: The text to clean of stop words
        :param as_string: If output must be a string or a list
        :param lowercase: If resulting text/words must be lowercased
        :param remove_punctuation: If punctuation must be removed from the text/list
        :return: The text without stop words
        """
        words = text_transformations.get_words(doc, lowercase, remove_punctuation)
        return self.remove_sw_from_list(words, as_string)

    def remove_sw_from_list(self, words, as_string=False):
        """
        :param words: The list of words to clean of stop words
        :param as_string: If output must be a string or a list
        :return: The list without stop words
        """
        logging.debug("Removing stop words [#words=%d, #stopwords=%d]" % (len(words), len(self.stopwords)))

        result = [w for w in words if w.lower() not in self.stopwords]

        logging.debug("Done [final #words=%d]" % len(result))

        if as_string:
            result = ' '.join(result)

        return result

    def remove_sw_from_dtm(self, dtm, terms):
        """
        :param dtm: Document Term Matrix of CSC-SparseMatrix type (for optimal performance)
        :param terms: ndarray of DTM terms (DTM matrix columns)
        :return A sparse DTM without stopwords columns
        :return DTM columns names (terms)
        """
        logging.debug("Removing stop words from DTM [#docs=%d, #words=%d, #stopwords=%d]" %
                      (dtm.shape[0], dtm.shape[1], len(self.stopwords)))

        start_time = timeit.default_timer()
        no_sw_index = [index for index, term in enumerate(terms) if term not in self.stopwords]
        dtm = dtm[:, no_sw_index]
        terms = terms[no_sw_index]
        elapsed = timeit.default_timer() - start_time

        logging.debug("Done [#final_docs=%d, #final_words=%d]. Elapsed=%f" % (dtm.shape[0], dtm.shape[1], elapsed))

        return dtm, terms


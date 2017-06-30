#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'fmrm'

import unittest
from preprocessing.text.stopwords import StopWords
import numpy as np
from sklearn.feature_extraction.text import CountVectorizer


class StopWordsTests(unittest.TestCase):

    def test_remove_default_sw_from_text_to_text(self):
        text = u"This, is number one test"
        sw = StopWords("english")
        result = sw.remove_sw_from_text(text)
        self.assertEqual(u"number one test", result)

    def test_remove_default_sw_from_text_to_list(self):
        text = u"This, is number one test"
        sw = StopWords("english")
        result = sw.remove_sw_from_text(text, as_string=False)
        self.assertEqual(["number", "one", "test"], result)

    def test_remove_default_sw_from_list_to_text(self):
        words = ["This", "is", "number", "one", "test"]
        sw = StopWords("english")
        result = sw.remove_sw_from_list(words, as_string=True)
        self.assertEqual(u"number one test", result)

    def test_remove_default_sw_from_list_to_list(self):
        words = ["This", "is", "number", "one", "test"]
        sw = StopWords("english")
        result = sw.remove_sw_from_list(words)
        self.assertEqual(["number", "one", "test"], result)

    def test_remove_additional_sw(self):
        text = u"Esta, es la Prueba número veinte"
        sw = StopWords("spanish", sw_files=["numbers"])
        result = sw.remove_sw_from_text(text)
        self.assertEqual(u"Prueba número", result)

    def test_remove_not_default_sw(self):
        text = u"Esta, es la Prueba número veinte"
        sw = StopWords("spanish", sw_files=["numbers"], load_default=False)
        result = sw.remove_sw_from_text(text, lowercase=True, remove_punctuation=False)
        self.assertEqual(u"esta , es la prueba número", result)

    def test_remove_sw_from_dtm(self):
        texts = ["Esto, es un texto",
                 "Este texto también: es otro texto",
                 "Este texto contiene la letra EÑE"]
        sw = StopWords("spanish")
        vec = CountVectorizer()
        dtm = vec.fit_transform(texts)  # This returns csc sparse matrix
        col_names = np.array(vec.get_feature_names())

        # Previous check
        self.assertEqual(len(col_names), 11)
        self.assertEqual(dtm.toarray().shape, (3, 11))
        self.assertEqual(np.sum(dtm.toarray()), 16)

        # Stopwords (columns) removed: es, este, esto, la, otro, también, un
        result_dtm, result_col_names = sw.remove_sw_from_dtm(dtm, col_names)

        self.assertEqual(len(result_col_names), 4)
        self.assertEqual(result_dtm.toarray().shape, (3, 4))
        self.assertEqual(np.sum(result_dtm.toarray()), 7)


  #!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'fmrm'

import unittest
from operator import itemgetter
from preprocessing import dtm

import numpy as np
from sklearn.feature_extraction.text import CountVectorizer
from scipy.sparse.csc import csc_matrix


TEXTS = ["Esto, es un texto",
         "Este texto: es otro texto",
         "Este texto contiene la letra EÑE"]
vec = CountVectorizer()
term_count = [(u'contiene', 1), (u'es', 2), (u'este', 2), (u'esto', 1), (u'eñe', 1),
              (u'la', 1), (u'letra', 1), (u'otro', 1), (u'texto', 4), (u'un', 1)]


class DtmPreprocessingTests(unittest.TestCase):

    def test_get_term_count_unsorted(self):
        data = vec.fit_transform(TEXTS)
        terms = vec.get_feature_names()
        result = dtm.get_term_freq(data, terms)
        expected = term_count
        self.assertItemsEqual(expected, result)

    def test_get_term_count_sorted_on_term(self):
        data = vec.fit_transform(TEXTS)
        terms = vec.get_feature_names()
        result = dtm.get_term_freq(data, terms, sort=dtm.SORT_ON_TERM)
        expected = sorted(term_count, key=itemgetter(0), reverse=False)
        self.assertEquals(expected, result)

    def test_get_term_count_sorted_on_term_reverse(self):
        data = vec.fit_transform(TEXTS)
        terms = vec.get_feature_names()
        result = dtm.get_term_freq(data, terms, sort=dtm.SORT_ON_TERM, reverse_sort=True)
        expected = sorted(term_count, key=itemgetter(0), reverse=True)
        self.assertEquals(expected, result)

    def test_get_term_count_sorted_on_frequency(self):
        data = vec.fit_transform(TEXTS)
        terms = vec.get_feature_names()
        result = dtm.get_term_freq(data, terms, sort=dtm.SORT_ON_FREQUENCY)
        expected = sorted(term_count, key=itemgetter(1), reverse=False)
        self.assertEquals(expected, result)

    def test_get_term_count_sorted_on_frequency_reverse(self):
        data = vec.fit_transform(TEXTS)
        terms = vec.get_feature_names()
        result = dtm.get_term_freq(data, terms, sort=dtm.SORT_ON_FREQUENCY, reverse_sort=True)
        expected = sorted(term_count, key=itemgetter(1), reverse=True)
        self.assertEquals(expected, result)

    def test_get_dtm_frequency_diff(self):
        texts_2 = ["Más texto con la letra eñe",
                   "Este texto también contiene la eñe"]

        dtm_1 = vec.fit_transform(TEXTS)
        terms_1 = np.array(vec.get_feature_names())
        dtm_2 = vec.fit_transform(texts_2)
        terms_2 = np.array(vec.get_feature_names())

        result = dtm.subtract_dtm_frequencies(dtm_1, terms_1, dtm_2, terms_2)
        expected = csc_matrix([0, 2, 1, 1, -1, -1, 0, 1, 2, 1])
        self.assertTrue(dtm.equal(expected, result))

    def test_get_term_frequency_diff(self):
        terms_1 = np.array(["term1", "term2", "term3", "term4", "term5"])
        freq_1 = np.array([1, 2, 3, 4, 5])
        terms_2 = np.array(["term3", "term3", "term7", "term1"])
        freq_2 = np.array([1, 2, 3, 4])
        expected = np.array([-3, 2, 2, 4, 5])

        result = dtm.subtract_term_frequencies(terms_1, freq_1, terms_2, freq_2)
        self.assertTrue(np.array_equal(expected, result))

    def test_set_frequency_as_percentage(self):
        data = vec.fit_transform(TEXTS)
        expected = np.array([[1/15., 2/15., 2/15., 1/15., 1/15., 1/15., 1/15., 1/15., 4/15., 1/15.]])

        result = dtm.set_frequency_as_percentage(data)
        self.assertTrue(np.array_equal(expected, result))

    def test_equal_same_shape(self):
        dtm1 = csc_matrix([[1, 2], [3, 4]])
        dtm2 = csc_matrix([[1, 2], [3, 4]])
        self.assertTrue(dtm.equal(dtm1, dtm2))

    def test_not_equal_different_shape(self):
        dtm1 = csc_matrix([[1, 2], [3, 4]])
        dtm2 = csc_matrix([1, 2, 3, 4])
        self.assertFalse(dtm.equal(dtm1, dtm2))

    def test_not_equal_same_shape(self):
        dtm1 = csc_matrix([[1, 2], [3, 4]])
        dtm2 = csc_matrix([[1, 1], [1, 1]])
        self.assertFalse(dtm.equal(dtm1, dtm2))


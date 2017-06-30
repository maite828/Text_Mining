#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'fmrm'

import unittest
import numpy as np

from sklearn.feature_extraction.text import CountVectorizer

from processing.text.term_statistics import get_term_docs_stats, get_terms_docs_stats

DOCS = [u"This is a text",
        u"This text: is another text",
        u"This is a document",
        u"The last document can't be other thing but a text"]


class TermStatisticsProcessingTests(unittest.TestCase):

    def test_get_term_statistics(self):
        term = u"text"
        result = get_term_docs_stats(term, DOCS)
        self.assertEqual(4, result['num_appearances'])
        self.assertEqual(3, result['num_docs_with_term'])
        self.assertEqual(3. / 4., result['pct_docs_with_term'])
        self.assertEqual(1, result['term_in_doc_median'])
        self.assertEqual((1 + 2 + 1) / 3., result['term_in_doc_mean'])
        self.assertEqual(np.std([1, 2, 1]), result['term_in_doc_std'])
        self.assertEqual(4, result['pos_first_appearance_abs_median'])
        self.assertEqual((4 + 2 + 10) / 3., result['pos_first_appearance_abs_mean'])
        self.assertEqual(np.std([4, 2, 10]), result['pos_first_appearance_abs_std'])
        self.assertEqual(1, result['pos_first_appearance_pct_median'])
        self.assertEqual((4/4. + 2/5. + 10/10.) / 3., result['pos_first_appearance_pct_mean'])
        self.assertEqual(np.std([4/4., 2/5., 10/10.]), result['pos_first_appearance_pct_std'])

    def test_get_terms_statistics(self):
        terms = [u"text", u"this"]
        dict_result = get_terms_docs_stats(terms, DOCS)

        self.assertEqual(2, len(dict_result))

        term_result = dict_result[u"text"]
        self.assertEqual(4, term_result['num_appearances'])
        self.assertEqual(3, term_result['num_docs_with_term'])
        self.assertEqual(3. / 4., term_result['pct_docs_with_term'])
        self.assertEqual(1, term_result['term_in_doc_median'])
        self.assertEqual((1 + 2 + 1) / 3., term_result['term_in_doc_mean'])
        self.assertEqual(np.std([1, 2, 1]), term_result['term_in_doc_std'])
        self.assertEqual(4, term_result['pos_first_appearance_abs_median'])
        self.assertEqual((4 + 2 + 10) / 3., term_result['pos_first_appearance_abs_mean'])
        self.assertEqual(np.std([4, 2, 10]), term_result['pos_first_appearance_abs_std'])
        self.assertEqual(1, term_result['pos_first_appearance_pct_median'])
        self.assertEqual((4/4. + 2/5. + 10/10.) / 3., term_result['pos_first_appearance_pct_mean'])
        self.assertEqual(np.std([4/4., 2/5., 10/10.]), term_result['pos_first_appearance_pct_std'])

        term_result = dict_result[u"this"]
        self.assertEqual(3, term_result['num_appearances'])
        self.assertEqual(3, term_result['num_docs_with_term'])
        self.assertEqual(3. / 4., term_result['pct_docs_with_term'])
        self.assertEqual(1, term_result['term_in_doc_median'])
        self.assertEqual((1 + 1 + 1) / 3., term_result['term_in_doc_mean'])
        self.assertEqual(np.std([1, 1, 1]), term_result['term_in_doc_std'])
        self.assertEqual(1, term_result['pos_first_appearance_abs_median'])
        self.assertEqual((1 + 1 + 1) / 3., term_result['pos_first_appearance_abs_mean'])
        self.assertEqual(np.std([1, 1, 1]), term_result['pos_first_appearance_abs_std'])
        self.assertEqual(1/4., term_result['pos_first_appearance_pct_median'])
        self.assertEqual((1/4. + 1/5. + 1/4.) / 3., term_result['pos_first_appearance_pct_mean'])
        self.assertEqual(np.std([1/4., 1/5., 1/4.]), term_result['pos_first_appearance_pct_std'])

    def test_get_term_statistics_with_dtm(self):
        term = u"text"
        vec = CountVectorizer()
        freq_dtm = vec.fit_transform(DOCS)
        feature_names = np.array(vec.get_feature_names())

        result = get_term_docs_stats(term, DOCS, freq_dtm=freq_dtm, feature_names=feature_names)

        self.assertEqual(4, result['num_appearances'])
        self.assertEqual(3, result['num_docs_with_term'])
        self.assertEqual(3. / 4., result['pct_docs_with_term'])
        self.assertEqual(1, result['term_in_doc_median'])
        self.assertEqual((1 + 2 + 1) / 3., result['term_in_doc_mean'])
        self.assertEqual(np.std([1, 2, 1]), result['term_in_doc_std'])
        self.assertEqual(4, result['pos_first_appearance_abs_median'])
        self.assertEqual((4 + 2 + 10) / 3., result['pos_first_appearance_abs_mean'])
        self.assertEqual(np.std([4, 2, 10]), result['pos_first_appearance_abs_std'])
        self.assertEqual(1, result['pos_first_appearance_pct_median'])
        self.assertEqual((4/4. + 2/5. + 10/10.) / 3., result['pos_first_appearance_pct_mean'])
        self.assertEqual(np.std([4/4., 2/5., 10/10.]), result['pos_first_appearance_pct_std'])

    def test_get_terms_statistics_with_dtm(self):
        terms = [u"text", u"this"]
        vec = CountVectorizer()
        freq_dtm = vec.fit_transform(DOCS)
        feature_names = np.array(vec.get_feature_names())

        dict_result = get_terms_docs_stats(terms, DOCS, freq_dtm=freq_dtm, feature_names=feature_names)

        self.assertEqual(2, len(dict_result))

        term_result = dict_result[u"text"]
        self.assertEqual(4, term_result['num_appearances'])
        self.assertEqual(3, term_result['num_docs_with_term'])
        self.assertEqual(3. / 4., term_result['pct_docs_with_term'])
        self.assertEqual(1, term_result['term_in_doc_median'])
        self.assertEqual((1 + 2 + 1) / 3., term_result['term_in_doc_mean'])
        self.assertEqual(np.std([1, 2, 1]), term_result['term_in_doc_std'])
        self.assertEqual(4, term_result['pos_first_appearance_abs_median'])
        self.assertEqual((4 + 2 + 10) / 3., term_result['pos_first_appearance_abs_mean'])
        self.assertEqual(np.std([4, 2, 10]), term_result['pos_first_appearance_abs_std'])
        self.assertEqual(1, term_result['pos_first_appearance_pct_median'])
        self.assertEqual((4/4. + 2/5. + 10/10.) / 3., term_result['pos_first_appearance_pct_mean'])
        self.assertEqual(np.std([4/4., 2/5., 10/10.]), term_result['pos_first_appearance_pct_std'])

        term_result = dict_result[u"this"]
        self.assertEqual(3, term_result['num_appearances'])
        self.assertEqual(3, term_result['num_docs_with_term'])
        self.assertEqual(3. / 4., term_result['pct_docs_with_term'])
        self.assertEqual(1, term_result['term_in_doc_median'])
        self.assertEqual((1 + 1 + 1) / 3., term_result['term_in_doc_mean'])
        self.assertEqual(np.std([1, 1, 1]), term_result['term_in_doc_std'])
        self.assertEqual(1, term_result['pos_first_appearance_abs_median'])
        self.assertEqual((1 + 1 + 1) / 3., term_result['pos_first_appearance_abs_mean'])
        self.assertEqual(np.std([1, 1, 1]), term_result['pos_first_appearance_abs_std'])
        self.assertEqual(1/4., term_result['pos_first_appearance_pct_median'])
        self.assertEqual((1/4. + 1/5. + 1/4.) / 3., term_result['pos_first_appearance_pct_mean'])
        self.assertEqual(np.std([1/4., 1/5., 1/4.]), term_result['pos_first_appearance_pct_std'])

    def test_excel(self):
        import visualizations.excel as xls
        xls.create_docterm_statistics_file("C:/Temp/test.xlsx", ["this", "text"], DOCS)

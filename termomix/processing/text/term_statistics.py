#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: fmrm
"""
"""
import logging
import numpy as np

from preprocessing.text.text_transformations import get_words
from sklearn.feature_extraction.text import CountVectorizer

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')


def get_terms_docs_stats(terms, documents, freq_dtm=None, feature_names=None):
    """
    Given a set of documents, return statistics on appearance of a set of terms in them
    :param terms: The terms to look for
    :param documents: The set of documents where to look for the term
    :param freq_dtm: If there is a DTM already calculated, do not calculate it again
    :param feature_names: ndarray with the column names of the DTM (terms)
    :return: A dictionary with statistics based on appearance each term in the documents
    """
    if freq_dtm is None:
        vec = CountVectorizer()
        freq_dtm = vec.fit_transform(documents)
        feature_names = np.array(vec.get_feature_names())

    result = {}
    for term in terms:
        result[term] = get_term_docs_stats(term, documents, freq_dtm, feature_names)
    return result


def get_term_docs_stats(term, documents, freq_dtm=None, feature_names=None):
    """
    Given a set of documents, return statistics on appearance of one term in them
    :param term: The term to look for
    :param documents: The set of documents where to look for the term
    :param freq_dtm: If there is a DTM already calculated, do not calculate it again
    :param feature_names: ndarray with the column names of the DTM (terms)
    :return: A dictionary with statistics based on appearance the searched term in the documents
    """
    if freq_dtm is None:
        vec = CountVectorizer()
        freq_dtm = vec.fit_transform(documents)
        feature_names = np.array(vec.get_feature_names())

    num_docs = len(documents)
    #Get the frequency column for the term across the documents
    term_index = np.where(feature_names == term)[0]
    term_freq = freq_dtm[:, term_index].toarray().flatten()
    # Get the nonzero frequencies
    freq_nonzero_index = term_freq.nonzero()[0]
    freq_nonzero_docs = [documents[i] for i in freq_nonzero_index]
    nonzero_frequencies = term_freq[freq_nonzero_index]

    result= {}
    # Total appearances of the term in the documents
    result['num_appearances'] = term_freq.sum()
    # Number of documents where the term appears
    result['num_docs_with_term'] = len(nonzero_frequencies)  # np.count_nonzero(term_freq)
    result['pct_docs_with_term'] = result['num_docs_with_term'] / float(num_docs)
    # Statistics of frequency of the term in the documents
    result['term_in_doc_median'] = np.median(nonzero_frequencies)
    result['term_in_doc_mean'] = np.mean(nonzero_frequencies)
    result['term_in_doc_std'] = np.std(nonzero_frequencies)
    # Mean position of the first appearance of the term in the documents
    position_first_occurrences_abs = []  # Positions of the first occurrences
    position_first_occurrences_pct = []  # Positions relative to the length of each document
    for doc in freq_nonzero_docs:
        words = get_words(doc, lowercase=True)
        first_occurrence_abs = words.index(term) + 1.
        first_occurrence_pct = first_occurrence_abs / len(words)
        position_first_occurrences_abs.append(first_occurrence_abs)
        position_first_occurrences_pct.append(first_occurrence_pct)
    result['pos_first_appearance_abs_median'] = np.median(position_first_occurrences_abs)
    result['pos_first_appearance_abs_mean'] = np.mean(position_first_occurrences_abs)
    result['pos_first_appearance_abs_std'] = np.std(position_first_occurrences_abs)
    result['pos_first_appearance_pct_median'] = np.median(position_first_occurrences_pct)
    result['pos_first_appearance_pct_mean'] = np.mean(position_first_occurrences_pct)
    result['pos_first_appearance_pct_std'] = np.std(position_first_occurrences_pct)

    return result

#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'fmrm'

from operator import itemgetter
import numpy as np
from scipy.sparse.csc import csc_matrix

UNSORTED = 0
SORT_ON_TERM = 1
SORT_ON_FREQUENCY = 2


def get_term_freq(freq_dtm, col_names, sort=UNSORTED, reverse_sort=False):
    """
    :param freq_dtm: Document Term Matrix
    :param col_names: Column names (terms)
    :param sort: Flag to sort result
    :param reverse_sort: Flag to sort reversed
    :return: List of (term, frequency) pairs
    """
    arr_freq = freq_dtm.sum(axis=0).getA1()
    result = zip(col_names, arr_freq)
    if sort == SORT_ON_TERM:
        result = sorted(result, key=itemgetter(0), reverse=reverse_sort)
    elif sort == SORT_ON_FREQUENCY:
        result = sorted(result, key=itemgetter(1), reverse=reverse_sort)
    return result


def subtract_dtm_frequencies(dtm_1, terms_1, dtm_2, terms_2):
    """
    :param dtm_1: DTM to subtract frequencies from
    :param terms_1: Terms (column names) for dtm_1
    :param dtm_2: DTM to subtract frequencies to dtm_1
    :param terms_2: Terms (column names) for dtm_2
    :return: DTM with a unique row with the difference of frequencies from terms_1 minus terms_2
    """
    arr_freq_1 = dtm_1.sum(axis=0).getA1()
    arr_freq_2 = dtm_2.sum(axis=0).getA1()
    return csc_matrix(subtract_term_frequencies(terms_1, arr_freq_1, terms_2, arr_freq_2))


def subtract_term_frequencies(terms_1, freq_1, terms_2, freq_2):
    """
    :param terms_1: ndarray of terms
    :param freq_1: ndarray of frequencies for terms in term_1
    :param terms_2: ndarray of terms whose frequencies are subtracted to freq_1
    :param freq_2: ndarray of frequencies for terms in term_2.
                   If there are repeated term/frequency pairs, only the first occurrence is considered
    :return: ndarray with the difference of frequencies from terms_1/freq_1 minus terms_2/freq_2
    """
    result = np.empty_like(freq_1)
    for index, term in enumerate(terms_1):
        frequencies = freq_2[np.where(terms_2 == term)[0]]
        if frequencies.any():
            result[index] = freq_1[index] - frequencies[0]
        else:
            result[index] = freq_1[index]
    return result


def set_frequency_as_percentage(freq_dtm):
    """
    Converts absolute frequencies of a DTM into percentages
    :param freq_dtm: DTM with absolute count frequencies
    :return dense matrix with percentage frequencies
    """
    term_freqs = freq_dtm.sum(axis=0)
    total_freqs = float(term_freqs.sum())
    result = term_freqs / total_freqs
    return result[0]


def equal(dtm1, dtm2):
    """
    Compare two sparse matrices
    :param dtm1: Sparse matrix to compare
    :param dtm2: Sparse matrix to compare
    :return: True if matrices have the same shape and elements
    """
    return (dtm1.shape == dtm2.shape) and \
           ((dtm1 - dtm2).nnz == 0)

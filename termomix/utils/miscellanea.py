#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""

"""

from operator import itemgetter
from sklearn.externals import joblib
from os.path import join

from scipy.sparse import spmatrix

SUBTRACTION_INTEGER = -1
SUBTRACTION_ABSOLUTE = 0
SUBTRACTION_POSITIVE = 1


def is_unicode(text):
    return isinstance(text, unicode)


def get_words_count(words, sort=False, reverse_sort=False):
    counted = {}
    for word in words:
        if word in counted:
            counted[word] += 1
        else:
            counted[word] = 1

    if sort:
        result = sorted(counted.iteritems(), key=itemgetter(1), reverse=reverse_sort)
    else:
        result = counted.items()
    return result


def subtract_words_count(x, y, mode=SUBTRACTION_INTEGER, sort=False, reverse_sort=False):
    """
    Subtracts two wordcount lists
    :param x: minuend wordcount list
    :param y: subtrahend wordcount list
    :param mode: SUBTRACTION_INTEGER returns positive and negative wordcount values
                 SUBTRACTION_ABSOLUTE returns absolute wordcount values
                 SUBTRACTION_POSITIVE returns only positive wordcount values
    :param sort: If resulting wordcount must be ordered or not
    :param reverse_sort: Reverse order or not
    :return: List of pairs (word, number_of_occurrences), where number of occurrences is the result of
             subtracting each occurrence of word in y to each occurrence of word in x
    """
    dict_y = dict(y)
    result = []
    for word, count in x:
        difference = count - dict_y.get(word, 0)
        if mode == SUBTRACTION_POSITIVE:
            if difference > 0:
                result.append((word, difference))
        elif mode == SUBTRACTION_ABSOLUTE:
            result.append((word, abs(difference)))
        elif mode == SUBTRACTION_INTEGER:
            result.append((word, difference))

    if sort:
        result = sorted(result, key=itemgetter(1), reverse=reverse_sort)

    return result


def get_instance(name):
    """
    Retrieve a python object, given its dotted.name.
    """
    parts = name.split('.')
    parts_copy = parts[:]
    module = None
    while parts_copy:
        try:
            module = __import__('.'.join(parts_copy))
            break
        except ImportError:
            del parts_copy[-1]
            if not parts_copy:
                raise
    parts = parts[1:]
    obj = module
    for part in parts:
        parent, obj = obj, getattr(obj, part)
    return obj

def generate_n_primes(n_primes):
    """
    Generates a list with the first n_primes primes
    :param n_primes: int for generate the list with the N first primes
    :return: list with n_primes primes
    """
    primes = []
    chkthis = 2
    while len(primes) < n_primes:
        ptest = [chkthis for i in primes if chkthis%i == 0]
        primes += [] if ptest else [chkthis]
        chkthis += 1
    return primes


def persist_objects(object_names, objects, dir_path):
    for idx, obj in enumerate(objects):
        joblib.dump(obj, join(dir_path, object_names[idx] + '.pkl'))


def load_objects(object_names, dir_path):
    return [joblib.load(join(dir_path, name + '.pkl')) for name in object_names]


def ismatrix(matrix):
    if isinstance(matrix, spmatrix):
         return True
    else:
        return False
#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""

"""

from datasets.base_dataset import BaseDataset
from sklearn.cross_validation import train_test_split
from sklearn.externals import joblib
from os.path import join

def get_instance(name):
    """
    Retrieve a python object, given its dotted.name.
    """
    parts = name.split('.')
    parts_copy = parts[:]
    while parts_copy:
        try:
            module = __import__('.'.join(parts_copy))
            break
        except ImportError:
            del parts_copy[-1]
            if not parts_copy:raise
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

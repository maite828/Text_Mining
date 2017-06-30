#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import csv
from time import time
from sklearn.cross_validation import train_test_split
import codecs
import logging
from miscellanea import ismatrix
import types

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')


def read_csv(X, y, file_path, **kwargs):
    """
    Builds X_src from a csv file. It expects the first line of the file to be the header.
    :param file_path: is the absolute path to csv file
    :param selected_attr: list of attr to be loaded
    :param **kwargs is a dict that can contain arbitrary parameters. For the case of this function:
        - 'selected_attr' key must exist with column name for attributes
        - if 'delimiter' key not exist then '\t' is used as the column delimiter.
        - if 'target' key exist then uses this column as target
    """
    logging.debug('Loading source from %s' % file_path)
    t0 = time()

    selected_attr = kwargs['selected_attr']
    X_src = []
    delimiter = kwargs['delimiter'] if 'delimiter' in kwargs else '\t'
    encoding = kwargs['encoding'] if 'encoding' in kwargs else 'UTF-8'
    ifile = codecs.open(file_path, 'rb', encoding=encoding)
    header = {attr: idx for idx, attr in enumerate((ifile.readline()).rstrip('\n').split(delimiter))}
    selected_attr_idx = [header[attr] for attr in selected_attr]
    if 'target' in kwargs:
        target_idx = header[kwargs['target']]
    for line in ifile:
        instance_attr = []
        row = line.rstrip('\n').split(delimiter)
        for attr_idx in selected_attr_idx:
            try:
                instance_attr.append(float(row[attr_idx]))
            except ValueError:
                try:
                    instance_attr.append(float(int(row[attr_idx])))
                except ValueError:
                    instance_attr.append(row[attr_idx])
            except TypeError:
                instance_attr.append(row[attr_idx])

        if 'target' in kwargs:
            try:
                y.append(int(row[header[kwargs['target']]]))
            except:
                y.append(row[header[kwargs['target']]])
        X_src.append(instance_attr)
    X.append(X_src)

    logging.debug(" done in %fs" % (time() - t0))


def read_txt(X, y, file_path, **kwargs):
    """
    Builds X_src specific for text from a csv file. It expects the first line of the file to be the header.
    :param file_path: is the absolute path to csv file
    :param **kwargs is a dict that can contain arbitrary parameters. For the case of this function:
        - 'selected_attr' key must exist with column name for extract de text
        - if 'delimiter' key not exist then '\t' is used as the column delimiter.
        - if 'target' key exist then uses this column as target
    """
    logging.debug('Loading source from %s' % file_path)
    t0 = time()

    selected_attr = kwargs['selected_attr']
    X_src = []
    delimiter = kwargs['delimiter'] if 'delimiter' in kwargs else '\t'
    encoding = kwargs['encoding'] if 'encoding' in kwargs else 'UTF-8'
    ifile = codecs.open(file_path, 'rb', encoding=encoding)
    header = {attr: idx for idx, attr in enumerate((ifile.readline()).rstrip('\n').split(delimiter))}
    selected_attr_idx = [header[attr] for attr in selected_attr]
    for line in ifile:
        row = line.rstrip('\n').split(delimiter)
        for attr_idx in selected_attr_idx:
            text = row[attr_idx]
            if 'target' in kwargs:
                try:
                    y.append(int(row[header[kwargs['target']]]))
                except:
                    y.append(row[header[kwargs['target']]])
            X_src.append(text)
    X.append(X_src)

    logging.debug(" done in %fs" % (time() - t0))


def write_csv(X, y, file_path, **kwargs):
    """

    :param X: collection of instances and their attributes. Can be sparse matrix,
    numpy array or regular python list
    :param y: can be None and contains the target
    :param file_path: the filepath to write
    :param kwargs: optional arguments:
        delimiter('\t' default value)
        enconding: UTF-8 is the default
        select_attr: (CURRENTLY IT IS MANDATORY)
                   list of tuples ('attribute_name', attribute_index) or
                   list of strings(or unicode) with attribute names
        target: name of the target
    :return: None
    """
    logging.debug('_' * 80)
    logging.debug ('Writing file %s' % file_path)
    t0 = time()
    encoding = kwargs['encoding'] if 'encoding' in kwargs else 'UTF-8'
    ofile = codecs.open(file_path, 'w', encoding=encoding)

    delimiter = kwargs['delimiter'] if 'delimiter' in kwargs else u'\t'

    if ismatrix(X):
        X = X.toarray()

    if isinstance(kwargs['selected_attr'][0], types.TupleType):
        header = delimiter.join(attr[0] for attr in kwargs['selected_attr'])
        select_attr_idx = [attr[1] for attr in kwargs['selected_attr']]
    else:
        header = delimiter.join(attr for attr in kwargs['selected_attr'])
        l = len(X[0])
        select_attr_idx = range(0, l)

    if 'target' in kwargs:
        header += delimiter + kwargs['target']
    ofile.write(header + u'\n')

    for idx, instance in enumerate(X):
        select_attr_tmp = []
        for attr_idx in select_attr_idx:
            select_attr_tmp.append(instance[attr_idx])
        line = delimiter.join((str(attr_value) if isinstance(attr_value, float) else attr_value)
                              for attr_value in select_attr_tmp)
        line += (delimiter + str(y[idx])) if 'target' in kwargs else ''
        ofile.write(line + u'\n')
    ofile.close()

    logging.debug(" done in %fs" % (time() - t0))


def split_train_dev_test(input_file, train_file, dev_file, test_file, test_size=0.33, random_state=42, **kwargs):
    """
    :param input_file: file path to dataset
    :param train_file: file path to write the train instances
    :param dev_file: file path to write the dev instances
    :param test_file: file path to write the test instances
    :param test_size: percentage of instances for test and dev(ech of them. Percentage of the total dataset)
    :param random_state: int or RandomState. Pseudo-random number generator state used for random sampling.
    :param kwargs: is a dict that can contain:
        - 'selected_attr' key must exist with column name for extract de attributes
        - if 'delimiter' key not exist then '\t' is used as the column delimiter.
        - if 'target' key exist then uses this column as target
    :return: None
    """

    X, y = [], []
    # X_train and y_train are filled with the original file
    read_csv(X, y, input_file, **kwargs)

    split_train_dev_test(X, y, train_file, dev_file, test_file, test_size, random_state, **kwargs)


def split_train_dev_test(X, y, train_file, dev_file, test_file, test_size=0.2, random_state=42, **kwargs):
    """
    Given a dataset(X,y) it splits it train, dev and test preserving the class ratios
    :param X: instances attributes
    :param y: instances target
    :param train_file: file path to write the train instances
    :param dev_file: file path to write the dev instances
    :param test_file: file path to write the test instances
    :param test_size: percentage of instances for test and dev(ech of them. Percentage of the total dataset)
    :param random_state: int or RandomState. Pseudo-random number generator state used for random sampling.
    :param kwargs: is a dict that can contain:
        - 'selected_attr' key must exist with column name for extract de text
        - if 'delimiter' key not exist then '\t' is used as the column delimiter.
        - if 'target' key exist then uses this column as target
    :return: None
    """
    X_test, y_test = [], []
    if dev_file is not None:
        logging.debug("Splitting dataset into train, test and dev")
        logging.debug(" First split")
        t0 = time()
        X, X_test, y, y_test = train_test_split(X, y, test_size=test_size, random_state=random_state)
        logging.debug(" done in %fs" % (time() - t0))

        write_csv(X_test, y_test, test_file, **kwargs)

        logging.debug(" Second split")
        t0 = time()
        X, X_test, y, y_test = train_test_split(X, y, test_size=test_size, random_state=random_state)
        print(" done in %fs" % (time() - t0))

        write_csv(X, y, train_file, **kwargs)
        write_csv(X_test, y_test, dev_file, **kwargs)
    else:
        logging.debug("dev_file is None. Splitting dataset into train and test")
        t0 = time()
        X, X_test, y, y_test = train_test_split(X, y, test_size=test_size, random_state=random_state)
        write_csv(X, y, train_file, **kwargs)
        write_csv(X_test, y_test, test_file, **kwargs)


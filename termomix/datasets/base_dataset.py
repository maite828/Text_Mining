#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import numpy as np

from sklearn.cross_validation import StratifiedKFold
from os.path import join
from utils.miscellanea import is_unicode


class BaseDataset(object):

    DEFAULT_ENCODING = 'UTF-8'

    def __init__(self, configuration):
        self.configuration = configuration
        # X_train, X_test are a matrix of (#instances,#sources). Each cell contains
        # a matrix of (#instances,#attr_of_i_source)
        self.X_train = []
        self.X_test = []
        # y_train, y_test are a list (#instances). Each cell contains the target for a given instance.
        self.y_train = []
        self.y_test = []
        # X_names is a list of lists. Each list contains the names of the attributes for this source.
        self.X_names = configuration['src_feature_names'] if 'src_feature_names' in configuration else None

    def load_dataset(self):
        """
        Loads sources files. Source files will be loaded into X_train or X_test base on the configuration.
        If train_src_names exists then sources will be loaded into X_train/y_train, otherwise
        if test_src_names exists then sources will be loaded into X_test/y_test.
        Functions used to load each source are specified on the configuration. Any function that follows
        this signature can be used:

            def load_func(X, y, source_file_path, **kwargs)

        """
        X = None
        y = None
        src_names = None
        if 'train_src_names' in self.configuration:
            X = self.X_train
            y = self.y_train
            src_names = self.configuration['train_src_names']
        elif 'test_src_names' in self.configuration:
            X = self.X_test
            y = self.y_test
            src_names = self.configuration['test_src_names']

        for idx, src in enumerate(src_names):
            src_file_path = join(self.configuration['data_path'], src_names[idx])
            self.configuration['src_load_function'][idx][0](X, y, src_file_path,
                                                            ** self.configuration['src_load_function'][idx][1])

        # self.y_train must be a numpy array and we do not know in advance the number of instances
        if len(y) > 0:
            y = np.array(y)
        self.n_instances = len(X[0])

    def get_texts(self, index):
        """
        Reads all the lines from one of the source documents, indicated by the index param
        :param index: The index of the source document in the source documents list
        :return: A list with the unicode texts (lines) of the source document
        """
        # Assuming the encoding is the same for all texts. Check only the first one.
        if is_unicode(self.X_train[index][0]):
            return self.X_train[index]
        else:
            return self.get_unicode_texts(index)

    def get_unicode_texts(self, index):
        return [unicode(text, encoding=self.configuration['encoding']) for text in self.X_train[index]]

    def get_fold(self):
        """
            This cross-validation object is a variation of KFold that
            returns stratified folds. The folds are made by preserving
            the percentage of samples for each class.

            Assumes that self.y_train is already initialized
        """
        yield StratifiedKFold(self.y_train, n_folds=self.configuration['num_folds'], random_state=42)

    def get_evaluation_function(self):
        """
        Returns keyword to identify the evaluation function that wants to use and optional params
        :return: if 'eval_func' not present in self.configuration then ('accuracy', None)
        """
        if 'eval_func' in self.configuration:
            return self.configuration['eval_func'][0]
        else:
            return 'accuracy'

    def get_evaluation_function_args(self):
        """
        Returns keyword to identify the evaluation function that wants to use and optional params
        :return: if 'eval_func' not present in self.configuration then ('accuracy', None)
        """
        if 'eval_func' in self.configuration:
            return self.configuration['eval_func'][1]
        else:
            return {'normalize': True, 'sample_weight': None}




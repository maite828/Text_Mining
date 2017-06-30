#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

""""
Dataset representation for transcriptions/meta of a call center.
"""
import fnmatch
import codecs
from os.path import join, isfile, isdir
from os import listdir
import csv
import sys

from datasets.base_dataset import BaseDataset
from utils.io_tools import read_csv, split_train_dev_test
from utils.miscellaneous import generate_n_primes
from sklearn.cross_validation import StratifiedKFold
import numpy as np


TEST_SIZE = 0.33
RANDOM_STATE = 42
DEFAULT_ENCODING = 'UTF-8'


class CRMDataset(BaseDataset):

    def __init__(self, configuration):
        BaseDataset.__init__(self, configuration)

    def load_dataset(self):
        BaseDataset.load_dataset(self)

    def get_fold(self):
        if 'bootstrap' in self.configuration and \
                        self.configuration['bootstrap'] > 1:
            n_bootstrap = self.configuration['bootstrap']
            for random in generate_n_primes(n_bootstrap):
                yield StratifiedKFold(self.y_train,
                                      n_folds=self.configuration['num_folds'],
                                        shuffle=False, random_state=random)
        else:
            yield StratifiedKFold(self.y_train,
                                  n_folds=self.configuration['num_folds'],
                                  random_state=42)

    @staticmethod
    def _read_text_from_individuals(configuration):
        """
        Loads transcriptions from files
        """
        # load sids to only load transcriptions with meta
        sids = []
        tipos = []
        with open(join(configuration['data_path'], configuration['source_names'][1])) as f:
            reader = csv.DictReader(f, delimiter='\t') # read rows into a dictionary format
            for row in reader: # read a row as {column1: value1, column2: value2,...}
                sids.append(row['sid_key'])
                tipos.append(row['tipo'])

        print('Load transcriptions from separated files ...')
        ofile = codecs.open(join(configuration['data_path'], configuration['source_names'][0]),
                            'w', DEFAULT_ENCODING)
        X_src = []
        for ifile in listdir(configuration['data_path']):
            if ifile != u'.DS_Store' and fnmatch.fnmatch(ifile, '*.txt'):
                sid = ifile[4:32]
                if sid in sids:
                    text = ''
                    with codecs.open(join(configuration['data_path'], ifile), 'r', DEFAULT_ENCODING) as process_file:
                        for line in process_file:
                            word, time_ini, time_end, locutor = line.split(' ')
                            text += ' ' + word

                        if text != '':
                            X_src.append(text)
                            ofile.write(text + u'\n')

        ofile.close()
        return X_src


def main(argv):
    audio_features_names = ["cust_mean_articulation_rate",
                        "cust_sd_articulation_rate",
                        "age_mean_articulation_rate",
                        "age_sd_articulation_rate",
                        "cust_mean_f0",
                        "cust_sd_f0",
                        "cust_mean_loudness",
                        "cust_sd_loudness",
                        "age_mean_f0",
                        "age_sd_f0",
                        "age_mean_loudness",
                        "age_sd_loudness",
                        "text"
                        ]

    configuration = {"encoding":"UTF-8",
                     "data_path":"/home/usertestpe/data/04_ChurnMovil/\
                     2.Speech_Analytics/201403",
                     "train_src_names": ["audio_text.binary"],
                     "src_feature_names":[audio_features_names]
                    }

    #Split original file into train and test:
    data_path = '/Users/zoraida/Analytics/analytics-text-mining/data/lleida/'

    X, y = [], []
    input_file = join(data_path, configuration['train_src_names'][0])
    kwargs = {'delimiter': '\t', 'selected_attr': audio_features_names,
              'target': 'response'}

    read_csv(X, y, input_file, **kwargs)

    train_file = join(data_path, 'train_' + configuration['train_src_names'][0])
    test_file = join(data_path, 'test_' + configuration['train_src_names'][0])

    split_train_dev_test(X[0], y, train_file, None, test_file,
                         test_size=0.2, random_state=1234, **kwargs)



if __name__ == '__main__':
    main(sys.argv[1:])
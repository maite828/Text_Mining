#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import os
from datasets.lleida import CRMDataset

from utils.io_tools import read_csv, read_txt

from sklearn.feature_extraction.text import TfidfVectorizer
#from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.multiclass import OneVsRestClassifier
from sklearn.svm import LinearSVC, SVC
from sklearn.linear_model import LogisticRegression
from sklearn.feature_selection import f_classif, f_regression, chi2
from sklearn.feature_selection import SelectKBest
from preprocessing.transformers import GetItemTransformer

try:
    from preprocessing.text.lemmatizer import LemmaTokenizer
    tokenizer = LemmaTokenizer("es")
except ImportError:
    print ("Warning: Error loading lemmatizer module.")
    print ("Warning: Lemmatization won't be applied to '%s' configuration."
           % os.path.basename(__file__))
    tokenizer = None

audio_features_names = [#"cust_mean_articulation_rate",
                        #"cust_sd_articulation_rate",
                        #"age_mean_articulation_rate",
                        "age_sd_articulation_rate",
                        #"cust_mean_f0",
                        "cust_sd_f0",
                        "cust_mean_loudness",
                        #"cust_sd_loudness",
                        #"age_mean_f0",
                        "age_sd_f0",
                        "age_mean_loudness",
                        #"age_sd_loudness"
                        ]

num_folds = 10

cls_train_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    "data_path": "../../data/lleida",
    # source_names contains file names of each source. They are expected to be
    # into data_path dir
    "train_src_names": ["audio_text.binary"],
    # source_load_function contains in 0 the function to load the source,
    # in 1 : **kwargs
    "src_load_function": [
        (read_csv,{'delimiter': '\t', 'target': 'response',
                   'selected_attr': audio_features_names})],
    "src_transformer": [('idem_audio', GetItemTransformer())],
                        #('minmax', MinMaxScaler(feature_range=(-1,1)))],
    "score_funcs": [('anova_reg_audio', SelectKBest(score_func=f_regression))
                    #('anova_cls', SelectKBest(score_func=f_classif)),
                    #('chi', SelectKBest(score_func=chi2))
    ],
    #"score_funcs": (('chi2',SelectKBest(score_func=chi2)),
    # ('fclassif',SelectKBest(score_func=f_classif))),
    # source_features_names is used for:
    # if the source file is csv then as the columns to load from it
    # if the transformer applied to this source does not have get_feature_names
    # function then as feature names
    "src_feature_names": [audio_features_names],
    "classifiers": [#('linearSvc', LinearSVC(class_weight='auto')),
                    ('svc_audio', SVC(kernel='rbf', C=1, gamma=0.2,
                                      class_weight={1: 1/0.1368631,
                                                    0: 1/0.8631369},
                                      max_iter=-1, verbose=5,
                                      probability=False))
                    #('regression', LogisticRegression(class_weight='auto')),
                    ],
    "show_conf_matrix": False,
    "show_scoring_matrix": True,
    "num_attr": [5],
    "num_folds": num_folds,
    #"eval_func": ("accuracy", {'pos_label': 1}),
    "eval_func": ("f1", {'pos_label': 1}),
    "bootstrap": 1000,
    "save_model": True
}

cls_test_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    "data_path": "../../data/lleida",
    # train_src_names contains file names of each source.
    # They are expected to be into data_path dir
    "train_src_names": ["audio_text.binary"],
    # src_load_function contains in 0 the function to load the source,
    # in 1 : **kwargs
    "src_load_function": [(read_csv, {'delimiter': '\t', 'target': 'response',
                                      'selected_attr': audio_features_names})],
    "src_transformer": ['idem_audio'],
    "score_func": 'anova_reg_audio',
    # src_features_names is used for:
    # if the transformer applied to this source does not have
    # get_feature_names function then as feature names
    "src_feature_names": [audio_features_names],
    "classifier": 'svc_audio',
    "show_conf_matrix": True,
    "show_scoring_matrix": True,
    #"eval_func": ("accuracy", {'pos_label': 1})
    "eval_func": ("f1", {'pos_label': 1})
    #"eval_func": ("lift", {'pos_label': 1})
    #"eval_func": ("roc", {'pos_label': 1})
}


def get_cls_train_cfg():
    return cls_train_cfg


def get_cls_test_cfg():
    return cls_test_cfg
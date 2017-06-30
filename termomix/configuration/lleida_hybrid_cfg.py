#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import os
from datasets.lleida import CRMDataset

from utils.io_tools import read_csv, read_txt

from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.svm import LinearSVC, SVC
from sklearn.linear_model import LogisticRegression
from sklearn.feature_selection import f_classif, f_regression
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
    "train_src_names": ("train_audio_text.binary", "train_audio_text.binary"),
    # source_load_function contains in 0 the function to load the source,
    # in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t',
                                      'selected_attr': ['text']}),
                          (read_csv,{'delimiter': '\t', 'target': 'response',
                                     'selected_attr': audio_features_names})),
    "src_transformer": (('tfidf_hybrid', TfidfVectorizer()),
                        #('idem_hybrid', GetItemTransformer())),
                        ('minmax_hybrid', MinMaxScaler())),
    "score_funcs": [('anova_cls_hybrid', SelectKBest(score_func=f_classif)),
                    ('anova_reg_hybrid',
                     SelectKBest(score_func=f_regression))],
    #"score_funcs": (('chi2',SelectKBest(score_func=chi2)),
    # ('fclassif',SelectKBest(score_func=f_classif))),
    # source_features_names is used for:
    # if the source file is csv then as the columns to load from it
    # if the transformer applied to this source does not have get_feature_names
    # function then as feature names
    "src_feature_names": (None, audio_features_names),
    "classifiers": (('svc_hybrid', SVC(kernel='rbf', C=1, gamma=0.2,
                                       class_weight={1: 1/0.1368631,
                                                     0: 1/0.8631369},
                                       max_iter=-1, verbose=5,
                                       probability=False)),
                    ('regression_hybrid',
                     LogisticRegression(class_weight='auto'))),
    "show_conf_matrix": False,
    "show_scoring_matrix": True,
    "num_attr": (12, 500, 1000, 1500),
    "num_folds": num_folds,
    "eval_func": ("f1", {'pos_label': 1}),
    #"eval_func": ("roc", {'labels': None, 'pos_label': 'B'}),
    "save_model": True
}

cls_test_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    "data_path": "../../data/lleida",
    # train_src_names contains file names of each source.
    # They are expected to be into data_path dir
    "train_src_names": ("test_audio_text.binary", "test_audio_text.binary"),
    # src_load_function contains in 0 the function to load the source,
    # in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t',
                                      'selected_attr': ['text']}),
                          (read_csv,{'delimiter': '\t', 'target': 'response',
                                     'selected_attr': audio_features_names})),
    "src_transformer": ['tfidf_hybrid', 'minmax_hybrid'],
    "score_func": 'anova_reg_hybrid',
    # src_features_names is used for:
    # if the transformer applied to this source does not have
    # get_feature_names function then as feature names
    "src_feature_names": [None, audio_features_names],
    "classifier": 'svc_hybrid',
    "show_conf_matrix": True,
    "show_scoring_matrix": True,
    #"eval_func": ("accuracy", {'pos_label': 1})
    "eval_func": ("f1", {'pos_label': 1})
    #"eval_func": ("gain", {'pos_label': 1})
}


def get_cls_train_cfg():
    return cls_train_cfg


def get_cls_test_cfg():
    return cls_test_cfg
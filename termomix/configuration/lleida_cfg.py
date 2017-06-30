#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import os
from datasets.lleida import CRMDataset

from utils.io_tools import read_csv, read_txt

from sklearn.feature_extraction.text import TfidfVectorizer
#from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import StandardScaler
from sklearn.multiclass import OneVsRestClassifier
from sklearn.svm import SVC
from sklearn.linear_model import LogisticRegression
from sklearn.feature_selection import f_classif, f_regression
from sklearn.feature_selection import SelectKBest

try:
    from preprocessing.text.lemmatizer import LemmaTokenizer
    tokenizer = LemmaTokenizer("es")
except ImportError:
    print ("Warning: Error loading lemmatizer module.")
    print ("Warning: Lemmatization won't be applied to '%s' configuration." % os.path.basename(__file__))
    tokenizer = None

meta_features_names = ["duration"]

num_folds = 2

cls_train_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    "data_path": "../../data/lleida",
    # source_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ("transcriptions.csv", "meta.csv", "churn_kept_features.txt"),
    # source_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ['text']}),
                          (read_csv,{'delimiter': '\t', 'target': 'tipo',
                                     'selected_attr': meta_features_names}),
                          (read_csv,{'delimiter': '\t', 'target': 'tipo',
                                     'selected_attr': meta_features_names})),
    "src_transformer": (('tfidf', TfidfVectorizer()), ('minmax', StandardScaler())),
    "score_funcs": [('chi2', SelectKBest(score_func=f_classif))],
    #"score_funcs": (('chi2',SelectKBest(score_func=chi2)), ('fclassif',SelectKBest(score_func=f_classif))),
    # source_features_names is used for:
    # if the source file is csv then as the columns to load from it
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    "src_feature_names": (None, meta_features_names),
    "classifiers": (('svc', SVC(probability=True, kernel='linear')),
                    ('regression', LogisticRegression(class_weight='auto'))),
    "show_conf_matrix": False,
    "show_scoring_matrix": True,
    "num_attr": (1000, 1250, 1100, 750),
    "num_folds": num_folds,
    "eval_func": ("f1", {'pos_label': 'B'})
    #"eval_func": ("roc", {'labels': None, 'pos_label': 'B'})
}

def get_cls_train_cfg():
    return cls_train_cfg
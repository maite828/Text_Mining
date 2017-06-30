#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import os
import logging
from datasets.peru import CRMDataset
from visualizations.wordcloud import WordCloud

from utils.io_tools import read_csv, read_txt

from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.preprocessing import MinMaxScaler
from sklearn.svm import LinearSVC
from sklearn.linear_model import LogisticRegression
from sklearn.feature_selection import f_classif, chi2
from sklearn.feature_selection import SelectKBest

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')

try:
    from lemmatizer import LemmaTokenizer
    tokenizer = LemmaTokenizer("es")
except ImportError: 
    logging.warning("Error loading lemmatizer module.")
    logging.warning("Lemmatization won't be applied to '%s' configuration." % os.path.basename(__file__))
    tokenizer = None

meta_feature_names = ("Duration", "NumberOfHolds", "NumberOfConferences", "NumberOfTransfers",
                      "TotalHoldTime", "WrapUpTime", "TOTAL_AGENT_TALK_TIME",
                      "NUM_AGENT_TALK_SEGMENTS", "TOTAL_CUSTOMER_TALK_TIME",
                      "NUM_CUSTOMER_TALK_SEGMENTS", "TOTAL_SILENCE_TIME", "NUM_SILENCE_SEGMENTS",
                      "TOTAL_TALK_OVER_TIME", "NUM_TALK_OVER_SEGMENTS",
                      "NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS",
                      "NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS")

cls_train_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    # Path relative to the learn module
    "data_path": "test/test-data/peru-calls/",
    # source_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ("info.csv.10","meta.csv.10"),
    # source_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ["text"]}),
                          (read_csv, {'delimiter': '\t','target': 'downgrade',
                                      'selected_attr': meta_feature_names})),
    "src_transformer": (('tfidf', TfidfVectorizer()), ('minmax', MinMaxScaler())),
    "score_funcs": (('chi2', SelectKBest(score_func=chi2)), ('fclassif', SelectKBest(score_func=f_classif))),
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    "src_feature_names": (None, meta_feature_names),
    "classifiers": (('svc', LinearSVC(class_weight='auto')),
                    ('regression', LogisticRegression(class_weight='auto'))),
    "show_conf_matrix": False,
    "show_scoring_matrix": False,
    "num_attr": (1000, 1250, 1100),
    "num_folds": 2
}

wordcloud_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    # Paths relative to the visualization module
    "data_path": "test/test-data/peru-calls/",
    # source_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ("customer_downgrades-1.csv", "customer_downgrades-2.csv"),
    # Multi-Scr is "enumerate" (generates individual wordclouds for sources) or "compare" (substracts words from
    # two sources, in both ways, and put the results in two wordclouds)
    "multi_src": WordCloud.MULTISOURCE_COMPARE,
    "output_path": "test/test-data/peru-calls/",
    # source_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ["text"]}),
                          (read_txt, {'delimiter': '\t', 'selected_attr': ["text"]})),
    "stopwords_lang": "spanish",
    "stopwords_files": ["crm", "numbers"],
    "num_tags": 100,
    "min_tag_size": 12,
    "max_tag_size": 70,
    "image_size": (1400, 1000),
    "font": "Nobile",
    "background": (0, 0, 0),
    # This to generate statistics on the most frequent terms
    "max_freq_terms": 20
}


def get_cls_train_cfg():
    return cls_train_cfg


def get_wordcloud_cfg():
    return wordcloud_cfg

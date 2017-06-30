# !/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: fmrm
# Created: 10/07/2014
from datasets.webcrawl import WebCrawlDataset
from visualizations.wordcloud import WordCloud

from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.multiclass import OneVsRestClassifier
from sklearn.svm import LinearSVC
from sklearn.feature_selection import chi2
from sklearn.feature_selection import SelectKBest


clf_train_cfg = {
    "dataset_class": WebCrawlDataset,
    "encoding": "UTF-8",
    # Paths relative to the learn module
    "data_path": "../../data/webcrawl",
    # Source_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ["part-r-00000.detid.clean"],
    "categories_csv": "../../data/webcrawl/domains_categories.csv",
    "src_transformer": [('tfidf',TfidfVectorizer(min_df=1, ngram_range=(1,1), decode_error='ignore', stop_words='english'))],
    "src_feature_names": [[]],
    "score_funcs": [('chi2',SelectKBest(score_func=chi2))],
    "classifiers": [('svc', OneVsRestClassifier(LinearSVC(class_weight='auto', C=1.0, dual=True, fit_intercept=True,
                                                  intercept_scaling=1, loss='l2', multi_class='ovr',
                                                  penalty='l2', random_state=0, tol=0.0001, verbose=0)))],
    "language_filter": "ENG",
    "language_trust_filter": 50,
    "skip_categories": [],
    "categories_level": 1,
    "show_conf_matrix": False,
    "show_scoring_matrix": True,
    "num_attr": [1000],
    "num_folds": 2
}


wordcloud_cfg = {
    "dataset_class": WebCrawlDataset,
    "encoding": "UTF-8",
    # Paths relative to the visualizations module
    "data_path": "../../data/webcrawl/",
    # Source_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ["part-r-00000.detid.clean"],
    # Multi-Scr is "enumerate" (generates individual wordclouds for sources) or "compare" (substracts words from
    # two sources, in both ways, and put the results in two wordclouds)
    "multi_src": WordCloud.MULTISOURCE_ENUMERATE,
    "output_path": "../../data/webcrawl/",
    "language_filter": "ENG",
    "language_trust_filter": 50,
    "stopwords_lang": "english",
    "stopwords_files": [],
    "num_tags": 100,
    "min_tag_size": 12,
    "max_tag_size": 70,
    "image_size": (1400, 1000),
    "font": "Nobile",
    "background": (0, 0, 0),
    # This to generate statistics on the most frequent terms
    "max_freq_terms": 20,
    # TODO: These properties are not needed, but they must be included due to code coupling (should be solved)
    "categories_csv": "../../data/webcrawl/domains_categories.csv",
    "skip_categories": [],
    "categories_level": 1
}


def get_cls_train_cfg():
    return clf_train_cfg


def get_wordcloud_cfg():
    return wordcloud_cfg

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
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.multiclass import OneVsRestClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.lda import LDA
from sklearn.qda import QDA
from sklearn.linear_model import RidgeClassifier
from sklearn.svm import LinearSVC
from sklearn.linear_model import SGDClassifier
from sklearn.linear_model import Perceptron
from sklearn.linear_model import PassiveAggressiveClassifier
from sklearn.naive_bayes import BernoulliNB, MultinomialNB
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neighbors import NearestCentroid
from sklearn.linear_model import LogisticRegression
from sklearn.grid_search import GridSearchCV
from sklearn.metrics import precision_score, recall_score
from sklearn.feature_selection import f_classif, chi2, f_regression
from sklearn.feature_selection import SelectKBest
import numpy as np

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')

try:
    from preprocessing.text.lemmatizer import LemmaTokenizer
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


param_svm = [{'kernel': ['rbf','poly', 'sigmoid'], 'gamma': [1e-3, 1e-4],
               'C': [1, 10, 100, 1000, 10000, 100000]},
              {'kernel': ['linear'], 'C': [1, 10, 100, 1000, 10000, 100000]}]

param_svm_linear = [{'C': [1, 10, 100, 1000, 10000, 100000]}]

parameters_sgd = {
    #'vect__max_df': (0.5, 0.75, 1.0),
    #'vect__max_features': (None, 5000, 10000, 50000),
    #'vect__ngram_range': ((1, 1), (1, 2)),  # unigrams or bigrams
    #'tfidf__use_idf': (True, False),
    #'tfidf__norm': ('l1', 'l2'),
    'clf__alpha': (0.00001, 0.000001),
    'clf__penalty': ('l2', 'elasticnet'),
    #'clf__n_iter': (10, 50, 80),
}
num_folds = 2
param_regression = {'C': np.logspace(-4, 4, 3),
                    'penalty': ('l1', 'l2')}
vocabulary = [u'celular', u'chip', u'comprar', u'conexión', u'correcto', u'código', u'dato', u'entrar', u'equipo',
              u'inconveniente', u'ingresar', u'internet', u'mego', u'navegar', u'opción', u'problema', u'referencia',
              u'servicio', u'teléfono', u'técnico']
cls_train_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    # Path relative to the learn module
    "data_path": "../../data/peru-calls/final_dataset",
    # train_src_names contains file names of each source. They are expected to be into data_path dir
    # train_info.lemma.out tiene train orig lemmatizado sin filtrar
    #"train_src_names": ("train_info.lemma.out", "train_meta.csv"),
    #train_info.lemma.onlycat tiene train orig lemmatizado con solo nombres, verbos y adjetivos
    #"train_src_names": ("train_info_escenario12.semifinal", "train_meta_escenario12.csv"),
    "train_src_names": ("train_info_escenario34_mig.semifinal.sw", "train_meta_escenario34_mig.csv"),
    #"train_src_names": ("train_info_escenario34.semifinal", "train_meta_escenario34.csv"),
    #"train_src_names": ("train_info_escenario34_mig.semifinal", "train_meta_escenario34_mig.csv"),
    # src_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ['text']}),
                          (read_csv, {'delimiter': '\t', 'target': 'downgrade',
                                      'selected_attr': meta_feature_names})),
    "src_transformer": (('tfidf',TfidfVectorizer(min_df=10, ngram_range=(1, 1))), ('minmax', MinMaxScaler())),
    "score_funcs": [#('f_regression', SelectKBest(score_func=f_regression)),('chi2', SelectKBest(score_func=chi2)),
                    ('f_classif', SelectKBest(score_func=f_classif))],
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    "src_feature_names": (None, meta_feature_names),
    # By default the GridSearchCV uses a 3-fold cross-validation. However, if it detects that a classifier is passed,
    # rather than a regressor, it uses a stratified 3-fold.
    #"classifiers": [('svc', SVC(kernel='poly', class_weight='auto', probability=True, max_iter=1000, verbose=5))],
    #"classifiers": [('svc', SVC(probability=True, class_weight='auto', max_iter=1000, verbose=5))],
    #"classifiers": [('svc', OneVsRestClassifier(SVC(class_weight='auto', probability=True, max_iter=10000, verbose=2) ))],
    #"classifiers": [('svc', GridSearchCV(SVC(class_weight='auto', probability=True, max_iter=1000, verbose=5), refit=True, param_grid=param_svm_linear, scoring='roc_auc', n_jobs=-1, verbose=5))],
    #"classifiers": [('regression', GridSearchCV(LogisticRegression(class_weight='auto'), refit=True,
    #                                           param_grid=param_regression, scoring='f1', n_jobs=-1, verbose=5))],
    "classifiers": [('regression', LogisticRegression(class_weight='auto'))],
    # "classifiers": [('regression', LogisticRegression(class_weight='auto')),
    #                 ('svc', SVC(class_weight='auto', max_iter=1000, probability=True)),
    #                 ('kneighbors', KNeighborsClassifier(n_neighbors=1)),
    #                 ('decision_tree', DecisionTreeClassifier(max_depth=5)),
    #                 ('random_forest', RandomForestClassifier(max_depth=5, n_estimators=10, max_features='auto',
    #                                                          n_jobs=-1, verbose=5)),
    #                 ('ada', AdaBoostClassifier()),
    #                 ('lda', LDA()),
    #                 #('qda', QDA()),
    #                 #('Ridge Classifier', RidgeClassifier(tol=1e-2, solver="lsqr", class_weight='auto')),
    #                 #('Perceptron', Perceptron(n_iter=50, class_weight='auto')),
    #                 #('Passive-Aggressive', PassiveAggressiveClassifier(n_iter=50)),
    #                 ('kNN', KNeighborsClassifier(n_neighbors=10)),
    #                 ('sgd', SGDClassifier(alpha=.0001, n_iter=50, penalty='l1', loss='modified_huber'),
    #                 ('rocchio', NearestCentroid())),
    #                 ('gaussianNB', GaussianNB()),
    #                 ('multinomialNB', MultinomialNB(alpha=.01)),
    #                 ('bernoullyNB', BernoulliNB(alpha=.01))],
    #"classifiers": (('svc', LinearSVC(class_weight={1:33, 0:1})),
    #               ('regression', LogisticRegression(class_weight={1:33, 0:1}))),
    "show_conf_matrix": False,
    "show_scoring_matrix": True,
    "num_attr": [1300],
    "num_folds": num_folds,
    "bootstrap": 1,
    "eval_func": ("lift", {'pos_label': 1}),
    "save_model": True,
    "save_data": True,
    "data_file": "train_info_escenario34_mig.data"
}

cls_test_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    "data_path": "../../data/peru-calls/final_dataset",
    # train_src_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ("test_info_escenario34_mig.semifinal.sw", "test_meta_escenario34_mig.csv"),
    # src_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ['text']}),
                          (read_csv, {'delimiter': '\t','target': 'downgrade',
                                      'selected_attr': meta_feature_names})),
    "src_transformer": ('tfidf','minmax'),
    "score_func": 'f_classif',
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    "src_feature_names": (None, meta_feature_names),
    "classifier": 'regression',
    "show_conf_matrix": False,
    "show_scoring_matrix": False,
    "eval_func": ("roc", {'pos_label': 1}),
    "save_data": True,
    "data_file": "test_info_escenario34_mig.data"
}

wordcloud_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    # Paths relative to the visualization module
    "data_path": "../../data/peru-calls/",
    # Source_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ["customer_downgrades.txt", "customer_not_downgrades.txt",
                        "agent_downgrades.txt", "agent_not_downgrades.txt",
                        "all_downgrades.txt", "all_not_downgrades.txt"],
    # Multi-Scr is "enumerate" (generates individual wordclouds for sources) or "compare" (substracts words from
    # two sources, in both ways, and put the results in two wordclouds)
    "multi_src": WordCloud.MULTISOURCE_COMPARE,
    #"multi_src": WordCloud.MULTISOURCE_ENUMERATE,
    "output_path": "../../data/peru-calls/",
    # source_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": [(read_txt, {'separator': '\t', 'selected_attr': ["text"]}),
                          (read_txt, {'separator': '\t', 'selected_attr': ["text"]}),
                          (read_txt, {'separator': '\t', 'selected_attr': ["text"]}),
                          (read_txt, {'separator': '\t', 'selected_attr': ["text"]}),
                          (read_txt, {'separator': '\t', 'selected_attr': ["text"]}),
                          (read_txt, {'separator': '\t', 'selected_attr': ["text"]})],
    "stopwords_lang": "spanish",
    "stopwords_files": ["crm", "numbers"],
    "num_tags": 100,
    "min_tag_size": 12,
    "max_tag_size": 85,
    "image_size": (750, 750),
    # Fonts: ['Nobile', 'Old Standard TT', 'Cantarell', 'Reenie Beanie', 'Cuprum', 'Molengo', 'Neucha', 'Philosopher', 'Yanone Kaffeesatz', 'Cardo', 'Neuton', 'Inconsolata', 'Crimson Text', 'Josefin Sans', 'Droid Sans', 'Lobster', 'IM Fell DW Pica', 'Vollkorn', 'Tangerine', 'Coustard', 'PT Sans Regular']
    "font": "Nobile",
    "background": (0, 0, 0),
    # This to generate statistics on the most frequent terms
    "max_freq_terms": 20
}


def get_cls_train_cfg():
    return cls_train_cfg


def get_cls_test_cfg():
    return cls_test_cfg


def get_wordcloud_cfg():
    return wordcloud_cfg

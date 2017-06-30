#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import os
import logging
from datasets.peru import CRMDataset
from visualizations.wordcloud import WordCloud

from utils.io_tools import read_csv, read_txt

from sklearn.feature_extraction.text import CountVectorizer
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
from sklearn import tree
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

traditional_feature_names = ('mb_mes','mb_perct','cxd_mes','dias_atraso','arpu','cant_recargas',
                 'frecuencia','cxr_mes','cambio_plan','cf_tot','sub_fact','valor_paquete','fact_tot','cf_voz',
                 'cf_datos','cf_min_adi','cf_min_adi_ldn','cf_min_adi_ldi','cf_adi_sms','adi_datos','valor_paq_fam',
                 'valor_paq_datos','valor_finan','valor_mora','contactos','cont_mov','cont_cla','cont_nex','cont_fij',
                 'cont_mov_top','cont_cla_top','cont_nex_top','cont_fij_top','d_rank1','d_rank2','d_rank3','d_rank5',
                 'w_rank1','w_rank2','w_rank3','w_rank5','dir_pro','dir_top3_pro','n_rank1','n_rank2','n_rank3',
                 'n_rank5','n_comu','n_comu_reg','airtime_tot_out','per_airt_tot_out','airtime_out_mvstr',
                 'per_airt_mov_out','airtime_out_rpm','per_airt_rpm_out','airtime_out_omov','per_airt_omov_out',
                 'airtime_out_fix','per_airt_fix_out','airtime_out_ldi','per_airt_ldi_out','sms_tot','per_sms_tot',
                 'sms_mvstr','per_sms_mov','sms_omov','per_sms_omov','call_out','airtime_in_tot','per_airt_tot_in',
                 'airtime_in_omov','per_airt_omov_in','airtime_in_ldi','per_airt_ldi_in','airtime_in_week',
                 'per_airt_week_in','airtime_in_weekd','per_airt_weekd_in','airtime_in_18_23','per_airt_18_23_in',
                 'airtime_in_23_06','per_airt_23_06_in','airtime_in_06_18','per_airt_06_18_in','plancd','nro_averias',
                 'p_nro_averias','renta_mensual','mb_plan','min_mov_fij','min_mov_ldi','min_mov_mov','min_omov',
                 'consumo_datos','consumo_min_fix','consumo_min_ldi','consumo_min_mov','consumo_omov')

num_folds = 2
param_regression = {'C': np.logspace(-4, 4, 3),
                    'penalty': ('l1', 'l2')}
cls_train_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    # Path relative to the learn module
    "data_path": "../../data/peru-calls/hybrid_dataset",
    "train_src_names": ("train_info_escenario34_mig.semifinal.sw",
                        "train_meta_escenario34_mig.csv",
                        "train_esc4_cos2_data_mig_ord.txt"),
    # src_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ['text']}),
                          (read_csv, {'delimiter': '\t', 'target': 'downgrade',
                                      'selected_attr': meta_feature_names}),
                          (read_csv, {'delimiter': '|', 'selected_attr': traditional_feature_names})),
    "src_transformer": (('tfidf',CountVectorizer(min_df=10, ngram_range=(1, 1))),
                        ('meta_minmax', MinMaxScaler()),
                        ('traditional_minmax', MinMaxScaler())),
    "score_funcs": [('f_classif', SelectKBest(score_func=f_classif)), ('chi2', SelectKBest(score_func=chi2))],
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    "src_feature_names": (None, meta_feature_names, traditional_feature_names),
    "classifiers": [#('regression', LogisticRegression(class_weight='auto')),
                    ('ada', AdaBoostClassifier(n_estimators=200))#,
                    #('tree', tree.ExtraTreeClassifier())
                    ],
    "show_conf_matrix": False,
    "show_scoring_matrix": True,
    "num_attr": [30, 40, 50, 60, 70],
    "num_folds": num_folds,
    "bootstrap": 1,
    "eval_func": ("roc", {'pos_label': 1}),
    "save_model": True,
    "save_data": False
}

cls_test_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    "data_path": "../../data/peru-calls/hybrid_dataset",
    # train_src_names contains file names of each source. They are expected to be into data_path dir
    "train_src_names": ("test_info_escenario34_mig.semifinal.sw", "test_meta_escenario34_mig.csv",
                        "test_esc4_cos2_data_mig_ord.txt"),
    # src_load_function contains in 0 the function to load the source, in 1 : **kwargs
    "src_load_function": ((read_txt, {'delimiter': '\t', 'selected_attr': ['text']}),
                          (read_csv, {'delimiter': '\t','target': 'downgrade',
                                      'selected_attr': meta_feature_names}),
                          (read_csv, {'delimiter': '|', 'selected_attr': traditional_feature_names})),
    "src_transformer": ('tfidf', 'meta_minmax', 'traditional_minmax'),
    "score_func": 'f_classif',
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    "src_feature_names": (None, meta_feature_names, traditional_feature_names),
    "classifier": 'ada',
    "show_conf_matrix": True,
    "show_scoring_matrix": True,
    "eval_func": ("gain", {'pos_label': 1}),
    "save_data": True,
    "data_file": "test_info_escenario34_mig.data"
}

wordcloud_cfg = {
    "dataset_class": CRMDataset,
    "encoding": "UTF-8",
    # Paths relative to the visualization module
    "data_path": "../../data/peru-calls/",
    # source_names contains file names of each source. They are expected to be into data_path dir
    #"train_src_names": ["customer_downgrades.txt"],
    #"train_src_names": ["customer_downgrades.txt", "customer_not_downgrades.txt", "agent_downgrades.txt", "agent_not_downgrades.txt", "all_downgrades.txt", "all_not_downgrades.txt"],
    "train_src_names": ["all_downgrades.txt", "all_not_downgrades.txt"],
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
    "background": (0, 0, 0)
}


def get_cls_train_cfg():
    return cls_train_cfg


def get_cls_test_cfg():
    return cls_test_cfg


def get_wordcloud_cfg():
    return wordcloud_cfg

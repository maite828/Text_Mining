#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import os
import logging
from datasets.peru import CRMDataset

from utils.io_tools import read_csv, read_txt
from preprocessing.transformers import GetItemTransformer

from sklearn.linear_model import LogisticRegression
from sklearn.grid_search import GridSearchCV
from sklearn.feature_selection import f_classif, chi2, f_regression
from sklearn.feature_selection import SelectKBest
import numpy as np

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')

try:
    from preprocessing.text.lemmatizer import LemmaTokenizer
    tokenizer = LemmaTokenizer('es')
except ImportError: 
    logging.warning('Error loading lemmatizer module.')
    logging.warning('Lemmatization wont be applied to %s configuration.' % os.path.basename(__file__))
    tokenizer = None


feature_names = ('mb_mes','mb_perct','cxd_mes','dias_atraso','arpu','cant_recargas',
                 'frecuencia','cxr_mes','cambio_plan','cf_tot','sub_fact','valor_paquete','fact_tot','cf_voz',
                 'cf_min_adi','cf_min_adi_ldi','cf_adi_sms','adi_datos',
                 'valor_paq_datos','valor_finan','valor_mora','contactos','cont_mov',
                 'cont_mov_top','d_rank1','d_rank2','d_rank3','d_rank5',
                 'w_rank1','w_rank2','w_rank3','w_rank5','dir_pro','dir_top3_pro','n_rank1','n_rank2','n_rank3',
                 'n_rank5','n_comu','n_comu_reg','airtime_tot_out','per_airt_tot_out','airtime_out_mvstr',
                 'per_airt_mov_out','airtime_out_rpm','per_airt_rpm_out','airtime_out_omov','per_airt_omov_out',
                 'airtime_out_fix','per_airt_fix_out','airtime_out_ldi','per_airt_ldi_out','sms_tot','per_sms_tot',
                 'sms_mvstr','per_sms_mov','sms_omov','per_sms_omov','call_out','airtime_in_tot','per_airt_tot_in',
                 'airtime_in_omov','per_airt_omov_in','airtime_in_week',
                 'per_airt_week_in','airtime_in_weekd','per_airt_weekd_in','airtime_in_18_23','per_airt_18_23_in',
                 'airtime_in_23_06','per_airt_23_06_in','airtime_in_06_18','per_airt_06_18_in','plancd','nro_averias',
                 )


num_folds = 2
param_regression = {'C': np.logspace(-4, 4, 3),
                    'penalty': ('l1', 'l2')}
cls_train_cfg = {
    'dataset_class': CRMDataset,
    'encoding': 'UTF-8',
    # Path relative to the learn module
    'data_path': '../../data/peru-calls/traditional_dataset',
    # train_src_names contains file names of each source. They are expected to be into data_path dir
    'train_src_names': ['train_esc4_cos2_data_mig_ord.txt'],
    # src_load_function contains in 0 the function to load the source, in 1 : **kwargs
    'src_load_function': [(read_csv, {'delimiter': '|', 'target': 'target', 'selected_attr': feature_names})],
    'src_transformer': [('idem', GetItemTransformer())],
    'score_funcs': [('f_regression', SelectKBest(score_func=f_classif))],#, ('chi2', SelectKBest(score_func=chi2))],
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    'src_feature_names': [feature_names],
    #'classifiers': [('regression', LogisticRegression(class_weight='auto'))],
    'classifiers': [('regression', GridSearchCV(LogisticRegression(class_weight='auto'), refit=True,
                                                param_grid=param_regression, scoring='f1', n_jobs=-1, verbose=5))],
    'show_conf_matrix': False,
    'show_scoring_matrix': True,
    'num_attr': [23],
    'num_folds': num_folds,
    'bootstrap': 1,
    'eval_func': ('gain', {'pos_label': 1}),
    'save_model': True,
    'save_data': False
}

cls_test_cfg = {
    'dataset_class': CRMDataset,
    'encoding': 'UTF-8',
    'data_path': '../../data/peru-calls/traditional_dataset',
    # train_src_names contains file names of each source. They are expected to be into data_path dir
    'train_src_names': ['test_esc4_cos2_data_mig_ord.txt'],
    # src_load_function contains in 0 the function to load the source, in 1 : **kwargs
    'src_load_function': [(read_csv, {'delimiter': '|','target': 'target', 'selected_attr': feature_names})],
    'src_transformer': ['idem'],
    'score_func': 'f_regression',
    # src_features_names is used for:
    # if the transformer applied to this source does not have get_feature_names function then as feature names
    'src_feature_names': [feature_names],
    'classifier': 'regression',
    'show_conf_matrix': True,
    'show_scoring_matrix': True,
    'eval_func': ('gain', {'pos_label': 1}),
    'save_data': False
}


def get_cls_train_cfg():
    return cls_train_cfg


def get_cls_test_cfg():
    return cls_test_cfg

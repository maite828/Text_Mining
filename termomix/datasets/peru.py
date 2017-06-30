#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

""""
Dataset representation for CRM transcriptions -  Peru
"""
import fnmatch
import codecs
from time import time
import csv
from os.path import join, isdir, splitext
from os import listdir
import logging
import sys
from datasets.base_dataset import BaseDataset
from utils.io_tools import split_train_dev_test, read_csv
from utils.miscellaneous import generate_n_primes
from preprocessing.sampling import StratifiedSamplingKFold
from sklearn.cross_validation import StratifiedKFold

from preprocessing.text.text_transformations import replace_numbers, get_words
from preprocessing.text.stopwords import StopWords
from utils.miscellanea import get_words_count, subtract_words_count

import xml.etree.ElementTree as etree

import numpy as np

DEFAULT_ENCODING = 'UTF-8'
TEST_SIZE = 0.33
RANDOM_STATE = 42

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')


class CRMDataset(BaseDataset):

    def __init__(self, configuration):
        BaseDataset.__init__(self, configuration)


    def load_dataset(self):
        BaseDataset.load_dataset(self)
        if 'train_src_names' in self.configuration:
            self.y_train = np.asarray(self.y_train, dtype=np.float)
        elif 'test_src_names' in self.configuration:
            self.y_test = np.asarray(self.y_test, dtype=np.float)


    def get_fold(self):
        if 'bootstrap' in self.configuration and self.configuration['bootstrap'] > 1:
            n_bootstrap = self.configuration['bootstrap']
            for random in generate_n_primes(n_bootstrap):
                yield StratifiedSamplingKFold(self.y_train, n_folds=self.configuration['num_folds'],
                                              method="undersample", scaling=1.0, shuffle=False, random_state=random)
        else:
            yield StratifiedKFold(self.y_train, n_folds=self.configuration['num_folds'], random_state=42)


    @staticmethod
    def load_text_from_info(configuration):
        """
        Loads transcriptions from files.
        This function is not used currently. See hive table and queries
        """
        logging.info('Load transcriptions from info files ...')
        ofile = codecs.open(join(configuration['data_path'], configuration['source_names'][0]), 'w', DEFAULT_ENCODING)
        for subdir in listdir(configuration['data_path']):
            subdir_path = join(configuration['data_path'], subdir)
            logging.info('     Processing: ' + subdir_path)
            if isdir(subdir_path):
                for ifile in listdir(subdir_path):
                    if ifile != u'.DS_Store' and fnmatch.fnmatch(ifile, '*.info'):
                        id = splitext(ifile)[0]
                        text = ''
                        with codecs.open(join(subdir_path, ifile), encoding=DEFAULT_ENCODING, mode='rb') as process_file:
                            for line in process_file:
                                text += id + '\t' + line
                            if text != '':
                                ofile.write(text)
        ofile.close()

    @staticmethod
    def load_meta_from_xml(configuration):
        """
        Load metada from xml files and write to csv
        """
        logging.info('Load meta transcriptions from meta files ...')
        ofile = codecs.open(join(configuration['data_path'], configuration['source_names'][1]), 'w', DEFAULT_ENCODING)

        for subdir in listdir(configuration['data_path']):
            subdir_path = join(configuration['data_path'], subdir)
            logging.info('     Processing: ' + subdir_path)
            if isdir(subdir_path):
                for ifile in listdir(subdir_path):
                    if ifile != u'.DS_Store' and fnmatch.fnmatch(ifile, '*.meta'):
                        attr_dict = {}
                        id = splitext(ifile)[0]
                        attr_dict['id'] = id
                        #parser = etree.XMLParser(encoding="utf-8")
                        parser = etree.XMLParser()
                        with open(join(subdir_path, ifile)) as process_file:
                            doc = etree.parse(process_file, parser)
                            # tag 'CAudioFile'
                            root = doc.getroot()
                            for child in root:
                                if child.tag == 'CRI':
                                    for sub_child in child:
                                        if sub_child.tag != 'PrivateData':
                                            attr_dict[sub_child.tag] = (sub_child.text != None
                                                                        and sub_child.text != '-1'
                                                                        and sub_child.text) or '0'
                                        else:
                                            private_data = sub_child.find('PrivateData')
                                            for dict_entry in private_data:
                                                attr_dict[dict_entry.find('Key').text] = \
                                                    (dict_entry.find('Value').text != None
                                                     and dict_entry.find('Value').text != '-1'
                                                     and dict_entry.find('Value').text) or '0'
                                elif child.tag == 'Rule':
                                    target = child.find('RuleInfos/RuleInfo/Name').text
                                    attr_dict[child.find('RuleInfos/RuleInfo').tag] = (target != None and target != ''
                                                                                       and target) or 'UNKNOWN'
                                else:
                                    pass

                            instance_values = []
                            for attr in configuration['source_feature_names'][1]:
                                if attr in attr_dict:
                                    instance_values.append(attr_dict[attr])
                                else:
                                    instance_values.append('0')
                            ofile.write('\t'.join(instance_values) + u'\n')
        ofile.close()


    @staticmethod
    def process_text(text, stopwords_remover):
        output = None
        if stopwords_remover is not None:
            output = stopwords_remover.remove_stopwords(text, as_string=True, remove_punctuation=False, lowercase=True)
        else:
            logging.info("Stop words removal is not configured: skipping")
        return output

    @staticmethod
    def process_stopwords(configuration):
        logging.debug('Processing stopwords')

        # Initialize stopwords remover class
        conf_language = configuration['stopwords_lang']

        conf_stopwords_files = configuration['stopwords_files']
        stopwords_remover = StopWords(conf_language, conf_stopwords_files)
        t0 = time()


        X_src = []
        selected_attr = configuration['selected_attr']
        delimiter = configuration['delimiter'] if 'delimiter' in configuration else '\t'
        ofile = codecs.open(join(configuration['data_path'], configuration['input_name'])+'.sw', 'w')
        ofile.write(selected_attr + '\n')
        f = codecs.open(join(configuration['data_path'], configuration['input_name']), mode='rb', encoding='utf-8')
        f.next()
        for line in f:
            text = line.split(delimiter)[3]
            o_text = CRMDataset.process_text(text, stopwords_remover)
            if o_text == '':
                o_text = 'all_stopwords'
            ofile.write(o_text.encode("utf-8") + '\n')

        logging.debug(" done in %fs" % (time() - t0))


PRIVATE_DATE_PATH = 'CRI/PrivateData/PrivateData/DictionaryEntry/'


def main(argv):
    # This main is only for loading original files from the cluster. Ignore it
    meta_features_names = ["id", "trans_datetime", "down_date", "AgentID", "ANI", "CD7", "CD2", "CD5", "CD16", "CD17",
                           "CD18", "CD23", "Duration", "NumberOfHolds", "NumberOfConferences", "NumberOfTransfers",
                           "TotalHoldTime", "WrapUpTime", "TOTAL_AGENT_TALK_TIME", "NUM_AGENT_TALK_SEGMENTS",
                           "TOTAL_CUSTOMER_TALK_TIME", "NUM_CUSTOMER_TALK_SEGMENTS", "TOTAL_SILENCE_TIME",
                           "NUM_SILENCE_SEGMENTS", "TOTAL_TALK_OVER_TIME", "NUM_TALK_OVER_SEGMENTS",
                           "NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS", "NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS"]

    # meta_features_names = ["Duration", "NumberOfHolds", "NumberOfConferences", "NumberOfTransfers",
    #                        "TotalHoldTime", "WrapUpTime", "TOTAL_AGENT_TALK_TIME", "NUM_AGENT_TALK_SEGMENTS",
    #                        "TOTAL_CUSTOMER_TALK_TIME", "NUM_CUSTOMER_TALK_SEGMENTS", "TOTAL_SILENCE_TIME",
    #                        "NUM_SILENCE_SEGMENTS", "TOTAL_TALK_OVER_TIME", "NUM_TALK_OVER_SEGMENTS",
    #                        "NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS", "NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS"]

    configuration = {"encoding":"UTF-8", "data_path":"/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201403",
                     "train_src_names": ["info_escenario34.semifinal", "meta_escenario34.csv"],
                     "src_feature_names":[["text"], meta_features_names],
                    }
    #CRMDataset.load_meta_from_xml(configuration)
    #CRMDataset.load_text_from_info(configuration)
    #configuration['data_path'] = '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201404'
    #CRMDataset.load_meta_from_xml(configuration)
    #CRMDataset.load_text_from_info(configuration)
    #configuration['data_path'] = '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201405'
    #CRMDataset.load_meta_from_xml(configuration)
    #CRMDataset.load_text_from_info(configuration)


    #Split original file into train, dev and test:
    data_path = '/Users/zoraida/Analytics/analytics-text-mining/data/peru-calls/final_dataset'

    # Load Meta first because contains target
    X, y = [], []
    input_file = join(data_path, configuration['train_src_names'][1])
    kwargs = {'delimiter': '\t', 'selected_attr': meta_features_names, 'target': 'downgrade'}

    read_csv(X, y, input_file, **kwargs)

    train_file = join(data_path, 'train_' + configuration['train_src_names'][1])
    #dev_file = join(data_path, 'dev_' + configuration['train_src_names'][1])
    dev_file = None
    test_file = join(data_path, 'test_' + configuration['train_src_names'][1])
    split_train_dev_test(X[0], y, train_file, dev_file, test_file, test_size=0.2, random_state=1234, **kwargs)

    # Text
    X= []
    input_file = join(data_path, configuration['train_src_names'][0])
    kwargs = {'delimiter': '\t', 'selected_attr': ['id', 'cd2', 'downgrade', 'text']}

    read_csv(X, y, input_file, **kwargs)
    train_file = join(data_path, 'train_' + configuration['train_src_names'][0])
    #dev_file = join(data_path, 'dev_' + configuration['train_src_names'][0])
    dev_file = None
    test_file = join(data_path, 'test_' + configuration['train_src_names'][0])

    split_train_dev_test(X[0], y, train_file, dev_file, test_file, test_size=0.2, random_state=1234, **kwargs)


def train_test_susana():
    selected_attr = ['suscriptioncd', 'numerotelefono', 'call_id', 'fec_alta', 'fec_down', 'tipo', 'tipo_fin', 'ant_linea',
                     'tiempo_down', 'tipo_down', 'mb_mes', 'mb_perct', 'cxd_mes', 'dias_atraso',
                     'arpu', 'cant_recargas', 'frecuencia', 'cxr_mes', 'cambio_plan', 'cf_tot', 'sub_fact',
                    'valor_paquete', 'fact_tot', 'cf_voz', 'cf_datos', 'cf_min_adi', 'cf_min_adi_ldn',
                    'cf_min_adi_ldi', 'cf_adi_sms','adi_datos', 'valor_paq_fam', 'valor_paq_datos', 'valor_finan',
                    'valor_mora','contactos',
                    'cont_mov','cont_cla','cont_nex','cont_fij','cont_mov_top','cont_cla_top','cont_nex_top',
                    'cont_fij_top','d_rank1','d_rank2','d_rank3','d_rank5','w_rank1','w_rank2','w_rank3','w_rank5',
                    'dir_pro','dir_top3_pro','n_rank1','n_rank2','n_rank3','n_rank5','n_comu','n_comu_reg',
                    'airtime_tot_out','per_airt_tot_out','airtime_out_mvstr','per_airt_mov_out','airtime_out_rpm',
                    'per_airt_rpm_out','airtime_out_omov','per_airt_omov_out','airtime_out_fix','per_airt_fix_out',
                    'airtime_out_ldi','per_airt_ldi_out','sms_tot','per_sms_tot','sms_mvstr','per_sms_mov','sms_omov',
                    'per_sms_omov','call_out','airtime_in_tot','per_airt_tot_in','airtime_in_omov','per_airt_omov_in',
                    'airtime_in_ldi','per_airt_ldi_in','airtime_in_week','per_airt_week_in','airtime_in_weekd',
                    'per_airt_weekd_in','airtime_in_18_23','per_airt_18_23_in','airtime_in_23_06','per_airt_23_06_in',
                    'airtime_in_06_18','per_airt_06_18_in','plancd','nro_averias','p_nro_averias','renta_mensual',
                    'mb_plan','min_mov_fij','min_mov_ldi','min_mov_mov','min_omov','consumo_datos','consumo_min_fix',
                    'consumo_min_ldi','consumo_min_mov','consumo_omov']
    X, y = [], []

    file_names = ['esc2_cos3_data_all_ord.txt', 'esc2_cos3_data_mig_ord.txt',
                  'esc4_cos2_data_all_ord.txt', 'esc4_cos2_data_mig_ord.txt']

    data_path = '/Users/zoraida/Desktop'
    kwargs = {'delimiter': '|', 'selected_attr': selected_attr, 'target': 'target'}

    input_file = join(data_path, file_names[3])
    read_csv(X, y, input_file, **kwargs)
    train_file = join(data_path, 'train_' + file_names[3])
    dev_file = None
    test_file = join(data_path, 'test_' + file_names[3])

    split_train_dev_test(X[0], y, train_file, dev_file, test_file, test_size=0.2, random_state=1234, **kwargs)

if __name__ == '__main__':
    #train_test_susana()
    #main(sys.argv[1:])
    configuration = { "stopwords_lang": "spanish",
                       "stopwords_files": ["crm", "numbers", "proper_names"],
                       "selected_attr": "text",
                       "delimiter": "\t",
                       "data_path": "/Users/zoraida/Analytics/analytics-text-mining/data/peru-calls/final_dataset",
                       "input_name": "train_info_escenario34.semifinal"}
    CRMDataset.process_stopwords(configuration)

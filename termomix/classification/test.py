#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

"""
 Main class for testing.
"""

import argparse
import sys
import logging
import importlib
from os.path import join

from utils.miscellaneous import load_objects
from utils.io_tools import write_csv
from common import features_selection, evaluate, process_transformers


logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(message)s')


def load_configuration(cfg_name):
    """
    Loads the configuration for a given dataset as a dictionary:
    transformers, classifiers, score_funcs, etc
    """
    module_name = 'configuration.' + cfg_name
    config_module = importlib.import_module(module_name)
    return config_module.get_cls_test_cfg()


def save_data_transformed(X, y, X_trunc_names, configuration):
    """
    if save_data then we save X and y into file
    :param X: X already transformerd and truncated
    :param y: target
    :param configuration: configuration file
    :return:
    """
    save_data = configuration['save_data'] if 'save_data' in configuration else False
    if save_data:
        write_csv(X, y,
                  join(configuration['data_path'], configuration['data_file'] if 'data_file' in configuration
                  else 'data.csv'),
                  **{'delimiter': configuration['delimiter'] if 'delimiter' in configuration else '\t',
                     'selected_attr': X_trunc_names})

def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("dataset_name", help="Dataset. Options are: [twitter|webcrawl|peru|lleida]")
    args = parser.parse_args()

    # Get the configuration module according to the dataset
    configuration = load_configuration(args.dataset_name + '_cfg')
    # Create and load the dataset based on its configuration
    dataset = configuration['dataset_class'](configuration)
    dataset.load_dataset()

    transformers = zip(dataset.configuration['src_transformer'],
                       load_objects(dataset.configuration['src_transformer'],
                                    join(dataset.configuration['data_path'],'model')))
    score_func = load_objects([dataset.configuration['score_func']],
                                  join(dataset.configuration['data_path'],'model'))
    classifier = load_objects([dataset.configuration['classifier']],
                                  join(dataset.configuration['data_path'],'model'))

    show_conf_matrix = ('show_conf_matrix' in configuration and configuration['show_conf_matrix']) or False

    _, X_test, _, y_test, X_names = process_transformers(dataset, transformers, None, range(0, dataset.n_instances))
    _, X_trunc_test, X_trunc_names = features_selection(score_func[0], None, X_names, X_test, None, None)

    # if save_data then we save X_trunc_test and y_test into file
    save_data_transformed(X_trunc_test, y_test, X_trunc_names, configuration)

    score = evaluate(classifier[0], X_trunc_test, y_test, None, None, show_conf_matrix,
                     dataset.get_evaluation_function(), **dataset.get_evaluation_function_args())
    print score


if __name__ == '__main__':
    main(sys.argv[1:])
#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

"""
 Main class for training. Trains a model for an specific dataset base on the configuration.
 Receives as arg the name of the dataset: see load_configuration function to understand how to define
 a configuration file
"""

import argparse
import sys
import logging
import importlib
from os.path import join

import numpy as np
from sklearn.feature_selection import SelectKBest

from utils.miscellanea import persist_objects
from utils.io_tools import write_csv
from common import features_selection, evaluate, process_transformers

from visualizations.metrics_evaluation import plot_score

DEFAULT_FOLDS = 2


logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(message)s')


def load_configuration(cfg_name):
    """
    Loads the configuration for a given dataset as a dictionary:
    transformers, classifiers, score_funcs, etc
    """
    module_name = 'configuration.' + cfg_name
    config_module = importlib.import_module(module_name)
    return config_module.get_cls_train_cfg()

def save_data_transformed(X, y, feature_names, **kwargs):
    """
    if save_data then we save X and y into file
    :param X: X already transformerd and truncated
    :param y: target
    :param name_attr: attr names
    :param configuration: configuration file
    :return:
    """
    save_data = kwargs['save_data'] if 'save_data' in kwargs else False
    if save_data:
        write_csv(X, y,
                  join(kwargs['data_path'], kwargs['data_file'] if 'data_file' in kwargs
                  else 'data.csv'),
                  **{'delimiter': kwargs['delimiter'] if 'delimiter' in kwargs else '\t',
                     'selected_attr': feature_names})


def refit_and_save_model(dataset, transformer_list, score_func_list, num_attr_list, classifier_list, scoring, **kwargs):
    """
    This function fits transformers, best score func and best classifier on dataset.X_train
    and persist into different files: transformers, best score function and best classifier.
    File names are of the form name + '. pkl' where name is taken from the configuration.
    :param dataset: dataset
    :param transformers: list of tuples ('transformer_name', transformer object)
    :param score_funcs: list of tuples ('score_func_name', score_func object)
    :param num_attr: list with number of attributes
    :param classifiers: list of tuples: ('cls_name', classifier objects)
    :param scoring: array with shape(len(classifiers), len(num_attr), len(score_funcs)). Each positions has the score
    for each tuple (classifier, num_attr, score_func)
    :return: None
    """
    i, j, z = np.unravel_index(scoring.argmax(), scoring.shape)
    # fit and persist transformers
    #X_test, y_test == None
    X_train, _, y_train, _, X_names = process_transformers(dataset, transformer_list,
                                                                     k_train=range(0, dataset.n_instances))
    persist_objects(zip(*transformer_list)[0], zip(*transformer_list)[1], join(dataset.configuration['data_path'], 'model'))

    # fit and persist score func
    X_trunc_train, X_trunc_test, X_trunc_names = features_selection(score_func_list[z][1], num_attr_list[j], X_names, X_test=None,
                                                     X_train=X_train, y_train=y_train)
    persist_objects(zip(*[score_func_list[z]])[0], zip(*[score_func_list[z]])[1], join(dataset.configuration['data_path'], 'model'))

    save_data_transformed(X_trunc_train, y_train, X_trunc_names, **kwargs)

    # fit and persist classifier
    evaluate(classifier_list[i][1], X_test=None, y_test=None, X_train=X_trunc_train, y_train=y_train)
    persist_objects(zip(*[classifier_list[i]])[0], zip(*[classifier_list[i]])[1], join(dataset.configuration['data_path'], 'model'))


def process_results(dataset, transformer_list, score_func_list, num_attr_list, classifier_list, scoring, **kwargs):
    show_scoring_matrix = kwargs['show_scoring_matrix'] if 'show_scoring_matrix' in kwargs else False
    save_model = kwargs['save_model'] if 'save_model' in kwargs else False
    if save_model:
        refit_and_save_model(dataset, transformer_list, score_func_list, num_attr_list, classifier_list, scoring,
                             **kwargs)
    # Print out scoring matrix (one matrix per score_function)
    for k in range(0, len(score_func_list)):
        print " Score function: %22s " % str(score_func_list[k])
        # classifier per row
        for i in range(0, len(classifier_list)):
            print "%22s " % classifier_list[i][0],
            for j in range(0, scoring.shape[1]):
                print "%5.3f" % scoring[i, j, k],
            print
        if show_scoring_matrix:
            plot_score(scoring[:, :, k], num_attr_list, zip(*classifier_list)[0], score_func_list[k][1].score_func.__name__,
                 dataset.get_evaluation_function())


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("dataset_name", help="Dataset. Options are: [twitter|webcrawl|peru|lleida]")
    args = parser.parse_args()

    # Get the configuration module according to the dataset
    configuration = load_configuration(args.dataset_name + '_cfg')
    # Create and load the dataset based on its configuration
    dataset = configuration['dataset_class'](configuration)
    dataset.load_dataset()

    transformers = configuration['src_transformer']
    score_funcs = configuration['score_funcs']
    classifiers = configuration['classifiers']
    num_folds = ('num_folds' in configuration and configuration['num_folds']) or DEFAULT_FOLDS
    show_conf_matrix = ('show_conf_matrix' in configuration and configuration['show_conf_matrix']) or False

    num_attr = configuration['num_attr'] if 'num_attr' in configuration else []

    # Matrix to display accuracies (rows are classifiers and columns #of features used to train
    scoring = None
    for idx, kfold in enumerate(dataset.get_fold()):
        for k_train, k_test in kfold:
            X_train, X_test, y_train, y_test, X_names = process_transformers(dataset, transformers, k_train, k_test)

            # If there are elements in num_attr greater than the features in the dataset, use the number of features
            if len(num_attr) > 0:
                num_attr = list(set([min(elem, X_train.shape[1]) for elem in num_attr]))
                num_attr.sort()
            else:
                num_attr.append(X_train.shape[1])

            if scoring == None:
                scoring = np.zeros((len(classifiers), len(num_attr), len(score_funcs)))

            # For each num_features, for each feature selector function, for each classifier: evaluate
            for k in range(0, len(score_funcs)):
                for j in range(0, len(num_attr)):
                    X_trunc_train, X_trunc_test, X_trunc_names = features_selection(score_funcs[k][1], num_attr[j],
                                                                                    X_names, X_test, X_train, y_train)

                    print('_' * 80)
                    print 'Evaluating classifiers:'
                    for i in range(0, len(classifiers)):
                        print("Classifier: %s (Num. features: %d) (Score funct: %s) ..."
                              % (classifiers[i][0], num_attr[j], score_funcs[k][1].score_func.__name__))

                        score = evaluate(classifiers[i][1], X_trunc_test, y_test, X_trunc_train, y_train,
                                         show_conf_matrix, dataset.get_evaluation_function(),
                                         **dataset.get_evaluation_function_args())
                        # score for test fold
                        scoring[i, j, k] += score

    # idx + 1 == bootstrap
    process_results(dataset, transformers, score_funcs, num_attr, classifiers, np.divide(scoring, (idx + 1) * num_folds),
                    **configuration)


if __name__ == '__main__':
    main(sys.argv[1:])
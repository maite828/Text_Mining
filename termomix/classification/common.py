#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

from time import time

import numpy as np

import logging

from scipy import sparse

from sklearn.svm import SVC

from sklearn.metrics import confusion_matrix, accuracy_score, f1_score
from visualizations.metrics_evaluation import show_confusion_matrix, plot_roc, plot_cum_gain, plot_lift

from sklearn.feature_selection import f_regression

from sklearn.preprocessing import LabelEncoder

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')


def process_transformers(dataset, transformers, k_train=None, k_test=None):
    """
    :param dataset:
    :param transformers: transformers to be applied to different sources of the dataset
    :param k_train: indexes of train instances. Can be None
    :param k_test: indexes of test instances. Can be None
    :return: X_train, X_test, y_train, y_test, X_names(feature names)
    """
    # fit and transform train
    y_train = None
    X_train = None
    X_names = np.array([])
    if k_train is not None:
        y_train = np.array([dataset.y_train[idx_train] for idx_train in k_train])
        X_train = np.array([])

        for src, transformer in enumerate(t[1] for t in transformers):
            # subset of instances(train and test) are built for the current source
            X_src_train = [dataset.X_train[src][idx_train] for idx_train in k_train]

            # each subset is transformed according to each transformer
            X_src_train, _ = feature_transformation(transformer, X_test=None, X_train=X_src_train, y_train=y_train)

            # current source is concatenated with previous sources
            if not sparse.issparse(X_src_train):
                X_train = sparse.hstack([X_train, sparse.csr_matrix(X_src_train)])if X_train.size \
                    else sparse.csr_matrix(X_src_train)
            else:
                X_train = sparse.hstack([X_train, X_src_train])if X_train.size else X_src_train

            X_train = X_train.tocsr()

    # transform test
    y_test = None
    X_test = None
    if k_test is not None:
        y_test = np.array([dataset.y_train[idx_test] for idx_test in k_test])
        X_test = np.array([])

        for src, transformer in enumerate(t[1] for t in transformers):
            # subset of instances(train and test) are built for the current source
            X_src_test = [dataset.X_train[src][idx_test]for idx_test in k_test]

            # each subset is transformed according to each transformer
            _, X_src_test = feature_transformation(transformer, X_test=X_src_test, X_train=None, y_train=None)

            # current source is concatenated with previous sources
            if not sparse.issparse(X_src_test):
                X_test = sparse.hstack([X_test, sparse.csr_matrix(X_src_test)])if X_test.size else sparse.csr_matrix(X_src_test)
            else:
                X_test = sparse.hstack([X_test, X_src_test])if X_test.size else X_src_test

            X_test = X_test.tocsr()

    # get features names
    for src, transformer in enumerate(t[1] for t in transformers):
        if hasattr(transformer, 'get_feature_names') and callable(getattr(transformer, 'get_feature_names')):
            X_names = np.hstack([X_names, np.array(transformer.get_feature_names())])if X_names.size \
                else np.array(transformer.get_feature_names())
        else:
            X_names = np.hstack([X_names, np.array(dataset.X_names[src])])if X_names.size \
                else np.array(dataset.X_names[src])

    return X_train, X_test, y_train, y_test, X_names


def feature_transformation(transformer, X_test=None, X_train=None, y_train=None):
    """
    Applies a transformer to a given dataset. If X_train and y_train are None
    then it applies only to X_test(it assumes that transformer has been fit previously)
    :param transformer: transformer to be applied
    :param X_test: dataset instances for test
    :param X_train: dataset instances for training. Default None
    :param y_train: target instances for training. Default None
    :return X_train, X_test because transformer.fit_transform does not override args
    """
    logging.info('_' * 80)
    logging.info("Extracting features from dataset using vectorizer: %s" % transformer.__class__.__name__)

    if X_train is not None and y_train is not None:
        logging.info("Transforming training data")
        t0 = time()
        X_train = transformer.fit_transform(X_train, y_train)
        logging.info("n_samples: %d, n_features: %d" % X_train.shape)
        logging.info("done in %fs" % (time() - t0))

    if X_test is not None:
        logging.info("Transforming test data")
        t0 = time()
        X_test = transformer.transform(X_test)
        logging.info("n_samples: %d, n_features: %d" % X_test.shape)
        logging.info("done in %fs" % (time() - t0))

    return X_train, X_test


def features_selection(score_func, k, X_names, X_test=None, X_train=None,  y_train=None):
    """
    Select features according to the k highest scores. If X_train and y_train are None
    then it applies only to X_test(it assumes that score_func has been fit previously)

    :param select_best: SelectKBest object.
    :param k: number of top features to select. int or “all”, optional, default=10
    :param X_names: list of feature names. X_names[i] contains the name for the i feature in X[:][i]
    :param X_test: dataset instances for test. Default None
    :param X_train: dataset instances for training. Default None
    :param y_train: target instances for training. Default None
    """
    logging.info('_' * 80)

    if X_train is not None and y_train is not None:
        logging.info("Extracting %d best features for training data: %s" % (k, score_func.score_func.__name__))
        t0 = time()
        score_func.set_params(**{'k':k})

        if score_func.score_func.func_name == 'f_regression':
            score_func.score_func.func_defaults = (False,)

        X_train = score_func.fit_transform(X_train, y_train)
        logging.info("done in %fs" % (time() - t0))

    if X_test is not None:
        logging.info("Extracting %d best features for testing data: %s" % (score_func.k,
                                                                    score_func.score_func.__name__))
        t0 = time()
        X_test = score_func.transform(X_test)
        logging.info("done in %fs" % (time() - t0))

    #build feature selected names
    X_names_trunc = []
    for ind, selected in enumerate(score_func.get_support()):
        if selected:
            X_names_trunc.append(X_names[ind])


    logging.debug('Features ordered by score: ')
    desc_score_indices = np.argsort(score_func.pvalues_, kind="mergesort")[::-1]
    support = score_func.get_support()
    for idx in desc_score_indices:
        if support[idx]:
            logging.debug('SELECTED: ' + X_names[idx] + ' score: ['
                          + str(score_func.pvalues_[idx]) + ']')
        #else:
        #    logging.debug(X_names[idx] + ' score: ['
        #                  + str(score_func.pvalues_[idx]) + ']')

    return X_train, X_test, X_names_trunc


def compute_sample_weight(classes, y_train):
    # Find the weight of each class as present in y.
    le = LabelEncoder()
    y_ind = le.fit_transform(y_train)
    if not all(np.in1d(classes, le.classes_)):
        raise ValueError("classes should have valid labels that are in y")

    # inversely proportional to the number of samples in the class
    recip_freq = 1. / np.bincount(y_ind)
    weight = recip_freq[le.transform(classes)] / np.mean(recip_freq)
    weight_by_class = dict(zip(le.classes_, weight))
    y_sample_weight = [weight_by_class[e] for e in y_train]
    return y_sample_weight

def evaluate(classifier, X_test, y_test, X_train=None, y_train=None, show_conf_matrix=False,
             eval_func='accuracy', **kwargs):
    """
    :param classifier: classifier to build the model
    :param X_test: dataset instances for test
    :param y_test: target instances for test
    :param X_train: dataset instances for training. Default None
    :param y_train: target instances for training. Default None
    :return: score on target prediction

    """
    score = None
    #sample_weight = None
    if X_train is not None and y_train is not None:

        sample_weight = None#np.asarray(compute_sample_weight(np.unique(y_train), y_train))
        try:
            classifier.fit(X_train.toarray(), y_train, sample_weight=sample_weight)
        except TypeError:
            try:
                classifier.fit(X_train.toarray(), y_train)
            except TypeError:
                try:
                    classifier.fit(X_train, y_train, sample_weight=sample_weight)
                except TypeError:
                    classifier.fit(X_train, y_train)

    if X_test is not None and y_test is not None:
        score = compute_score(classifier, X_test, y_test, eval_func, **kwargs)
        _compute_confusion_matrix(classifier, X_test, y_test, show_conf_matrix)

    return score


def compute_score(model, X_test, y_test, score='accuracy', **kwargs):
    """
    The model is applied to the dataset and score is returned.
    may change base on
    :param model: model to be applied
    :param X: test dataset
    :param y: test dataset target
    :param score: string that currently can be ['accuracy', 'precision_recall']
    :return: score
    """
    try:
        return ({'f1': _compute_f1,
                 'accuracy': _compute_accuracy,
                 'roc': _compute_roc,
                 'gain':_compute_gain,
                 'lift':_compute_lift}[score])(model, X_test, y_test, **kwargs)
    except KeyError:
        logging.error('Unknown score: %s' % score)
        raise


def _compute_confusion_matrix(model, X_test, y_test, show_conf_matrix=False):
    """
    Computes confusion matrix and it is printed on the standar output.
    If show_conf_matrix is true then a coloured graph is shown
    :param model:
    :param X_test:
    :param y_test:
    :param show_conf_matrix:
    :return:
    """
    categories = np.unique(y_test)
    try:
        y_pred = model.predict(X_test.toarray())
    except TypeError:
        y_pred = model.predict(X_test)

    cm = (confusion_matrix(y_test, y_pred, categories))
    logging.info(cm)
    if show_conf_matrix:
        show_confusion_matrix(cm, categories)


def _compute_accuracy(model, X_test, y_test, **kwargs):
    """
    Computes accuracy
    :param model:
    :param X_test: test dataset
    :param y_test: test dataset target
    :return: accuracy
    """
    try:
        y_pred = model.predict(X_test.toarray())
    except TypeError:
        y_pred = model.predict(X_test)

    return accuracy_score(y_test, y_pred)


def _compute_f1(model, X_test, y_test, **kwargs):
    """
    Computes f1_score for label 1(only for binary class)
    :param model:
    :param X_test: test dataset
    :param y_test: test dataset target
    :return:
    """
    try:
        y_pred = model.predict(X_test.toarray())
    except TypeError:
        y_pred = model.predict(X_test)

    score = f1_score(y_test, y_pred, ** kwargs)
    return score


def _compute_roc(model, X_test, y_test, **kwargs):
    """
    Computes roc for label 1(only for binary class)
    :param model:
    :param X_test: test dataset
    :param y_test: test dataset target
    :return:
    """
    if hasattr(model, 'decision_function') and \
            callable(getattr(model, 'decision_function')):
        try:
            probas_ = model.decision_function(X_test.toarray())
        except TypeError:
            probas_ = model.decision_function(X_test)
    else:
        try:
            probas_ = model.predict_proba(X_test.toarray())
        except TypeError:
            probas_ = model.predict_proba(X_test)

    score = plot_roc(y_test, probas_)
    return score

def _compute_gain(model, X_test, y_test, **kwargs):
    """
    Computes gain curve for label 1(only for binary class)
    :param model:
    :param X_test: test dataset
    :param y_test: test dataset target
    :return:
    """
    if hasattr(model, 'decision_function') and \
            callable(getattr(model, 'decision_function')):
        try:
            probas_ = model.decision_function(X_test.toarray())
        except TypeError:
            probas_ = model.decision_function(X_test)
    else:
        try:
            probas_ = model.predict_proba(X_test.toarray())
        except TypeError:
            probas_ = model.predict_proba(X_test)

    score = plot_cum_gain(y_test, probas_, **kwargs)
    return score


def _compute_lift(model, X_test, y_test, **kwargs):
    """
    Computes lift for label 1(only for binary class)
    :param model:
    :param X_test: test dataset
    :param y_test: test dataset target
    :return:
    """
    if hasattr(model, 'decision_function') and \
            callable(getattr(model, 'decision_function')):
        try:
            probas_ = model.decision_function(X_test.toarray())
        except TypeError:
            probas_ = model.decision_function(X_test)
    else:
        try:
            probas_ = model.predict_proba(X_test.toarray())
        except TypeError:
            probas_ = model.predict_proba(X_test)

    score = plot_lift(y_test, probas_, **kwargs)
    return score
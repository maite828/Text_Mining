#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import matplotlib.pyplot as pl
import numpy as np
from sklearn.metrics import roc_curve, auc
from sklearn.utils.validation import column_or_1d, check_arrays
from numpy.core.numeric import isclose
from numpy import percentile

def plot_score(scorings, xvals, legends, score_func_name, eval_func='F1_score'):
    """
    Plots a curve for each classifier. Accuracies base on num_attr
    :param accuracies: matrix with accuracies shape(classifiers, num_attr)
    :param xvals: num_attrs
    :param legends: legends
    :param func_name: score_func
    :return:
    """
    for i in range(0, scorings.shape[0]):
        pl.plot(xvals, scorings[i, :], marker='x', label=legends[i])
    pl.xlabel("Number of features")
    pl.ylabel(eval_func)
    pl.title("Different classifiers using [" + score_func_name + "] score func")
    pl.legend(legends, loc='upper left')
    pl.show()


def plot_roc(y_test, probas_, **kwargs):
    # Compute ROC curve and area the curve
    probas = probas_[:, 1] if probas_.shape[1] > 1 else probas_[:, 0]
    fpr, tpr, thresholds = roc_curve(y_test, probas, **kwargs)
    roc_auc = auc(fpr, tpr)
    print "Area under the ROC curve : %f" % roc_auc

    # Plot ROC curve
    pl.clf()
    pl.plot(fpr, tpr, marker='x', label='ROC curve (area = %0.2f)' % roc_auc)
    pl.plot([0, 1], [0, 1], marker='x', label='Baseline')
    pl.xlim([0.0, 1.0])
    pl.ylim([0.0, 1.0])
    pl.xlabel('False Positive Rate')
    pl.ylabel('True Positive Rate')
    pl.title('Receiver operating characteristic example')
    pl.legend(loc="lower right")
    pl.show()

    return roc_auc


def gain_curve(y_true, y_score, pos_label=None):
    y_true, y_score = check_arrays(y_true, y_score)
    y_true = column_or_1d(y_true)
    y_score = column_or_1d(y_score)
    # ensure binary classification if pos_label is not specified
    classes = np.unique(y_true)
    if (pos_label is None and
        not (np.all(classes == [0, 1]) or
             np.all(classes == [-1, 1]) or
             np.all(classes == [0]) or
             np.all(classes == [-1]) or
             np.all(classes == [1]))):
        raise ValueError("Data is not binary and pos_label is not specified")
    elif pos_label is None:
        pos_label = 1.

    # sort scores and corresponding truth values. -1 to invert order
    desc_score_indices = np.argsort(y_score, kind="mergesort")[::-1]
    y_score = y_score[desc_score_indices]
    y_true = y_true[desc_score_indices]

    # make y_true a boolean vector to deal with
    y_true = (y_true == pos_label)

    # accumulate the true positives with decreasing threshold
    total = len(y_true)
    _, total_target = np.bincount(y_true)
    tps_cum = np.cumsum(y_true)
    percentages = np.asarray(range(10, 110, 10))
    percentages_indices = np.divide(np.multiply(total, percentages), 100) - 1
    tps_cum_percentages = np.divide(np.multiply(100,
                                                tps_cum[percentages_indices]),
                                    total_target)

    # Add an extra threshold position if necessary
    percentages = np.r_[0, percentages]
    tps_cum_percentages = np.r_[0, tps_cum_percentages]
    # print 'Indexes of customers on the first percentil'
    # for idx in range(0,percentages_indices[0]):
    #     if y_true[idx]:
    #         print str(desc_score_indices[idx])

    return percentages, tps_cum_percentages


def plot_cum_gain(y_test, probas_, pos_label=None ):
    probas = probas_[:, 1] if probas_.shape[1] > 1 else probas_[:, 0]
    percentages, tps_cum = gain_curve(y_test, probas, pos_label)

    gain_auc = auc(np.true_divide(percentages, 100), np.true_divide(tps_cum, 100))
    pl.clf()
    pl.plot(percentages, tps_cum, marker='x', label='Gain chart (area = %0.2f)' % gain_auc)
    pl.plot([0, 100], [0, 100], marker='x', label='Baseline')
    pl.xlim([0.0, 100.0])
    pl.ylim([0.0, 100.0])
    pl.xlabel('% Customers contacted')
    pl.ylabel('% Positive responses')
    pl.title('Cumulative Gains Chart')
    pl.legend(loc="lower right")
    pl.show()

    return gain_auc

def plot_lift(y_test, probas_, pos_label=None ):
    probas = probas_[:, 1] if probas_.shape[1] > 1 else probas_[:, 0]
    percentages, tps_cum = gain_curve(y_test, probas, pos_label)
    lift_percentages = np.true_divide(tps_cum[1:], percentages[1:])

    ylim_max = np.amax(lift_percentages)
    pl.clf()
    pl.plot(percentages[1:], lift_percentages, marker='x', label='Lift chart')
    pl.plot(percentages[1:], np.ones(10), marker='x', label='Baseline')
    pl.xlim([10.0, 100.0])
    pl.ylim([0.0, ylim_max + 1])
    pl.xlabel('% Customers contacted')
    pl.ylabel('% Lift')
    pl.title('Lift Chart')
    pl.legend(loc="lower right")
    pl.show()

    return lift_percentages[0]


def show_confusion_matrix(confusion_matrix, labels):
    norm_conf = []
    for i in confusion_matrix:
        tmp_arr = []
        a = sum(i, 0)
        for j in i:
            if a == 0:
                tmp_arr.append(float(0))
            else:
                tmp_arr.append(float(j) / float(a))
        norm_conf.append(tmp_arr)

    fig = pl.figure()
    pl.clf()
    ax = fig.add_subplot(111)
    ax.set_aspect(1)
    res = ax.imshow(np.array(norm_conf), cmap=pl.cm.jet, interpolation='nearest')
    fig.colorbar(res)
    width = len(confusion_matrix)
    height = len(confusion_matrix[0])
    for x in xrange(width):
        for y in xrange(height):
            ax.annotate(str(confusion_matrix[x][y]), xy=(y, x),
                        horizontalalignment='center',
                        verticalalignment='center')
    pl.ylabel('True label')
    pl.xlabel('Predicted label')
    ax.set_xticklabels(labels, rotation=45)
    pl.xticks(range(width), labels[:width])
    pl.yticks(range(height), labels[:height])
    #plt.savefig('confusion_matrix.png', format='png')
    pl.show()

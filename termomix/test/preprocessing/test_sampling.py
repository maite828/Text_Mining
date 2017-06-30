#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

import unittest

from preprocessing.sampling import StratifiedSamplingKFold

import numpy as np

class StratifiedSamplingKFoldTests(unittest.TestCase):

    def test_undersample_when_balanced(self):
        X = np.array([[1, 2], [3, 4], [1, 2], [3, 4]])
        y = np.array([0, 0, 1, 1])
        skf = StratifiedSamplingKFold(y, n_folds=2, method="undersample", scaling=1.0)

        self.assertEqual(2, len(skf))

        for train_index, test_index in skf:
            print("TRAIN:", train_index, "TEST:", test_index)
            y_train, y_test = y[train_index], y[test_index]

        self.assertIn(1, y_train)
        self.assertIn(0, y_train)

        self.assertIn(1, y_test)
        self.assertIn(0, y_test)


    def test_undersample_when_unbalanced(self):
        X = np.array([[1, 2], [3, 4], [1, 2], [3, 4],[1, 2], [3, 4], [1, 2], [3, 4], [1, 2], [3, 4]])
        y = np.array([0, 0, 0, 0, 0, 0, 0, 0, 1, 1])
        skf = StratifiedSamplingKFold(y, n_folds=2, method="undersample", scaling=1.0)

        self.assertEqual(2, len(skf))

        for train_index, test_index in skf:
            print("TRAIN:", train_index, "TEST:", test_index)
            y_train, y_test = y[train_index], y[test_index]

        self.assertEqual(len(y_train), 2)
        self.assertIn(1, y_train)
        self.assertIn(0, y_train)

        self.assertEqual(len(y_test), 2)
        self.assertIn(1, y_test)
        self.assertIn(0, y_test)
#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/09/2014

import unittest

from visualizations.metrics_evaluation import gain_curve
import numpy as np

class GainCurveTest(unittest.TestCase):

    def test_gain_curve(self):
        y_true = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0]
        y_score = [0.9, 0.8, 0.7, 0.5, 0.3, 0.4, 0.5, 0.6, 0.6, 0.6]
        expected = ([10, 20, 30, 40, 50, 60, 70, 80, 90, 100],
                    [1.0, 2.0, 3.0, 4.0, 5.0, 5.0, 5.0, 6.0, 6.0, 6.0])
        result = gain_curve(y_true, y_score)
        self.assertEqual(expected[0], result[0].tolist())
        self.assertEqual(expected[1], result[1].tolist())

    def test_check_arrays(self):
        y_true = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0]
        y_score = [0.9, 0.8, 0.7, 0.5, 0.3, 0.4, 0.5, 0.6, 0.6]

        self.assertRaises(ValueError, gain_curve, y_true, y_score)

    def test_not_binary_target(self):
        y_true = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0]
        y_score = [0.9, 0.8, 0.7, 0.5, 0.3, 0.4, 0.5, 0.6, 0.6, 0.4]

        self.assertRaises(ValueError, gain_curve, y_true, y_score)

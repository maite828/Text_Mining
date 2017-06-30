#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: zoraida
# Created: 01/04/2014

from sklearn.base import TransformerMixin
import numpy as np


class GetItemTransformer(TransformerMixin):

    def __init__(self, fields_indices=None):
        self.fields_indices = fields_indices
    # assume default fit()

    def fit_transform(self, X, y=None, **fit_params):
        return self.transform(X)

    def transform(self, X, copy=False):
        if not hasattr(X, 'dtype'):
            X = np.asarray(X)

        if self.fields_indices == None:
            return X
        else:
            return X[self.fields_indices]

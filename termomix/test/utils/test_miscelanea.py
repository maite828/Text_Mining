#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'fmrm'

import unittest
from utils.miscellanea import is_unicode, get_words_count, subtract_words_count
from utils.miscellanea import SUBTRACTION_ABSOLUTE, SUBTRACTION_POSITIVE, SUBTRACTION_INTEGER


class IsUnicodeTests(unittest.TestCase):
    def test_string_is_unicode(self):
        result = is_unicode(u"Español")
        self.assertTrue(result)

    def test_string_is_not_unicode(self):
        result = is_unicode("English")
        self.assertFalse(result)

    def test_other_type_is_not_unicode(self):
        result = is_unicode(999)
        self.assertFalse(result)


class GetWordsCountTests(unittest.TestCase):
    words_list = ["One", "Two", "Three", "Two", "Three", "Three"]
    u_words_list = [u"One", u"Two", u"Three", u"Two", u"Three", u"Three"]

    def test_unordered_count(self):
        result = get_words_count(self.words_list)
        self.assertItemsEqual([("Two", 2), ("One", 1), ("Three", 3)], result)

    def test_unicode_unordered_count(self):
        result = get_words_count(self.u_words_list)
        self.assertItemsEqual([(u"Two", 2), (u"One", 1), (u"Three", 3)], result)

    def test_ordered_count(self):
        result = get_words_count(self.words_list, sort=True)
        self.assertItemsEqual([("One", 1), ("Two", 2), ("Three", 3)], result)

    def test_unicode_ordered_count(self):
        result = get_words_count(self.u_words_list, sort=True)
        self.assertItemsEqual([(u"One", 1), (u"Two", 2), (u"Three", 3)], result)

    def test_reverse_ordered_count(self):
        result = get_words_count(self.words_list, sort=True, reverse_sort=True)
        self.assertItemsEqual([("Three", 3), ("Two", 2), ("One", 1)], result)

    def test_unicode_reverse_ordered_count(self):
        result = get_words_count(self.u_words_list, sort=True, reverse_sort=True)
        self.assertItemsEqual([(u"Three", 3), (u"Two", 2), (u"One", 1)], result)


class SubtractWordsCountTests(unittest.TestCase):
    wc_1 = [("One", 1),  ("Two", 2), ("Three", 3), ("Four", 4)]
    wc_2 = [("One", 4),  ("Two", 2), ("Three", 1)]

    def test_subtract_positive_reverse_sort(self):
        result = subtract_words_count(self.wc_1, self.wc_2, mode=SUBTRACTION_POSITIVE, sort=True, reverse_sort=True)
        self.assertEqual([("Four", 4), ("Three", 2)], result)

    def test_subtract_positive_unordered(self):
        result = subtract_words_count(self.wc_1, self.wc_2, mode=SUBTRACTION_POSITIVE)
        self.assertItemsEqual([("Three", 2), ("Four", 4)], result)

    def test_subtract_absolute_reverse_sort(self):
        result = subtract_words_count(self.wc_1, self.wc_2, mode=SUBTRACTION_ABSOLUTE, sort=True, reverse_sort=True)
        self.assertEqual([("Four", 4), ("One", 3), ("Three", 2),  ("Two", 0)], result)

    def test_subtract_absolute_unordered(self):
        result = subtract_words_count(self.wc_1, self.wc_2, mode=SUBTRACTION_ABSOLUTE)
        self.assertItemsEqual([("One", 3),  ("Two", 0), ("Three", 2), ("Four", 4)], result)

    def test_subtract_integer_natural_sort(self):
        result = subtract_words_count(self.wc_1, self.wc_2, mode=SUBTRACTION_INTEGER, sort=True, reverse_sort=False)
        self.assertEqual([("One", -3), ("Two", 0), ("Three", 2), ("Four", 4)], result)

    def test_subtract_integer_unordered(self):
        result = subtract_words_count(self.wc_1, self.wc_2, mode=SUBTRACTION_INTEGER)
        self.assertItemsEqual([("One", -3), ("Two", 0), ("Three", 2), ("Four", 4)], result)


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'fmrm'

import unittest
from preprocessing.text.text_transformations import get_words, replace_regex, replace_numbers


class GetWordsTests(unittest.TestCase):
    input_str = u"Párrafo en español: lowercase and UPPERCASE!!"

    def test_get_words_list(self):
        result = get_words(self.input_str)
        self.assertEqual([u"Párrafo", u"en", u"español", u"lowercase", u"and", u"UPPERCASE"], result)

    def test_get_lowercase_words_list(self):
        result = get_words(self.input_str, lowercase=True)
        self.assertEqual([u"párrafo", u"en", u"español", u"lowercase", u"and", u"uppercase"], result)

    def test_get_words_list_with_punctuation(self):
        result = get_words(self.input_str, remove_punctuation=False)
        self.assertEqual([u"Párrafo", u"en", u"español", u":", u"lowercase", u"and", u"UPPERCASE", u"!", u"!"], result)


class ReplaceRegexTests(unittest.TestCase):
    input_str = "H3110 W0r1d!!"

    def test_replace_one_regex(self):
        result = replace_regex(self.input_str, [[r"[0-9]", ""]])
        self.assertEqual("H Wrd!!", result)

    def test_replace_two_regex(self):
        result = replace_regex(self.input_str, [[r"[A-Za-z]", "x"], [r"[0-9]", "#"]])
        self.assertEqual("x#### x#x#x!!", result)


class ReplaceNumbersTests(unittest.TestCase):
    input_str = "One, 2, Ten times, 5 veces."

    def test_delete_figures(self):
        result = replace_numbers(self.input_str)
        expected = "One, , Ten times,  veces."
        self.assertEqual(expected, result)

    def test_replace_figures(self):
        result = replace_numbers(self.input_str, replace_str="xxx")
        expected = "One, xxx, Ten times, xxx veces."
        self.assertEqual(expected, result)


if __name__ == '__main__':
    unittest.main()

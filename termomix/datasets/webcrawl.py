#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: fmrm
# Created: 11/05/2014

""""
Dataset representation for web crawl.
"""
import itertools
import csv
import codecs

from urlparse import urlparse
from datasets.base_dataset import BaseDataset

import numpy as np


class WebCrawlDataset(BaseDataset):

    def __init__(self, configuration):
        BaseDataset.__init__(self, configuration)

    def load_dataset(self):
        encoding = ('encoding' in self.configuration and self.configuration['encoding']) \
                   or BaseDataset.DEFAULT_ENCODING
        categories_file = self.configuration['categories_csv']
        skip_categories_filter = self.configuration['skip_categories']
        categories_level = self.configuration['categories_level']
        language_filter = self.configuration['language_filter']
        language_trust_filter = self.configuration['language_trust_filter']
        source_file = self.configuration['data_path'] + "/" + self.configuration['train_src_names'][0]

        dict_categories = WebCrawlDataset._load_categories(categories_file, skip_categories_filter, categories_level)
        domains_w_category = dict_categories.keys()
        domains_wo_category = set()
        dict_X = {}
        num_urls = 0
        num_filtered_urls = 0

        print("Reading URLs from file: %s" % source_file)
        with codecs.open(source_file, encoding=encoding, mode='rb') as f:
            # Iterate over groups of 5 lines
            for webpage in itertools.izip(*[f]*5):
                num_urls += 1
                language = webpage[2].rstrip()
                lang_trust = int(webpage[3])
                # Filter by language
                if (language != language_filter) or (lang_trust < language_trust_filter):
                    continue
                url = webpage[0].rstrip()
                domain = urlparse(url)[1]
                # For training we only use domains with known category
                if domain not in domains_w_category:
                    domains_wo_category.add(domain)
                    continue
                text = webpage[1].rstrip()
                # Applies all text transformations (replace mentions, urls, hashtags, lemmatizer, break lines ...)
                num_filtered_urls += 1
                WebCrawlDataset._text_transformation(text)
                if domain in dict_X:
                    dict_X[domain] = dict_X[domain] + text
                else:
                    dict_X[domain] = text

        print("Read %d urls" % num_urls)
        print("Filtered %d URLs (language=%s; language trust>=%d)" %
              (num_filtered_urls, language_filter, language_trust_filter))
        print("Discarded %d URLs from domains in excluded categories or without category" % len(domains_wo_category))
        #print("Discarded domains without category: %s" % list(domains_wo_category))
        print("Loaded texts from %d domains" % len(dict_X))

        # Append texts from domain
        self.X_train.append(dict_X.values())
        # Append category from domain and convert to numpy array
        for domain in dict_X.keys():
            self.y_train.append(dict_categories[domain])
        self.y_train = np.array(self.y_train)
        # Number of domains
        self.n_instances = len(self.X_train[0])

    @staticmethod
    def _text_transformation(text):
        """
        This function applies all required transformations to the text of the tweet.
        Lemmatizer will be called from here.
        :param text: json with tweet
        """

    @staticmethod
    def _load_categories(categories_file, skip_categories, level=0):
        """
        Reads the categories from a domain/category CSV file and loads y_train (categories) matching X_names (domains).
        When a domain belongs to more than one category, the last category found is chosen.
        :param categories_file: CSV with pairs domain-category (a domain may belong to several categories)
        :param skip_categories: List with categories to ignore (not loaded)
        :param level: Depth level of the categories. Default value 0 means categories are taken as read from CSV file
        :return: Numpy array of categories matching the input domains
        """
        # TODO: deal with domains belonging to more than one category
        # File contents are kept in an domain:category dictionary
        dict_categories = {}
        num_rows = 0
        num_discarded = 0
        print("Reading domain-categories CSV file")
        reader = csv.reader(open(categories_file, mode='rb'), delimiter=',', quotechar='"')
        for row in reader:
            num_rows += 1
            domain = row[0]
            category = row[1]
            # Do not add domain-category when category is in the skip list
            if category in skip_categories:
                num_discarded += 1
                continue
            if level != 0:
                category_levels = category.split("/")
                category = "/".join(category_levels[:level])
            # When domain belongs to more than one category, keep the last ocurrence
            dict_categories[domain] = category
        # Print statistics
        print("Read %d domain-category pairs" % num_rows)
        print("Discarded %d domains in categories %s" % (num_discarded, skip_categories))
        print("Unique domains: %d" % len(dict_categories))
        categories = dict_categories.values()
        unique_categories = list(set(categories))
        print("There are %d unique categories of level %d:" % (len(unique_categories), level))
        unique_categories.sort()
        print("\n".join(unique_categories))
        """
        # This code is slow. Use it for debug only
        for category in unique_categories:
            print("%s - %d domains" % (category, len([cat for cat in categories if cat == category])))
        """

        return dict_categories

#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: fmrm
# Created: 22/07/2014
"""
"""
import pytagcloud as wc

import argparse
import sys
import logging
import importlib
import numpy as np

from preprocessing.text.text_transformations import replace_numbers
from preprocessing.text.stopwords import StopWords
from preprocessing import dtm
from visualizations import excel
from sklearn.feature_extraction.text import CountVectorizer

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')


class WordCloud(object):
    MULTISOURCE_ENUMERATE = 0
    MULTISOURCE_COMPARE = 1

    configuration = None
    dataset = None
    stopwords_remover = None
    dtm_matrices = []
    dtm_col_names = []

    def __init__(self, dataset_name):
        """
        Loads the wordcloud configuration for a given dataset
        """
        cfg_name = dataset_name + '_cfg'
        module_name = 'configuration.' + cfg_name
        config_module = importlib.import_module(module_name)
        self.configuration = config_module.get_wordcloud_cfg()

        # Initialize stopwords remover class
        conf_language = self.configuration['stopwords_lang']
        if conf_language is None:
            self.stopwords_remover = None
            logging.info("Stop words removal is not configured and will be skipped")
        else:
            conf_stopwords_files = self.configuration['stopwords_files']
            self.stopwords_remover = StopWords(conf_language, conf_stopwords_files)

    def load_dataset(self):
        """
        Create and load the dataset based on its configuration
        """
        self.dataset = self.configuration['dataset_class'](self.configuration)
        self.dataset.load_dataset()
        # Load texts sources in a dtm each
        vec = CountVectorizer()
        for index, src in enumerate(self.configuration['train_src_names']):
            logging.debug("Creating DTM for source %s" % src)
            # This returns a csc sparse matrix
            frequencies_dtm = vec.fit_transform(self.dataset.get_texts(index))
            self.dtm_matrices.append(frequencies_dtm)
            self.dtm_col_names.append(np.array(vec.get_feature_names()))

    def create_wordcloud_file(self, tags, output_file):
        # Get configuration parameters
        conf_num_tags = self.configuration['num_tags']
        conf_min_tag_size = self.configuration['min_tag_size']
        conf_max_tag_size = self.configuration['max_tag_size']
        conf_image_size = self.configuration['image_size']
        conf_font = self.configuration['font']
        conf_background = self.configuration['background']

        logging.info("Creating wordcloud image file: %s" % output_file)
        # Limit the tags to be displayed to those appearing more frequently
        tags = tags[:conf_num_tags]
        # Create the image
        tags = wc.make_tags(tags, minsize=conf_min_tag_size, maxsize=conf_max_tag_size)
        # Save image to file
        wc.create_tag_image(tags, output_file, size=conf_image_size, fontname=conf_font,
                            layout=wc.LAYOUT_HORIZONTAL, background=conf_background)
        logging.info("Created wordcloud image file: %s" % output_file)
        print("Created wordcloud image file: %s" % output_file)

    def create_wordcloud_for_dtm(self, freq_dtm, dtm_col_names, out_path):
        """
        Given a DTM, create a wordcloud file
        :param freq_dtm: Document Term Matrix to be displayed as a wordcloud
        :param dtm_col_names: Column names (terms) of the DTM
        :param out_path: Path to the output file with the wordcloud image
        """
        # Get ordered tags by frequency
        # We use our own implementation, because pytagcloud.get_tag_counts has a poor performance
        words = dtm.get_term_freq(freq_dtm, dtm_col_names, sort=dtm.SORT_ON_FREQUENCY, reverse_sort=True)
        # Output filename is the same as input without the extension
        logging.debug("Term frequency (first 10): %s" % words[:10])
        self.create_wordcloud_file(words, out_path)

    def preprocess_source_dtms(self):
        for index, src in enumerate(self.configuration['train_src_names']):
            self.dtm_matrices[index], self.dtm_col_names[index] = \
                self.preprocess_source_dtm(self.dtm_matrices[index], self.dtm_col_names[index])

    def preprocess_source_dtm(self, source_dtm, dtm_col_names):
        # Do not consider numbers in text for the wordcloud
        for index, term in enumerate(dtm_col_names):
            dtm_col_names[index] = replace_numbers(term)
        # Remove stop words for all DTMs
        if self.stopwords_remover is not None:
            source_dtm, dtm_col_names = self.stopwords_remover.remove_sw_from_dtm(source_dtm, dtm_col_names)
        return source_dtm, dtm_col_names

    def create_wordclouds_for_sources(self):
        """
        Creates one wordcloud file for each source document
        """
        for index, src in enumerate(self.configuration['train_src_names']):
            logging.info("...Wordcloud generation for source '%s' " % src)

            # Output filename is the same as input without the extension
            path = self.configuration['output_path']
            filename = ".".join(src.split(".")[:-1]) + ".png"
            self.create_wordcloud_for_dtm(self.dtm_matrices[index], self.dtm_col_names[index], path + "/" + filename)

    def create_comparison_wordclouds(self):
        """
        Creates two wordclouds with the subtraction, in both ways, of two given sources
        """
        path = self.configuration['output_path']
        sources = self.configuration['train_src_names']
        max_freq_terms = self.configuration['max_freq_terms']
        for iteration in range(len(sources) / 2):
            idx1, idx2 = iteration * 2, iteration * 2 + 1
            logging.info("...Wordcloud comparation of sources '%s' and '%s' " % (sources[idx1], sources[idx2]))
            # Use source files without extension as basename
            basename_1 = ".".join(sources[idx1].split(".")[:-1])
            basename_2 = ".".join(sources[idx2].split(".")[:-1])

            # First we transform the frequencies as a percentage of the total, so we can compare the two data sets
            self.dtm_matrices[idx1] = dtm.set_frequency_as_percentage(self.dtm_matrices[idx1])
            self.dtm_matrices[idx2] = dtm.set_frequency_as_percentage(self.dtm_matrices[idx2])

            '''
            # Uncomment for debug or exploration: get tags ordered by frequency for each source
            words1 = dtm.get_term_freq(self.dtm_matrices[idx1], self.dtm_col_names[idx1],
                                       sort=dtm.SORT_ON_FREQUENCY, reverse_sort=True)
            words2 = dtm.get_term_freq(self.dtm_matrices[idx2], self.dtm_col_names[idx2],
                                       sort=dtm.SORT_ON_FREQUENCY, reverse_sort=True)
            logging.debug("Len words source(%d) = %d: " % (idx1, len(words1)))
            logging.debug("First 10 words from source(%d): %s" % (idx1, words1[:10]))
            logging.debug("Len words source(%d) = %d: " % (idx2, len(words2)))
            logging.debug("First 10 words from source(%d): %s" % (idx2, words2[:10]))
            '''

            # TODO: put duplicate code in one function
            # Calculate the differences
            subtract_dtm = dtm.subtract_dtm_frequencies(self.dtm_matrices[idx1], self.dtm_col_names[idx1],
                                                        self.dtm_matrices[idx2], self.dtm_col_names[idx2])
            words = dtm.get_term_freq(subtract_dtm, self.dtm_col_names[idx1], sort=dtm.SORT_ON_FREQUENCY, reverse_sort=True)
            logging.debug("Length source(%d) = %d" % (idx1, len(self.dtm_col_names[idx1])))
            logging.debug("Length source(%d) - source(%d) = %d: " % (idx1, idx2, len(words)))
            logging.debug("First 10 words from subtraction: %s" % words[:10])
            wc_file = "%s/%s_minus_%s.png" % (path, basename_1, basename_2)
            self.create_wordcloud_file(words, wc_file)
            # TODO: set how many words to track in configuration file
            xlsx_file = "%s/%s_minus_%s.xlsx" % (path, basename_1, basename_2)
            excel.create_docterm_statistics_file(xlsx_file, np.array(words)[:max_freq_terms, 0],
                                                 self.dataset.get_texts(idx1), image=wc_file)

            # Now the same, in the other way
            subtract_dtm = dtm.subtract_dtm_frequencies(self.dtm_matrices[idx2], self.dtm_col_names[idx2],
                                                        self.dtm_matrices[idx1], self.dtm_col_names[idx1])
            words = dtm.get_term_freq(subtract_dtm, self.dtm_col_names[idx2], sort=dtm.SORT_ON_FREQUENCY, reverse_sort=True)
            logging.debug("Length source(%d) = %d" % (idx2, len(self.dtm_col_names[idx2])))
            logging.debug("Length source(%d) - source(%d) = %d: " % (idx2, idx1, len(words)))
            logging.debug("First 10 words from subtraction: %s" % words[:10])
            wc_file = "%s/%s_minus_%s.png" % (path, basename_2, basename_1)
            self.create_wordcloud_file(words, wc_file)
            # TODO: set how many words to track in configuration file
            xlsx_file = "%s/%s_minus_%s.xlsx" % (path, basename_2, basename_1)
            excel.create_docterm_statistics_file(xlsx_file, np.array(words)[:max_freq_terms, 0],
                                                 self.dataset.get_texts(idx2), image=wc_file)

    def generate_wc(self):
        """
        Calls the appropriate generator based on the configuration properties
        """
        if self.configuration['multi_src'] == WordCloud.MULTISOURCE_ENUMERATE:
            self.create_wordclouds_for_sources()
        else:
            self.create_comparison_wordclouds()


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("dataset_name", help="Dataset. Options are: [twitter|webcrawl|peru|lleida]")
    args = parser.parse_args()

    wordcloud_generator = WordCloud(args.dataset_name)
    wordcloud_generator.load_dataset()
    wordcloud_generator.preprocess_source_dtms()
    wordcloud_generator.generate_wc()


if __name__ == '__main__':
    main(sys.argv[1:])

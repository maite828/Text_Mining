#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: Zoraida Hidalgo Sanchez
#         zoraida.hidalgosanchez@telefonica.com
# Created: 01/11/2015
"""
    This module is a client that calls to the freeling python wrapper.
    See:
    http://nlp.lsi.upc.edu/freeling
"""
import logging.config

import freeling


class FreelingClient(object):

    def __init__(self, language, freeling_dir, options):
        self.logger = logging.getLogger(__name__)
        self.logger.info('FREELING_DIR: %s', freeling_dir)
        self.logger.info('LANGUAGE: %s', language)
        self.logger.info('MAPO_OPTIONS: %s', options)

        freeling.util_init_locale("default")

        # create options set for maco analyzer. Default values are Ok, except for data files.
        data_dir = freeling_dir + "/share/freeling/"
        maco_opts = freeling.maco_options(language)
        maco_opts.set_data_files("", 
                          data_dir + "/common/punct.dat",
                          data_dir + language + "/dicc.src",
                          data_dir + language + "/afixos.dat",
			  "", 
                          data_dir + language + "/locucions.dat",
                          data_dir + language + "/np.dat", 
                          data_dir + language + "/quantities.dat",
                          data_dir + language + "/probabilitats.dat")

        # create analyzers
        self.tokenizer = freeling.tokenizer(data_dir + language + "/tokenizer.dat")
        self.splitter = freeling.splitter(data_dir + language + "/splitter.dat")
        self.maco = freeling.maco(maco_opts)
	self.maco.set_active_options(*options)

    def analyze(self, text):
	"""
        Given a text, this function returns a dict with the keys:
            form lemma prob sentence_idx tag word_idx
        where
            form is the word
            lemma is the lemma
            prob is the probability of being the PoS on tag
            sentece_idx index of the sentence that the words belongs to(from 0)
            tag is the PoS for the word
            word_idx index of the word into the sentence(from 0)
    """

        text_tk = self.tokenizer.tokenize(text)
        text_spl = self.splitter.split(text_tk)
        sentences = self.maco.analyze(text_spl)
            
        sentence_idx_list = []
        word_idx_list = []
        form_list = []
        lemma_list = []
        tag_list = []
        prob_list = []
        for s_idx, s in enumerate(sentences):
            words = s.get_words()
            for w_idx, w in enumerate(words):
                # would be nice to explore splitter and remove dots
                if w.get_form() != '.':
                    sentence_idx_list.append(s_idx)
                    word_idx_list.append(w_idx)
                    form_list.append(w.get_form())
                    w_analysis = w.get_analysis()[0]

                    lemma_list.append(w_analysis.get_lemma())
                    tag_list.append(w_analysis.get_tag())
                    prob_list.append(w_analysis.get_prob())

        analyzed_text = dict(sentence_idx=sentence_idx_list,
                             word_idx=word_idx_list,
                             form=form_list,
                             lemma=lemma_list,
                             tag=tag_list,
                             prob=prob_list)

        self.logger.debug('Analized text: %s', analyzed_text)
        return analyzed_text


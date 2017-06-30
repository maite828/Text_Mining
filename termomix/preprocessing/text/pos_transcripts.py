#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: Zoraida Hidalgo Sanchez
#         zoraida.hidalgosanchez@telefonica.com
# Created: 01/11/2015

"""
This module processes a set of transcripts in a given directory.
Specifically, it extends word information by appending:
form lemma prob sentence_idx tag word_idx
where
    form is the word
    lemma is the lemma
    prob is the probability of being the PoS on tag
    sentece_idx index of the sentence that the words belongs to(from 0)
    tag is the PoS for the word
    word_idx index of the word into the sentence(from 0)
"""
from freeling_client import FreelingClient
import configargparse
import logging.config
from os.path import exists, dirname, join
from os import listdir
import pandas as pd


def remove_tags(transcripts_df, tag_list):
    """

    Parameters
    ----------
    transcripts_df: ctm dataframe
    tag_list: [<garbage>, <noise>]

    Returns
    -------

    """
    return transcripts_df[~transcripts_df['word'].isin(tag_list)]


def get_transcript_by_turns(transcript_df):
    """

    Parameters
    ----------
    transcript_df

    Returns
    -------

    """
    text = ''
    current_locutor = transcript_df['audioid_channel'].iloc[0].split('_')[1]
    #transcript-header = audioid_channel, start_sec, duration, word, precision

    for id, start, dur, word, prec in transcript_df.itertuples(index=False):
        locutor = id.split('_')[1]
        if locutor != current_locutor:
            current_locutor = locutor
            text += '. ' + word
        else:
            text += ' ' + word
    return text


def parse_args():
    parser = configargparse.ArgParser()
    parser.add('-c', '--config', required=False, is_config_file=True, help='Config file path')

    parser.add('--log-config', default='./conf/logging.ini', help='Logging config file path')
    parser.add('-l', '--language', default='es', choices=['es', 'ca', 'en', 'fr', 'it', 'pt'], help='Language')

    parser.add('--freeling-dir', default='/usr/local/freeling', help='Freeling directory')

    parser.add('--user-map', action='store_true', default=False, help='Use User Map module. '
                                                                      'IT Assigns Part-of-Speech tags to words')
    parser.add('--affix-analysis', action='store_true', default=False, help='Use Affixation Rules Submodule of '
                                                                            'Dictionary Search Module.')
    parser.add('--multiwords-detection', action='store_true', default=False, help='Use Multiwords Detection Module. '
                                                                                  'It aggregates input tokens in a '
                                                                                  'single word object if they are found'
                                                                                  ' in a given list of multiwords')
    parser.add('--numbers-detection', action='store_true', default=False, help='Use Number Detection Module. It is '
                                                                               'language dependent: It recognizes '
                                                                               'nummerical expression(e.g.: 1,220.54 or'
                                                                               ' two-hundred sixty-five), and assigns '
                                                                               'them a normalized value as lemma')
    parser.add('--punctuation-detection', action='store_true', default=False, help='Use Punctuation Detection Module. '
                                                                                   'It assigns Part-of-Speech tags '
                                                                                   'to punctuation symbols')
    parser.add('--dates-detection', action='store_true', default=False, help='Use Dates Detection Module')
    parser.add('--quantities-detection', action='store_true', default=False, help='Use Quantitiy Recognition Module')
    parser.add('--dictionary-search', action='store_true', default=False, help='Use Dictionary Search Module. Has two '
                                                                               'functions: Search the word forms in the'
                                                                               ' dictionary to find out their lemmas'
                                                                               'and PoS tags, and apply affixation '
                                                                               'rules to find the same information in '
                                                                               'the cases in which the form is a '
                                                                               'derived form not included in the '
                                                                               'dictionary')
    parser.add('--probability-assignment', action='store_true', default=False, help='Use Probability Assigment Module. '
                                                                                    'It assigns an a priori probability'
                                                                                    ' to each analysis of each word. '
                                                                                    'These probabilities will be needed'
                                                                                    ' for the PoS tagger later.')
    parser.add('--ne-recognition', action='store_true', default=False, help='Use Named Entity Recognition Module')
    parser.add('--compound-analysis', action='store_true', default=False, help='Compound Analysis Module')
    parser.add('--retok-contractions', action='store_true', default=False, help='Retok Contractions Module')
    parser.add('--transcript-dir', required=True, help='Transcripts directory')
    parser.add('--analyzed-transcript-dir', required=True, help='Output dir')
    parser.add('--transcript-header', action='append', help='Transcripts header')
    parser.add('--separator', default=' ', help='Transcripts ctm file separator')
    parser.add('--encoding', default='UTF-8', help='Transcripts ctm file Encoding')
    parser.add('--tag-list', action='append', help='List of tags to be removed')

    args = parser.parse_args()

    if not exists(args.config):
        parser.error("The file '%s' does not exist" % args.config)
    if dirname(args.log_config) and not exists(dirname(args.log_config)):
        parser.error("The directory '%s' does not exist" % dirname(args.log_config))
    if not exists(args.freeling_dir):
        parser.error("The directory '%s' does not exist" % args.freeling_dir)
    if not exists(args.transcript_dir):
        parser.error("The directory '%s' does not exist" % args.transcript_dir) 
    if not exists(args.analyzed_transcript_dir):
        parser.error("The directory '%s' does not exist" % args.analyzed_transcript_dir)

    logging.info(parser.print_values())

    return args


def main():
    # Parse arguments
    args = parse_args()
    logging.config.fileConfig(args.log_config)

    # Ini log
    freeling_client = FreelingClient(args.language, args.freeling_dir, [args.user_map,
								        args.numbers_detection,
                                                                        args.punctuation_detection,
                                                                        args.dates_detection,
                                                                        args.dictionary_search,
                                                                        args.affix_analysis,
									args.compound_analysis,
									args.retok_contractions,
                                                                        args.multiwords_detection,
                                                                        args.ne_recognition,
                                                                        args.quantities_detection,
                                                                        args.probability_assignment])

    for transcript_file in listdir(args.transcript_dir):
        logging.debug('Processing transcript: %s', join(args.transcript_dir, transcript_file))

        transcript_df = pd.read_csv(join(args.transcript_dir, transcript_file), header=None,
                                    names=args.transcript_header,
                                    sep=args.separator, encoding=args.encoding).sort_values(['start_sec'],
                                                                                            ascending=[True])
        transcript_df = remove_tags(transcript_df, args.tag_list)
        text = get_transcript_by_turns(transcript_df)
        analyzed_text_df = pd.DataFrame(freeling_client.analyze(text))
        # for now we cannot concatenate transcript_df with analyzed_text_df since freeling concatenates some words
        # and then matrix does not correlate
        analyzed_text_df.to_csv(join(args.analyzed_transcript_dir, transcript_file), index=False, sep=args.separator,
                        encoding=args.encoding)

if __name__ == "__main__":
    main()

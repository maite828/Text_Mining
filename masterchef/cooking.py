#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Cooks the text
"""
from coo2cfg import delete, keywords, languages, stopwords, steps, text_field_out, text_field_in, url_regex, email_regex
import twittercook
import unicodedata
import re
import string
import json
import sys


def replace(v):
    """
    Replace words in 'wordlist' with 'replace' from 'text'
    """
    for elem in delete:
        v[text_field_out] = re.sub(elem, u'', v[text_field_out])


def stripAccents(v):
    """
    Strip diacritic characters
    """
    v[text_field_out] = ''.join(c for c in unicodedata.normalize('NFD', v[text_field_out]) if unicodedata.category(c) != 'Mn')


def replacePunctuation(v):
    """
    Remplaces !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~ with ' '
    """
    translate_table = dict((ord(char), u' ') for char in string.punctuation)
    v[text_field_out] = v[text_field_out].translate(translate_table)


def stripStopwords(v):
    """
    Strips stopwords base on section languages in config file
    """
    for l in languages:
        v[text_field_out] = ' '.join([w for w in v[text_field_out].split(' ') if not w in stopwords.words(l)])


def buildKeywords(v):
    """
    Builds an array with the keywords contained into the string
    """
    keyword_list = []
    for k in keywords:
        kl = unicode(k, "utf-8").lower()
        if v[text_field_out].find(kl) != -1:
            keyword_list.append(kl)
    if not keyword_list:
        v[text_field_out] = u''
    else:
        v['keywords'] = keyword_list


def replaceUrls(v):
    """
    Remove URLs
    """
    v[text_field_out] = re.sub(url_regex, '', v[text_field_out])


def replaceEmails(v):
    """
    Remove Emails
    """
    v[text_field_out] = re.sub(email_regex, '', v[text_field_out])


process_step = {'diacritic': stripAccents,
                    'punctuation': replacePunctuation,
                    'stopwords': stripStopwords,
                    'keywords': buildKeywords,
                    'processretweets': twittercook.processRetweets,
                    'excluderetweets': twittercook.excludeRetweets,
                    'excludeoficialusers': twittercook.excludeTweetsFromOficialUsers,
                    'hashtags': twittercook.replaceHashtags,
                    'mentions': twittercook.replaceMentions,
                    'urls': replaceUrls,
                    'emails': replaceEmails,
                    'delete': replace}


def mapper(k, v):
    """
    Outputs k and cooked v
    """
    try:
        instance = json.loads(v)
        sys.stderr.write(json.dumps(instance) + '\n')
        instance[text_field_out] = instance[text_field_in].lower()

        for step in steps:
            process_step[step](instance)
            if not instance[text_field_out]:
                break

        if instance[text_field_out]:
            # if keywors provided then one output for each k contained in instance[text_field_in]
            if 'keywords' in instance and instance['keywords']:
                # for keyword in instance['keywords']:
                #yield keyword, instance
                yield instance['keywords'][0], instance
            else:
                yield 'all', instance

    except ValueError as e:
        sys.stderr.write(e.message + '\n')
        sys.stderr.write(v + '\n')


def reducer(k, values):
    """
    Outputs to output/key directory
    """
    for v in values:
        yield (k, json.dumps(v, ensure_ascii=False)), ''


def runner(job):
    opts = [("inputformat", "text"),
            ("outputformat", "text"),
            ("getpath", "yes")
           ]
    job.additer(mapper, reducer, opts=opts)


if __name__ == "__main__":
    from dumbo import main
    main(runner)

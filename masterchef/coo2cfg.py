#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Config File for Masterchef
"""
from ConfigParser import SafeConfigParser
from nltk.corpus import stopwords
from nltk.corpus import WordListCorpusReader
from nltk import data
import zipimport
import os

REGEX_SEPARATOR = '|||'

config = SafeConfigParser()
config.read("masterchef.ini")

URL_REGEX = 'http://[^\s]+'
EMAIL_REGEX = '^[A-Za-z0-9\.\+_-]+@[A-Za-z0-9\._-]+\.[a-zA-Z]*$'
HASHTAG_REGEX = '(\A|\s)#(\w+)'
MENTION_REGEX = '(\A|\s)@(\w+)'

# Loading cooking section
url_regex = (config.has_option('cooking', 'url_regex') and config.get('cooking', 'url_regex')) or URL_REGEX
email_regex = (config.has_option('cooking', 'email_regex') and config.get('cooking', 'email_regex')) or EMAIL_REGEX
hashtag_regex = (config.has_option('cooking', 'hashtag_regex') and config.get('cooking', 'hashtag_regex')) or HASHTAG_REGEX
mention_regex = (config.has_option('cooking', 'mention_regex') and config.get('cooking', 'mention_regex')) or MENTION_REGEX
delete = (config.has_option('cooking', 'delete') and config.get('cooking', 'delete').split(REGEX_SEPARATOR)) or ''
keywords = (config.has_option('cooking', 'keywords') and config.get('cooking', 'keywords').lower().split(',')) or 'movistar'
oficial_users = (config.has_option('cooking', 'oficial_users') and config.get('cooking', 'oficial_users').lower().split(',')) or 'movistar'
languages = (config.has_option('cooking', 'languages') and config.get('cooking', 'languages').lower().split(',')) or 'spanish'
steps = config.get('cooking', 'steps').lower().split(',')
text_field_out = (config.has_option('cooking', 'text_field_out') and config.get('cooking', 'text_field_out').lower()) or ''
text_field_in = (config.has_option('cooking', 'text_field_in') and config.get('cooking', 'text_field_in').lower()) or ''

# Loading serving section
output_fields = (config.has_option('serving', 'output_fields') and config.get('serving', 'output_fields').lower().split(',')) or ''
output_separator = (config.has_option('serving', 'output_separator') and config.get('serving', 'output_separator')) or ','

# Reading corpus
data.path = [os.getcwd()] + data.path
stopwords = WordListCorpusReader(data.GzipFileSystemPathPointer('stopwords.zip'), languages)

# Loading dependencies
yamlImport = zipimport.zipimporter('yaml.zip')
yalm = yamlImport.load_module('yaml')
nltkImport = zipimport.zipimporter('nltk.zip')
nltk = nltkImport.load_module('nltk')

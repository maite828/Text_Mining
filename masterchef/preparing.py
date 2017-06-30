#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Remove duplicated tweets and parses date. Outputs to directories
base on date(by day)
"""
from dumbo import run, opt
import json
import time

INPUT_TIME_FORMAT = '%a %b %d %H:%M:%S +0000 %Y'
OUTPUT_TIME_FORMAT = '%Y-%m-%dT%H:%M:%SZ'


def buildTime(created_at):
    t = time.strptime(created_at, INPUT_TIME_FORMAT)
    return time.strftime(OUTPUT_TIME_FORMAT, t)


def mapper(k, v):
    """
    Outputs tweet identifier as key, tweet as value
    """
    tweet = json.loads(v)
    yield tweet['id_str'], tweet


def reducer(k, values):
    """
    Outputs tweet identifier as key, tweet as value
    Theoretically only mut be one element into unique
    """
    tweet = values.next()
    tweet['created_at'] = buildTime(tweet['created_at'])
    yield ('date=' + tweet['created_at'][:10], json.dumps(tweet)), ''


def runner(job):
    opts = [("inputformat", "text"),
            ("outputformat", "text"),
            ("getpath", "yes")
           ]
    job.additer(mapper, reducer, opts=opts)


if __name__ == "__main__":
    from dumbo import main
    main(runner)

#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Generates a list of candidates to be 'oficial users'.
The purpose of this job is to be able to filter tweets
comming from oficial users(not useful to brand tracker)
"""


def mapper(k, v):
    """
    Value is a tweet.
    If any of the keywors in keywords is contained into the tweet's
    author then the output is keyword, author otherwise skip
    """
    keywords = [u'movistar', u'telefónica', u'telefonica']

    if 'retweeted_status' in v:
        author = unicode(v['retweeted_status']['user']['screen_name'], 'utf-8').lower()
    else:
        author = unicode(v['user']['screen_name'], 'utf-8').lower()

    for k in keywords:
        if author.find(k) != -1:
            yield k, author


def reducer(k, values):
    """
    Outputs the key and the list of authors candidates for that key
    """
    yield k, ','.join(set(values))


def runner(job):
    opts = [("inputformat", "sequencefile"),
            ("outputformat", "sequencefile"),
           ]
    job.additer(mapper, reducer, opts=opts)


if __name__ == "__main__":
    from dumbo import main
    main(runner)
